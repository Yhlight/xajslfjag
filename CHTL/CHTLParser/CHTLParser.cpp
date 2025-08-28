#include "CHTLParser.h"
#include <sstream>
#include <regex>

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
		// 检查是否包含多个语法结构
		if (source_.find("[Template]") != std::string::npos || 
			source_.find("[Custom]") != std::string::npos ||
			source_.find("[Origin]") != std::string::npos ||
			source_.find("[Import]") != std::string::npos ||
			source_.find("[Namespace]") != std::string::npos ||
			source_.find("[Configuration]") != std::string::npos) {
			
			// 如果包含特殊语法，优先解析模板
			if (source_.find("[Template]") != std::string::npos) {
				auto templateNode = parseTemplate();
				if (templateNode) {
					return templateNode;
				}
			}
			
			// 尝试解析自定义元素
			if (source_.find("[Custom]") != std::string::npos) {
				auto customNode = parseCustom();
				if (customNode) {
					return customNode;
				}
			}
			
			// 尝试解析Origin嵌入
			if (source_.find("[Origin]") != std::string::npos) {
				auto originNode = parseOrigin();
				if (originNode) {
					return originNode;
				}
			}
			
			// 尝试解析Import
			if (source_.find("[Import]") != std::string::npos) {
				auto importNode = parseImport();
				if (importNode) {
					return importNode;
				}
			}
			
			// 尝试解析Namespace
			if (source_.find("[Namespace]") != std::string::npos) {
				auto namespaceNode = parseNamespace();
				if (namespaceNode) {
					return namespaceNode;
				}
			}
			
			// 尝试解析Configuration
			if (source_.find("[Configuration]") != std::string::npos) {
				auto configNode = parseConfiguration();
				if (configNode) {
					return configNode;
				}
			}
		}
		
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
		
		// 尝试解析Text节点
		auto textNode = parseText();
		if (textNode) {
			return textNode;
		}
		
		addError("无法解析任何有效的CHTL语法");
		return nullptr;
		
	} catch (const std::exception& e) {
		addError("解析异常: " + std::string(e.what()));
		return nullptr;
	}
}

std::shared_ptr<BaseNode> CHTLParser::parseElement() {
	// 检测HTML元素语法
	std::vector<std::string> htmlElements = {"html", "head", "body", "div", "span", "button", "input", "form", "table", "tr", "td", "th", "ul", "ol", "li", "a", "img", "p", "h1", "h2", "h3", "h4", "h5", "h6"};
	
	for (const auto& element : htmlElements) {
		if (source_.find(element + " {") != std::string::npos || 
			source_.find(element + "\n{") != std::string::npos) {
			
			// 创建元素节点
			auto elementNode = std::make_shared<ElementNode>();
			elementNode->type = NodeType::ELEMENT;
			elementNode->tagName = element;
			
			// 解析属性
			parseElementAttributes(elementNode);
			
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
			
			// 检查是否有text块
			if (source_.find("text") != std::string::npos) {
				auto textNode = parseText();
				if (textNode) {
					elementNode->children.push_back(textNode);
				}
			}
			
			return elementNode;
		}
	}
	
	return nullptr;
}

void CHTLParser::parseElementAttributes(std::shared_ptr<ElementNode> elementNode) {
	// 解析元素属性，如 id: "box"; class: "container";
	std::regex attrRegex(R"((\w+)\s*:\s*["']?([^"';]+)["']?\s*;)");
	std::string source = source_;
	
	std::sregex_iterator iter(source.begin(), source.end(), attrRegex);
	std::sregex_iterator end;
	
	for (; iter != end; ++iter) {
		std::string attrName = (*iter)[1];
		std::string attrValue = (*iter)[2];
		
		// 去除引号
		if (attrValue.front() == '"' || attrValue.front() == '\'') {
			attrValue = attrValue.substr(1, attrValue.length() - 2);
		}
		
		elementNode->attributes[attrName] = attrValue;
	}
}

