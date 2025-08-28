#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../CHTLNode/BaseNode.h"
#include "../CHTLLexer/CHTLLexer.h"

namespace CHTL {

class CHTLParser {
public:
	CHTLParser();
	~CHTLParser() = default;
	
	// 设置源代码
	void setSource(const std::string& source);
	
	// 解析生成AST
	std::shared_ptr<BaseNode> parse();
	
	// 获取解析错误
	const std::vector<std::string>& getErrors() const { return errors_; }
	
	// 检查是否解析成功
	bool isSuccess() const { return errors_.empty(); }

private:
	std::string source_;
	std::shared_ptr<CHTLLexer> lexer_;
	std::vector<std::string> errors_;
	
	// 解析方法
	std::shared_ptr<BaseNode> parseElement();
	std::shared_ptr<BaseNode> parseTemplate();
	std::shared_ptr<BaseNode> parseCustom();
	std::shared_ptr<BaseNode> parseOrigin();
	std::shared_ptr<BaseNode> parseImport();
	std::shared_ptr<BaseNode> parseNamespace();
	std::shared_ptr<BaseNode> parseConfiguration();
	std::shared_ptr<BaseNode> parseStyleBlock();
	std::shared_ptr<BaseNode> parseScriptBlock();
	
	// 辅助方法
	void addError(const std::string& error);
	bool expectToken(CHTLTokenType type);
	void skipToToken(CHTLTokenType type);
};

} // namespace CHTL