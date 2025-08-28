#pragma once

#include <string>
#include <memory>
#include "../CHTLNode/BaseNode.h"
#include "../CHTLContext/CHTLContext.h"

namespace CHTL {

class CHTLGenerator {
public:
	CHTLGenerator();
	~CHTLGenerator() = default;
	
	// 设置上下文
	void setContext(std::shared_ptr<CHTLContext> context);
	
	// 生成代码
	std::string generate(std::shared_ptr<BaseNode> ast);
	
	// 获取生成错误
	const std::vector<std::string>& getErrors() const { return errors_; }
	
	// 检查是否生成成功
	bool isSuccess() const { return errors_.empty(); }

private:
	std::shared_ptr<CHTLContext> context_;
	std::vector<std::string> errors_;
	
	// 生成方法
	std::string generateElement(std::shared_ptr<BaseNode> node);
	std::string generateTemplate(std::shared_ptr<BaseNode> node);
	std::string generateCustom(std::shared_ptr<BaseNode> node);
	std::string generateOrigin(std::shared_ptr<BaseNode> node);
	std::string generateImport(std::shared_ptr<BaseNode> node);
	std::string generateNamespace(std::shared_ptr<BaseNode> node);
	std::string generateConfiguration(std::shared_ptr<BaseNode> node);
	std::string generateStyleBlock(std::shared_ptr<BaseNode> node);
	std::string generateScriptBlock(std::shared_ptr<BaseNode> node);
	
	// 辅助方法
	void addError(const std::string& error);
	std::string resolveTemplate(const std::string& templateName);
	std::string processInheritance(const std::string& baseContent, const std::string& derivedContent);
};

} // namespace CHTL