#pragma once

#include <string>

namespace CHTL {

enum class CHTLTokenType {
	// 基础类型
	IDENTIFIER,
	NUMBER,
	STRING,
	
	// 关键字
	TEMPLATE,
	CUSTOM,
	ORIGIN,
	IMPORT,
	NAMESPACE,
	CONFIGURATION,
	STYLE,
	SCRIPT,
	TEXT,
	
	// 分隔符
	LEFT_BRACE,  // {
	RIGHT_BRACE, // }
	LEFT_BRACKET, // [
	RIGHT_BRACKET, // ]
	COLON,       // :
	SEMICOLON,   // ;
	COMMA,       // ,
	
	// 特殊
	EOF_TOKEN,
	ERROR
};

class CHTLLexer {
public:
	CHTLLexer() = default;
	~CHTLLexer() = default;
	
	// 设置源代码
	void setSource(const std::string& source);
	
	// 获取源代码
	const std::string& getSource() const { return source_; }

private:
	std::string source_;
};

} // namespace CHTL