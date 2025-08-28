#include "CHTLUnifiedScanner.h"
#include <algorithm>
#include <iostream>
#include <cctype>
#include <sstream>
#include "../ThirdParty/CJMODAPI/Syntax.h"
#include "../ThirdParty/CJMODAPI/Arg.h"
#include "../ThirdParty/CJMODAPI/CJMODScannerAPI.h"
#include "../ThirdParty/CJMODAPI/CJMODGenerator.h"

namespace Scanner {

CHTLUnifiedScanner::CHTLUnifiedScanner()
	: currentState_(ScannerState::INITIAL), currentPos_(0), currentLine_(1), currentColumn_(1) {
	initializeConfig();
}

void CHTLUnifiedScanner::setSource(const std::string& source) {
	source_ = source;
	reset();
}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
	fragments_.clear();
	currentState_ = ScannerState::INITIAL;
	while (!isEOF()) { scanNextFragment(); }
	return fragments_;
}

void CHTLUnifiedScanner::reset() {
	currentPos_ = 0;
	currentLine_ = 1;
	currentColumn_ = 1;
	currentState_ = ScannerState::INITIAL;
	fragments_.clear();
}

void CHTLUnifiedScanner::initializeConfig() {
	config_.minFragmentSize = 10;
	config_.maxFragmentSize = 1000;
	config_.lookAheadSize = 100;
	config_.enableSmartCutting = true;
}

void CHTLUnifiedScanner::scanNextFragment() {
	if (isEOF()) return;
	size_t startPos = currentPos_;
	size_t startLine = currentLine_;
	size_t startColumn = currentColumn_;
	size_t fragmentSize = std::min(config_.maxFragmentSize, source_.length() - currentPos_);
	if (config_.enableSmartCutting) {
		size_t optimalCutPoint = findOptimalCutPoint(startPos, fragmentSize);
		if (optimalCutPoint > startPos) fragmentSize = optimalCutPoint - startPos;
	}
	if (fragmentSize < config_.minFragmentSize && !isEOF()) {
		fragmentSize = std::min(config_.minFragmentSize, source_.length() - currentPos_);
	}
	size_t endPos = startPos + fragmentSize;
	CodeFragmentType fragmentType = detectFragmentType(startPos, endPos);
	std::string content = source_.substr(startPos, fragmentSize);
	CodeFragment fragment(fragmentType, content, startPos, endPos, startLine, startColumn);
	fragments_.push_back(fragment);
	currentPos_ = endPos;
	currentColumn_ += fragmentSize;
	setState(ScannerState::INITIAL);
}

CodeFragmentType CHTLUnifiedScanner::detectFragmentType(size_t startPos, size_t endPos) {
	CodeFragmentType type = detectCHTLJSType(startPos, endPos);
	if (type != CodeFragmentType::UNKNOWN) return type;
	type = detectCHTLType(startPos, endPos);
	if (type != CodeFragmentType::UNKNOWN) return type;
	type = detectCSSType(startPos, endPos);
	if (type != CodeFragmentType::UNKNOWN) return type;
	type = detectJSType(startPos, endPos);
	if (type != CodeFragmentType::UNKNOWN) return type;
	return CodeFragmentType::UNKNOWN;
}

CodeFragmentType CHTLUnifiedScanner::detectCHTLType(size_t startPos, size_t endPos) {
	std::string content = source_.substr(startPos, endPos - startPos);
	if (content.find("[Template]") != std::string::npos ||
		content.find("[Custom]") != std::string::npos ||
		content.find("[Origin]") != std::string::npos ||
		content.find("[Import]") != std::string::npos ||
		content.find("[Namespace]") != std::string::npos ||
		content.find("[Configuration]") != std::string::npos ||
		content.find("text") != std::string::npos ||
		content.find("style") != std::string::npos ||
		content.find("script") != std::string::npos) {
		return CodeFragmentType::CHTL;
	}
	return CodeFragmentType::UNKNOWN;
}

