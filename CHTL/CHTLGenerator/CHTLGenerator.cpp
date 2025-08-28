#include "CHTLGenerator.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLGenerator::CHTLGenerator() {
	// 初始化生成器
}

void CHTLGenerator::setContext(std::shared_ptr<CHTLContext> context) {
	context_ = context;
}

std::string CHTLGenerator::generate(std::shared_ptr<BaseNode> ast) {
	if (!ast) {
		addError("AST节点为空");
		return "";
	}
	
	try {
		// 根据节点类型生成代码
		switch (ast->type) {
			case NodeType::ELEMENT:
				return generateElement(ast);
			case NodeType::TEXT:
				return generateText(ast);
			case NodeType::TEMPLATE:
				return generateTemplate(ast);
			case NodeType::CUSTOM:
				return generateCustom(ast);
			case NodeType::ORIGIN:
				return generateOrigin(ast);
			case NodeType::IMPORT:
				return generateImport(ast);
			case NodeType::NAMESPACE:
				return generateNamespace(ast);
			case NodeType::CONFIG:
				return generateConfiguration(ast);
			case NodeType::STYLE:
				return generateStyleBlock(ast);
			case NodeType::SCRIPT:
				return generateScriptBlock(ast);
			default:
				addError("未知的节点类型: " + std::to_string(static_cast<int>(ast->type)));
				return "";
		}
		
	} catch (const std::exception& e) {
		addError("生成异常: " + std::string(e.what()));
		return "";
	}
}

std::string CHTLGenerator::generateElement(std::shared_ptr<BaseNode> node) {
	if (auto elementNode = std::dynamic_pointer_cast<ElementNode>(node)) {
		std::stringstream output;
		
		output << elementNode->tagName << "\n{\n";
		
		// 生成属性
		for (const auto& attr : elementNode->attributes) {
			output << "    " << attr.first << ": " << attr.second << ";\n";
		}
		
		// 生成子元素
		for (const auto& child : elementNode->children) {
			output << "    " << generate(child) << "\n";
		}
		
		output << "}\n";
		
		return output.str();
	}
	
	addError("无法转换为ElementNode");
	return "";
}

std::string CHTLGenerator::generateText(std::shared_ptr<BaseNode> node) {
	if (auto textNode = std::dynamic_pointer_cast<TextNode>(node)) {
		return textNode->textContent;
	}
	
	addError("无法转换为TextNode");
	return "";
}

std::string CHTLGenerator::generateTemplate(std::shared_ptr<BaseNode> node) {
	if (auto templateNode = std::dynamic_pointer_cast<TemplateNode>(node)) {
		std::stringstream output;
		
		output << "[Template] @" << templateNode->templateType << " " << templateNode->templateName;
		
		if (!templateNode->inheritType.empty()) {
			output << " " << templateNode->inheritType << " " << templateNode->inheritName;
		}
		
		output << "\n{\n";
		
		// 生成子元素
		for (const auto& child : templateNode->children) {
			output << "    " << generate(child) << "\n";
		}
		
		output << "}\n";
		
		return output.str();
	}
	
	addError("无法转换为TemplateNode");
	return "";
}

std::string CHTLGenerator::generateCustom(std::shared_ptr<BaseNode> node) {
	if (auto customNode = std::dynamic_pointer_cast<CustomNode>(node)) {
		std::stringstream output;
		
		output << "[Custom] @" << customNode->customType << " " << customNode->customName << "\n{\n";
		
		// 生成自定义内容
		if (!customNode->operation.empty()) {
			output << "    " << customNode->operation;
			if (!customNode->operationTarget.empty()) {
				output << " " << customNode->operationTarget;
			}
			output << ";\n";
		}
		
		// 生成子元素
		for (const auto& child : customNode->children) {
			output << "    " << generate(child) << "\n";
		}
		
		output << "}\n";
		
		return output.str();
	}
	
	addError("无法转换为CustomNode");
	return "";
}

std::string CHTLGenerator::generateOrigin(std::shared_ptr<BaseNode> node) {
	if (auto originNode = std::dynamic_pointer_cast<OriginNode>(node)) {
		std::stringstream output;
		
		output << "[Origin] @" << originNode->originName << "\n";
		
		// 生成子元素
		for (const auto& child : originNode->children) {
			output << "    " << generate(child) << "\n";
		}
		
		return output.str();
	}
	
	addError("无法转换为OriginNode");
	return "";
}

std::string CHTLGenerator::generateImport(std::shared_ptr<BaseNode> node) {
	if (auto importNode = std::dynamic_pointer_cast<ImportNode>(node)) {
		std::stringstream output;
		
		output << "[Import] @" << importNode->importType << " from " << importNode->from;
		
		if (importNode->hasAs) {
			output << " as " << importNode->asName;
		}
		
		output << "\n";
		
		return output.str();
	}
	
	addError("无法转换为ImportNode");
	return "";
}

std::string CHTLGenerator::generateNamespace(std::shared_ptr<BaseNode> node) {
	if (auto namespaceNode = std::dynamic_pointer_cast<NamespaceNode>(node)) {
		std::stringstream output;
		
		output << "[Namespace] " << namespaceNode->namespaceName << "\n{\n";
		
		// 生成子元素
		for (const auto& child : namespaceNode->children) {
			output << "    " << generate(child) << "\n";
		}
		
		output << "}\n";
		
		return output.str();
	}
	
	addError("无法转换为NamespaceNode");
	return "";
}

std::string CHTLGenerator::generateConfiguration(std::shared_ptr<BaseNode> node) {
	if (auto configNode = std::dynamic_pointer_cast<ConfigNode>(node)) {
		std::stringstream output;
		
		output << "[Configuration] " << configNode->configName << "\n{\n";
		
		// 生成子元素
		for (const auto& child : configNode->children) {
			output << "    " << generate(child) << "\n";
		}
		
		output << "}\n";
		
		return output.str();
	}
	
	addError("无法转换为ConfigNode");
	return "";
}

std::string CHTLGenerator::generateStyleBlock(std::shared_ptr<BaseNode> node) {
	if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(node)) {
		std::stringstream output;
		
		output << "style\n{\n";
		
		// 生成子元素
		for (const auto& child : styleNode->children) {
			output << "    " << generate(child) << "\n";
		}
		
		output << "}\n";
		
		return output.str();
	}
	
	addError("无法转换为StyleNode");
	return "";
}

std::string CHTLGenerator::generateScriptBlock(std::shared_ptr<BaseNode> node) {
	if (auto scriptNode = std::dynamic_pointer_cast<ScriptNode>(node)) {
		std::stringstream output;
		
		output << "script\n{\n";
		
		// 生成子元素
		for (const auto& child : scriptNode->children) {
			output << "    " << generate(child) << "\n";
		}
		
		output << "}\n";
		
		return output.str();
	}
	
	addError("无法转换为ScriptNode");
	return "";
}

void CHTLGenerator::addError(const std::string& error) {
	errors_.push_back(error);
}

std::string CHTLGenerator::resolveTemplate(const std::string& templateName) {
	// 简化实现：返回模板名称
	return templateName;
}

std::string CHTLGenerator::processInheritance(const std::string& baseContent, const std::string& derivedContent) {
	// 简化实现：合并基础内容和派生内容
	return baseContent + "\n" + derivedContent;
}

} // namespace CHTL