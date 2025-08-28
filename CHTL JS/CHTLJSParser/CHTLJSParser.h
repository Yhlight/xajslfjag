#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../CHTLJSNode/AST.h"
#include "../CHTLJSLexer/CHTLJSLexer.h"

namespace CHTLJS {

class CHTLJSParser {
public:
	CHTLJSParser();
	~CHTLJSParser() = default;
	
	// 设置源代码
	void setSource(const std::string& source);
	
	// 解析生成AST
	std::shared_ptr<CHTLJSBaseNode> parse();
	
	// 获取解析错误
	const std::vector<std::string>& getErrors() const { return errors_; }
	
	// 检查是否解析成功
	bool isSuccess() const { return errors_.empty(); }

private:
	std::string source_;
	std::shared_ptr<CHTLJSLexer> lexer_;
	std::vector<std::string> errors_;
	
	// 解析方法
	std::shared_ptr<CHTLJSBaseNode> parseVirtualObject();
	std::shared_ptr<CHTLJSBaseNode> parseEnhancedSelector();
	std::shared_ptr<CHTLJSBaseNode> parseChainOperation();
	std::shared_ptr<CHTLJSBaseNode> parseEventBinding();
	std::shared_ptr<CHTLJSBaseNode> parseListenBlock();
	std::shared_ptr<CHTLJSBaseNode> parseDelegateBlock();
	std::shared_ptr<CHTLJSBaseNode> parseAnimateBlock();
	std::shared_ptr<CHTLJSBaseNode> parseINeverAway();
	std::shared_ptr<CHTLJSBaseNode> parsePrintMyLove();
	
	// 辅助方法
	void addError(const std::string& error);
	bool expectToken(CHTLJSTokenType type);
	void skipToToken(CHTLJSTokenType type);
};

} // namespace CHTLJS