std::shared_ptr<BaseNode> CHTLParser::parseTemplate() {
	// 检测模板语法 [Template] @Style/@Element/@Var
	if (source_.find("[Template]") != std::string::npos) {
		auto templateNode = std::make_shared<TemplateNode>();
		templateNode->type = NodeType::TEMPLATE;
		
		// 解析模板类型和名称
		if (source_.find("@Style") != std::string::npos) {
			templateNode->templateType = "Style";
			// 提取样式组名称
			std::regex nameRegex(R"(@Style\s+(\w+))");
			std::smatch match;
			if (std::regex_search(source_, match, nameRegex)) {
				templateNode->templateName = match[1];
			}
		} else if (source_.find("@Element") != std::string::npos) {
			templateNode->templateType = "Element";
			// 提取元素名称
			std::regex nameRegex(R"(@Element\s+(\w+))");
			std::smatch match;
			if (std::regex_search(source_, match, nameRegex)) {
				templateNode->templateName = match[1];
			}
		} else if (source_.find("@Var") != std::string::npos) {
			templateNode->templateType = "Var";
			// 提取变量组名称
			std::regex nameRegex(R"(@Var\s+(\w+))");
			std::smatch match;
			if (std::regex_search(source_, match, nameRegex)) {
				templateNode->templateName = match[1];
			}
		}
		
		// 解析继承
		if (source_.find("inherit") != std::string::npos) {
			std::regex inheritRegex(R"(inherit\s+@(\w+)\s+(\w+))");
			std::smatch match;
			if (std::regex_search(source_, match, inheritRegex)) {
				templateNode->inheritType = match[1];
				templateNode->inheritName = match[2];
			}
		}
		
		// 解析模板内容（大括号内的部分）
		size_t startBrace = source_.find('{');
		size_t endBrace = source_.rfind('}');
		
		if (startBrace != std::string::npos && endBrace != std::string::npos && endBrace > startBrace) {
			std::string content = source_.substr(startBrace + 1, endBrace - startBrace - 1);
			
			// 解析内容中的属性
			std::regex propRegex(R"((\w+)\s*:\s*["']?([^"';]+)["']?\s*;)");
			std::sregex_iterator iter(content.begin(), content.end(), propRegex);
			std::sregex_iterator end;
			
			for (; iter != end; ++iter) {
				std::string propName = (*iter)[1];
				std::string propValue = (*iter)[2];
				
				// 去除引号
				if (propValue.front() == '"' || propValue.front() == '\'') {
					propValue = propValue.substr(1, propValue.length() - 2);
				}
				
				// 创建属性节点或直接存储属性
				// 这里简化处理，直接创建文本节点
				auto textNode = std::make_shared<TextNode>();
				textNode->type = NodeType::TEXT;
				textNode->textContent = propName + ": " + propValue + ";";
				templateNode->children.push_back(textNode);
			}
		}
		
		return templateNode;
	}
	
	return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseCustom() {
	// 检测自定义元素语法 [Custom] @Style/@Element/@Var
	if (source_.find("[Custom]") != std::string::npos) {
		auto customNode = std::make_shared<CustomNode>();
		customNode->type = NodeType::CUSTOM;
		
		// 解析自定义类型和名称
		if (source_.find("@Style") != std::string::npos) {
			customNode->customType = "Style";
			// 提取样式组名称
			std::regex nameRegex(R"(@Style\s+(\w+))");
			std::smatch match;
			if (std::regex_search(source_, match, nameRegex)) {
				customNode->customName = match[1];
			}
		} else if (source_.find("@Element") != std::string::npos) {
			customNode->customType = "Element";
			// 提取元素名称
			std::regex nameRegex(R"(@Element\s+(\w+))");
			std::smatch match;
			if (std::regex_search(source_, match, nameRegex)) {
				customNode->customName = match[1];
			}
		} else if (source_.find("@Var") != std::string::npos) {
			customNode->customType = "Var";
			// 提取变量组名称
			std::regex nameRegex(R"(@Var\s+(\w+))");
			std::smatch match;
			if (std::regex_search(source_, match, nameRegex)) {
				customNode->customName = match[1];
			}
		}
		
		// 解析特例化操作
		if (source_.find("delete") != std::string::npos) {
			customNode->operation = "delete";
			// 提取删除的属性或继承
			std::regex deleteRegex(R"(delete\s+([^;]+))");
			std::smatch match;
			if (std::regex_search(source_, match, deleteRegex)) {
				customNode->operationTarget = match[1];
			}
		} else if (source_.find("insert") != std::string::npos) {
			customNode->operation = "insert";
			// 提取插入位置和目标
			std::regex insertRegex(R"(insert\s+(\w+)\s+(\w+)\s*\{)");
			std::smatch match;
			if (std::regex_search(source_, match, insertRegex)) {
				customNode->operationTarget = std::string(match[1]) + " " + std::string(match[2]);
			}
		}
		
		return customNode;
	}
	
	return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseOrigin() {
	// 检测Origin嵌入语法 [Origin] @Html/@Style/@JavaScript
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
		
		// 解析Origin名称（如果有）
		std::regex nameRegex(R"(@\w+\s+(\w+))");
		std::smatch match;
		if (std::regex_search(source_, match, nameRegex)) {
			originNode->originName = match[1];
		}
		
		return originNode;
	}
	
	return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseImport() {
	// 检测Import语法 [Import] @Html/@Style/@JavaScript/@Chtl/@CJmod
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
		
		// 解析from路径
		std::regex fromRegex(R"(from\s+["']?([^"';]+)["']?)");
		std::smatch match;
		if (std::regex_search(source_, match, fromRegex)) {
			importNode->from = match[1];
		}
		
		// 解析as别名
		std::regex asRegex(R"(as\s+(\w+))");
		if (std::regex_search(source_, match, asRegex)) {
			importNode->asName = match[1];
			importNode->hasAs = true;
		}
		
		return importNode;
	}
	
	return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseNamespace() {
	// 检测Namespace语法 [Namespace] 名称
	if (source_.find("[Namespace]") != std::string::npos) {
		auto namespaceNode = std::make_shared<NamespaceNode>();
		namespaceNode->type = NodeType::NAMESPACE;
		
		// 提取命名空间名称
		std::regex nameRegex(R"(\[Namespace\]\s+(\w+))");
		std::smatch match;
		if (std::regex_search(source_, match, nameRegex)) {
			namespaceNode->namespaceName = match[1];
		}
		
		return namespaceNode;
	}
	
	return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseConfiguration() {
	// 检测Configuration语法 [Configuration]
	if (source_.find("[Configuration]") != std::string::npos) {
		auto configNode = std::make_shared<ConfigNode>();
		configNode->type = NodeType::CONFIG;
		
		// 解析配置组名称（如果有）
		std::regex nameRegex(R"(\[Configuration\]\s*@Config\s+(\w+))");
		std::smatch match;
		if (std::regex_search(source_, match, nameRegex)) {
			configNode->configName = match[1];
		}
		
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

std::shared_ptr<BaseNode> CHTLParser::parseText() {
	// 检测text块
	if (source_.find("text") != std::string::npos) {
		auto textNode = std::make_shared<TextNode>();
		textNode->type = NodeType::TEXT;
		
		// 提取文本内容
		std::regex textRegex(R"(text\s*\{\s*["']?([^"'}]+)["']?\s*\})");
		std::smatch match;
		if (std::regex_search(source_, match, textRegex)) {
			textNode->textContent = match[1];
		}
		
		return textNode;
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