CodeFragmentType CHTLUnifiedScanner::detectCHTLJSType(size_t startPos, size_t endPos) {
	std::string content = source_.substr(startPos, endPos - startPos);
	if (content.find("{{") != std::string::npos ||
		content.find("->") != std::string::npos ||
		content.find("&->") != std::string::npos ||
		content.find("vir") != std::string::npos ||
		content.find("listen") != std::string::npos ||
		content.find("delegate") != std::string::npos ||
		content.find("animate") != std::string::npos ||
		content.find("iNeverAway") != std::string::npos ||
		content.find("printMylove") != std::string::npos) {
		return CodeFragmentType::CHTL_JS;
	}
	return CodeFragmentType::UNKNOWN;
}

CodeFragmentType CHTLUnifiedScanner::detectCSSType(size_t startPos, size_t endPos) {
	std::string content = source_.substr(startPos, endPos - startPos);
	if (content.find(":") != std::string::npos && content.find(";") != std::string::npos &&
		(content.find("color") != std::string::npos || content.find("width") != std::string::npos ||
		 content.find("height") != std::string::npos || content.find("background") != std::string::npos ||
		 content.find("margin") != std::string::npos || content.find("padding") != std::string::npos)) {
		return CodeFragmentType::CSS;
	}
	return CodeFragmentType::UNKNOWN;
}

CodeFragmentType CHTLUnifiedScanner::detectJSType(size_t startPos, size_t endPos) {
	std::string content = source_.substr(startPos, endPos - startPos);
	if (content.find("function") != std::string::npos || content.find("const") != std::string::npos ||
		content.find("let") != std::string::npos || content.find("var") != std::string::npos ||
		content.find("console.log") != std::string::npos || content.find("addEventListener") != std::string::npos ||
		content.find("()") != std::string::npos || content.find("=>") != std::string::npos) {
		return CodeFragmentType::JAVASCRIPT;
	}
	return CodeFragmentType::UNKNOWN;
}

size_t CHTLUnifiedScanner::findOptimalCutPoint(size_t startPos, size_t maxSize) {
	size_t endPos = startPos + maxSize;
	if (endPos > source_.length()) return source_.length();
	for (size_t pos = endPos; pos > startPos; --pos) {
		if (isValidCutPoint(pos)) return pos;
	}
	return endPos;
}

bool CHTLUnifiedScanner::isValidCutPoint(size_t pos) {
	if (pos >= source_.length()) return true;
	char ch = source_[pos];
	if (std::isspace(static_cast<unsigned char>(ch))) return true;
	if (ch == ';' || ch == ',' || ch == '}' || ch == ']' || ch == ')') return true;
	size_t startPos = (pos > 50 ? pos - 50 : 0);
	return isCompleteExpression(startPos, pos);
}

bool CHTLUnifiedScanner::isCompleteExpression(size_t startPos, size_t endPos) {
	if (hasUnclosedBrackets(startPos, endPos) || hasUnclosedBraces(startPos, endPos) ||
		hasUnclosedParens(startPos, endPos) || hasUnclosedQuotes(startPos, endPos)) return false;
	return true;
}

bool CHTLUnifiedScanner::hasUnclosedBrackets(size_t startPos, size_t endPos) {
	int count = 0; for (size_t i = startPos; i < endPos; ++i) { if (source_[i] == '[') ++count; if (source_[i] == ']') --count; }
	return count != 0;
}

bool CHTLUnifiedScanner::hasUnclosedBraces(size_t startPos, size_t endPos) {
	int count = 0; for (size_t i = startPos; i < endPos; ++i) { if (source_[i] == '{') ++count; if (source_[i] == '}') --count; }
	return count != 0;
}

bool CHTLUnifiedScanner::hasUnclosedParens(size_t startPos, size_t endPos) {
	int count = 0; for (size_t i = startPos; i < endPos; ++i) { if (source_[i] == '(') ++count; if (source_[i] == ')') --count; }
	return count != 0;
}

bool CHTLUnifiedScanner::hasUnclosedQuotes(size_t startPos, size_t endPos) {
	bool inSingle = false, inDouble = false;
	for (size_t i = startPos; i < endPos; ++i) {
		char ch = source_[i];
		if (ch == '\'' && !inDouble) inSingle = !inSingle;
		else if (ch == '"' && !inSingle) inDouble = !inDouble;
	}
	return inSingle || inDouble;
}

