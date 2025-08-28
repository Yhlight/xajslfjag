#include "CHTLParser.h"
#include <sstream>

namespace CHTL {

CHTLParser::CHTLParser() {
	lexer_ = std::make_shared<CHTLLexer>();
}

void CHTLParser::setSource(const std::string& source) {
	source_ = source;
	lexer_->setSource(source);
}

std::shared_ptr<BaseNode> CHTLParser::parse() {
	// 重置状态
	errors_.clear();
	
	try {
		// 解析根节点
		auto root = parseElement();
		if (root) {
			return root;
		}
		
		// 如果没有元素，尝试解析模板
		auto templateNode = parseTemplate();
		if (templateNode) {
			return templateNode;
		}
		
		// 尝试解析自定义元素
		auto customNode = parseCustom();
		if (customNode) {
			return customNode;
		}
		
		// 尝试解析Origin嵌入
		auto originNode = parseOrigin();
		if (originNode) {
			return originNode;
		}
		
		// 尝试解析Import
		auto importNode = parseImport();
		if (importNode) {
			return importNode;
		}
		
		// 尝试解析Namespace
		auto namespaceNode = parseNamespace();
		if (namespaceNode) {
			return namespaceNode;
		}
		
		// 尝试解析Configuration
		auto configNode = parseConfiguration();
		if (configNode) {
			return configNode;
		}
		
		addError("无法解析任何有效的CHTL语法");
		return nullptr;
		
	} catch (const std::exception& e) {
		addError("解析异常: " + std::string(e.what()));
		return nullptr;
	}
}

std::shared_ptr<BaseNode> CHTLParser::parseElement() {
	// 简化实现：检测元素语法
	if (source_.find("div") != std::string::npos || 
		source_.find("span") != std::string::npos ||
		source_.find("button") != std::string::npos) {
		
		// 创建元素节点
		auto elementNode = std::make_shared<ElementNode>();
		elementNode->type = NodeType::ELEMENT;
		elementNode->tagName = "div"; // 简化
		
		// 检查是否有style块
		if (source_.find("style") != std::string::npos) {
			auto styleNode = parseStyleBlock();
			if (styleNode) {
				elementNode->children.push_back(styleNode);
			}
		}
		
		// 检查是否有script块
		if (source_.find("script") != std::string::npos) {
			auto scriptNode = parseScriptBlock();
			if (scriptNode) {
				elementNode->children.push_back(scriptNode);
			}
		}
		
		return elementNode;
	}
	
	return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseTemplate() {
	// 检测模板语法
	if (source_.find("[Template]") != std::string::npos) {
		auto templateNode = std::make_shared<TemplateNode>();
		templateNode->type = NodeType::TEMPLATE;
		
		// 解析模板类型
		if (source_.find("@Style") != std::string::npos) {
			templateNode->templateType = "Style";
		} else if (source_.find("@Element") != std::string::npos) {
			templateNode->templateType = "Element";
		} else if (source_.find("@Var") != std::string::npos) {
			templateNode->templateType = "Var";
		}
		
		return templateNode;
	}
	
	return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseCustom() {
	// 检测自定义元素语法
	if (source_.find("[Custom]") != std::string::npos) {
		auto customNode = std::make_shared<CustomNode>();
		customNode->type = NodeType::CUSTOM;
		
		// 解析自定义类型
		if (source_.find("delete") != std::string::npos) {
			customNode->operation = "delete";
		} else if (source_.find("insert") != std::string::npos) {
			customNode->operation = "insert";
		} else if (source_.find("use") != std::string::npos) {
			customNode->operation = "use";
		}
		
		return customNode;
	}
	
	return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseOrigin() {
	// 检测Origin嵌入语法
	if (source_.find("[Origin]") != std::string::npos) {
		auto originNode = std::make_shared<OriginNode>();
		originNode->type = NodeType::ORIGIN;
		
		// 解析Origin类型
		if (source_.find("@Html") != std::string::npos) {
			originNode->originType = "Html";
		} else if (source_.find("@Style") != std::string::npos) {
			originNode->originType = "Style";
		} else if (source_.find("@JavaScript") != std::string::npos) {
			originNode->originType = "JavaScript";
		}
		
		return originNode;
	}
	
	return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseImport() {
	// 检测Import语法
	if (source_.find("[Import]") != std::string::npos) {
		auto importNode = std::make_shared<ImportNode>();
		importNode->type = NodeType::IMPORT;
		
		// 解析Import类型
		if (source_.find("@Chtl") != std::string::npos) {
			importNode->importType = "Chtl";
		} else if (source_.find("@CJmod") != std::string::npos) {
			importNode->importType = "CJmod";
		} else if (source_.find("@Html") != std::string::npos) {
			importNode->importType = "Html";
		} else if (source_.find("@Style") != std::string::npos) {
			importNode->importType = "Style";
		} else if (source_.find("@JavaScript") != std::string::npos) {
			importNode->importType = "JavaScript";
		} else if (source_.find("@Config") != std::string::npos) {
			importNode->importType = "Config";
		}
		
		return importNode;
	}
	
	return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseNamespace() {
	// 检测Namespace语法
	if (source_.find("[Namespace]") != std::string::npos) {
		auto namespaceNode = std::make_shared<NamespaceNode>();
		namespaceNode->type = NodeType::NAMESPACE;
		return namespaceNode;
	}
	
	return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseConfiguration() {
	// 检测Configuration语法
	if (source_.find("[Configuration]") != std::string::npos) {
		auto configNode = std::make_shared<ConfigNode>();
		configNode->type = NodeType::CONFIG;
		return configNode;
	}
	
	return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseStyleBlock() {
	// 检测style块
	if (source_.find("style") != std::string::npos) {
		auto styleNode = std::make_shared<StyleNode>();
		styleNode->type = NodeType::STYLE;
		return styleNode;
	}
	
	return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseScriptBlock() {
	// 检测script块
	if (source_.find("script") != std::string::npos) {
		auto scriptNode = std::make_shared<ScriptNode>();
		scriptNode->type = NodeType::SCRIPT;
		return scriptNode;
	}
	
	return nullptr;
}

void CHTLParser::addError(const std::string& error) {
	errors_.push_back(error);
}

bool CHTLParser::expectToken(CHTLTokenType type) {
	// 简化实现：总是返回true
	(void)type;
	return true;
}

void CHTLParser::skipToToken(CHTLTokenType type) {
	// 简化实现：无操作
	(void)type;
}

} // namespace CHTL