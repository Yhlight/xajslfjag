#include "CompilerDispatcher.h"
#include <algorithm>
#include <sstream>

namespace CHTLCompiler {

CompilerDispatcher::CompilerDispatcher() {
	scanner_ = std::make_shared<Scanner::CHTLUnifiedScanner>();
}

void CompilerDispatcher::setSource(const std::string& source) {
	source_ = source;
	scanner_->setSource(source);
}

CompilationResult CompilerDispatcher::compile() {
	CompilationResult result;
	
	try {
		// 使用统一扫描器分割代码
		auto fragments = scanner_->scan();
		
		if (fragments.empty()) {
			addError("没有扫描到任何代码片段");
			return result;
		}
		
		// 分别编译每个片段
		std::vector<CompilationResult> fragmentResults;
		
		for (const auto& fragment : fragments) {
			CompilationResult fragmentResult;
			
			switch (fragment.type) {
				case Scanner::CodeFragmentType::CHTL:
					fragmentResult = compileCHTL(fragment.content);
					break;
				case Scanner::CodeFragmentType::CHTL_JS:
					fragmentResult = compileCHTLJS(fragment.content);
					break;
				case Scanner::CodeFragmentType::CSS:
					fragmentResult = compileCSS(fragment.content);
					break;
				case Scanner::CodeFragmentType::JAVASCRIPT:
					fragmentResult = compileJavaScript(fragment.content);
					break;
				default:
					fragmentResult.errors.push_back("未知的代码片段类型");
					fragmentResult.success = false;
					break;
			}
			
			fragmentResults.push_back(fragmentResult);
		}
		
		// 合并所有编译结果
		result.output = mergeResults(fragmentResults);
		result.success = true;
		
		// 收集所有错误
		for (const auto& fragmentResult : fragmentResults) {
			result.errors.insert(result.errors.end(), 
							   fragmentResult.errors.begin(), 
							   fragmentResult.errors.end());
		}
		
	} catch (const std::exception& e) {
		addError("编译过程中发生异常: " + std::string(e.what()));
	} catch (...) {
		addError("编译过程中发生未知异常");
	}
	
	return result;
}

CompilationResult CompilerDispatcher::compileCHTL(const std::string& fragment) {
	CompilationResult result;
	
	try {
		// TODO: 集成CHTL编译器
		// 暂时返回原代码
		result.output = fragment;
		result.success = true;
		
	} catch (const std::exception& e) {
		result.errors.push_back("CHTL编译异常: " + std::string(e.what()));
		result.success = false;
	}
	
	return result;
}

CompilationResult CompilerDispatcher::compileCHTLJS(const std::string& fragment) {
	CompilationResult result;
	
	try {
		// TODO: 集成CHTL JS编译器
		// 暂时返回原代码
		result.output = fragment;
		result.success = true;
		
	} catch (const std::exception& e) {
		result.errors.push_back("CHTL JS编译异常: " + std::string(e.what()));
		result.success = false;
	}
	
	return result;
}

CompilationResult CompilerDispatcher::compileCSS(const std::string& fragment) {
	CompilationResult result;
	
	try {
		// TODO: 集成ANTLR4 CSS编译器
		// 暂时返回原代码
		result.output = fragment;
		result.success = true;
		
	} catch (const std::exception& e) {
		result.errors.push_back("CSS编译异常: " + std::string(e.what()));
		result.success = false;
	}
	
	return result;
}

CompilationResult CompilerDispatcher::compileJavaScript(const std::string& fragment) {
	CompilationResult result;
	
	try {
		// TODO: 集成ANTLR4 JavaScript编译器
		// 暂时返回原代码
		result.output = fragment;
		result.success = true;
		
	} catch (const std::exception& e) {
		result.errors.push_back("JavaScript编译异常: " + std::string(e.what()));
		result.success = false;
	}
	
	return result;
}

void CompilerDispatcher::addError(const std::string& error) {
	errors_.push_back(error);
}

std::string CompilerDispatcher::mergeResults(const std::vector<CompilationResult>& results) {
	std::stringstream merged;
	
	for (const auto& result : results) {
		if (result.success && !result.output.empty()) {
			merged << result.output << "\n";
		}
	}
	
	return merged.str();
}

} // namespace CHTLCompiler