char CHTLUnifiedScanner::peek(size_t offset) const {
	size_t pos = currentPos_ + offset;
	if (pos >= source_.length()) return '\0';
	return source_[pos];
}

void CHTLUnifiedScanner::advance(size_t count) { currentPos_ += count; currentColumn_ += count; }

void CHTLUnifiedScanner::advanceLine() { ++currentPos_; ++currentLine_; currentColumn_ = 1; }

bool CHTLUnifiedScanner::isEOF() const { return currentPos_ >= source_.length(); }

void CHTLUnifiedScanner::setState(ScannerState state) { if (currentState_ != state) { logStateTransition(currentState_, state); currentState_ = state; } }

void CHTLUnifiedScanner::logStateTransition(ScannerState, ScannerState) { /* optional logging */ }

// 基础CJMOD切分：按词法将区间切分为token序列（数字/标识符/操作符保留）
static bool isIdentChar(char c) {
	return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '.';
}

CHTLUnifiedScanner::CJMODScanResult CHTLUnifiedScanner::scanCJMODByTwoPointers(size_t start, size_t end) {
	CJMODScanResult result;
	if (start >= source_.length() || end > source_.length() || start >= end) {
		return result;
	}
	
	std::string fragment = source_.substr(start, end - start);
	
	// 使用CJMOD API进行双指针扫描
	try {
		// 分析模式：$ ** $ (双指针操作)
		auto pattern = CJMODAPI::Syntax::analyze("$ ** $");
		
		// 扫描片段
		auto scanned = CJMODAPI::CJMODScannerAPI::scan(pattern, "**", fragment);
		
		// 提取结果
		for (size_t i = 0; i < scanned.size(); ++i) {
			result.tokens.push_back(scanned[i].value);
		}
		
		// 记录扫描位置
		result.startPos = start;
		result.endPos = end;
		result.success = true;
		
	} catch (const std::exception& e) {
		result.success = false;
		result.error = "CJMOD扫描异常: " + std::string(e.what());
	}
	
	return result;
}

std::string CHTLUnifiedScanner::preEmptiveTruncateCJMOD(const std::string& fragment) {
	// 前置截取：根据CJMOD语法规则智能截取
	try {
		// 检测CJMOD关键字
		std::vector<std::string> cjmodKeywords = {"vir", "listen", "delegate", "animate", "iNeverAway", "printMylove"};
		
		for (const auto& keyword : cjmodKeywords) {
			size_t pos = fragment.find(keyword);
			if (pos != std::string::npos) {
				// 找到关键字，截取到下一个分号或大括号
				size_t endPos = fragment.find(';', pos);
				if (endPos == std::string::npos) {
					endPos = fragment.find('{', pos);
				}
				if (endPos != std::string::npos) {
					return fragment.substr(pos, endPos - pos + 1);
				}
			}
		}
		
		// 如果没有找到关键字，尝试截取表达式
		if (fragment.find("**") != std::string::npos || 
			fragment.find("->") != std::string::npos ||
			fragment.find("{{") != std::string::npos) {
			
			// 截取到表达式结束
			size_t endPos = fragment.find(';');
			if (endPos != std::string::npos) {
				return fragment.substr(0, endPos + 1);
			}
		}
		
	} catch (const std::exception& e) {
		// 截取失败，返回原片段
	}
	
	return fragment;
}

CHTLUnifiedScanner::CJMODScanResult CHTLUnifiedScanner::preCaptureForCJMOD(size_t startPos, size_t endPos) {
	// 前置截取：结合双指针扫描和前置截取
	CJMODScanResult result;
	
	// 先进行前置截取
	std::string fragment = source_.substr(startPos, endPos - startPos);
	std::string truncated = preEmptiveTruncateCJMOD(fragment);
	
	// 然后对截取后的片段进行双指针扫描
	if (truncated != fragment) {
		// 如果发生了截取，重新计算位置
		size_t newStart = startPos + fragment.find(truncated);
		size_t newEnd = newStart + truncated.length();
		return scanCJMODByTwoPointers(newStart, newEnd);
	} else {
		// 如果没有截取，直接扫描
		return scanCJMODByTwoPointers(startPos, endPos);
	}
}

} // namespace Scanner