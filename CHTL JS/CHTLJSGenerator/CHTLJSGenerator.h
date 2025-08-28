#pragma once

#include <string>
#include <memory>
#include "../CHTLJSNode/AST.h"
#include "../CHTLJSContext/CHTLJSContext.h"

namespace CHTLJS {

class CHTLJSGenerator {
public:
	CHTLJSGenerator();
	~CHTLJSGenerator() = default;
	
	// 设置上下文
	void setContext(std::shared_ptr<CHTLJSContext> context);
	
	// 生成代码
	std::string generate(std::shared_ptr<CHTLJSBaseNode> ast);
	
	// 获取生成错误
	const std::vector<std::string>& getErrors() const { return errors_; }
	
	// 检查是否生成成功
	bool isSuccess() const { return errors_.empty(); }

private:
	std::shared_ptr<CHTLJSContext> context_;
	std::vector<std::string> errors_;
	
	// 生成方法
	std::string generateVirtualObject(std::shared_ptr<CHTLJSBaseNode> node);
	std::string generateEnhancedSelector(std::shared_ptr<CHTLJSBaseNode> node);
	std::string generateChainOperation(std::shared_ptr<CHTLJSBaseNode> node);
	std::string generateEventBinding(std::shared_ptr<CHTLJSBaseNode> node);
	std::string generateListenBlock(std::shared_ptr<CHTLJSBaseNode> node);
	std::string generateDelegateBlock(std::shared_ptr<CHTLJSBaseNode> node);
	std::string generateAnimateBlock(std::shared_ptr<CHTLJSBaseNode> node);
	std::string generateINeverAway(std::shared_ptr<CHTLJSBaseNode> node);
	std::string generatePrintMyLove(std::shared_ptr<CHTLJSBaseNode> node);
	
	// 辅助方法
	void addError(const std::string& error);
	std::string convertEnhancedSelector(const std::string& selector);
	std::string expandChainOperation(const std::string& operation);
};

} // namespace CHTLJS