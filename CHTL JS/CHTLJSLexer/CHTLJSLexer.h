#pragma once

#include <string>

namespace CHTLJS {

enum class CHTLJSTokenType {
	// 基础类型
	IDENTIFIER,
	NUMBER,
	STRING,
	
	// 关键字
	VIRTUAL_OBJECT,
	ENHANCED_SELECTOR,
	CHAIN_OPERATION,
	EVENT_BINDING,
	LISTEN,
	DELEGATE,
	ANIMATE,
	I_NEVER_AWAY,
	PRINT_MY_LOVE,
	
	// 分隔符
	LEFT_BRACE,  // {
	RIGHT_BRACE, // }
	LEFT_BRACKET, // [
	RIGHT_BRACKET, // ]
	COLON,       // :
	SEMICOLON,   // ;
	COMMA,       // ,
	ARROW,       // ->
	DOT,         // .
	
	// 特殊
	EOF_TOKEN,
	ERROR
};

class CHTLJSLexer {
public:
	CHTLJSLexer() = default;
	~CHTLJSLexer() = default;
	
	// 设置源代码
	void setSource(const std::string& source);
	
	// 获取源代码
	const std::string& getSource() const { return source_; }

private:
	std::string source_;
};

} // namespace CHTLJS