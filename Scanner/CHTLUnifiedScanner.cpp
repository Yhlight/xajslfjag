#include "CHTLUnifiedScanner.h"
#include <algorithm>
#include <iostream>

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

// CJMOD占位实现
CHTLUnifiedScanner::CJMODScanResult CHTLUnifiedScanner::scanCJMODByTwoPointers(size_t, size_t) {
	return CJMODScanResult{}; // 后续实现双指针扫描
}

CHTLUnifiedScanner::CJMODScanResult CHTLUnifiedScanner::preCaptureForCJMOD(size_t, size_t) {
	return CJMODScanResult{}; // 后续实现前置截取
}

} // namespace Scanner