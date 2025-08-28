#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "../Scanner/CHTLUnifiedScanner.h"

namespace CHTLCompiler {

enum class CompilerType {
	CHTL,
	CHTL_JS,
	CSS,
	JAVASCRIPT
};

struct CompilationResult {
	std::string output;
	std::vector<std::string> errors;
	bool success;
	
	CompilationResult() : success(false) {}
};

class CompilerDispatcher {
public:
	CompilerDispatcher();
	~CompilerDispatcher() = default;
	
	// 设置源代码
	void setSource(const std::string& source);
	
	// 编译整个项目
	CompilationResult compile();
	
	// 获取编译错误
	const std::vector<std::string>& getErrors() const { return errors_; }
	
	// 检查是否编译成功
	bool isSuccess() const { return errors_.empty(); }

private:
	std::string source_;
	std::shared_ptr<Scanner::CHTLUnifiedScanner> scanner_;
	std::vector<std::string> errors_;
	
	// 编译方法
	CompilationResult compileCHTL(const std::string& fragment);
	CompilationResult compileCHTLJS(const std::string& fragment);
	CompilationResult compileCSS(const std::string& fragment);
	CompilationResult compileJavaScript(const std::string& fragment);
	
	// 辅助方法
	void addError(const std::string& error);
	std::string mergeResults(const std::vector<CompilationResult>& results);
};

} // namespace CHTLCompiler