#include "CHTLJSParser.h"
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTLJS {

CHTLJSParser::CHTLJSParser() {
	lexer_ = std::make_shared<CHTLJSLexer>();
}

void CHTLJSParser::setSource(const std::string& source) {
	source_ = source;
	lexer_->setSource(source);
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parse() {
	// 重置状态
	errors_.clear();
	
	try {
		// 解析虚拟对象
		auto virNode = parseVirtualObject();
		if (virNode) {
			return virNode;
		}
		
		// 解析增强选择器
		auto selectorNode = parseEnhancedSelector();
		if (selectorNode) {
			return selectorNode;
		}
		
		// 解析链式操作
		auto chainNode = parseChainOperation();
		if (chainNode) {
			return chainNode;
		}
		
		// 解析事件绑定
		auto eventNode = parseEventBinding();
		if (eventNode) {
			return eventNode;
		}
		
		// 解析监听块
		auto listenNode = parseListenBlock();
		if (listenNode) {
			return listenNode;
		}
		
		// 解析委托块
		auto delegateNode = parseDelegateBlock();
		if (delegateNode) {
			return delegateNode;
		}
		
		// 解析动画块
		auto animateNode = parseAnimateBlock();
		if (animateNode) {
			return animateNode;
		}
		
		// 解析特殊函数
		auto ineverNode = parseINeverAway();
		if (ineverNode) {
			return ineverNode;
		}
		
		auto printNode = parsePrintMyLove();
		if (printNode) {
			return printNode;
		}
		
		addError("无法解析任何有效的CHTL JS语法");
		return nullptr;
		
	} catch (const std::exception& e) {
		addError("解析异常: " + std::string(e.what()));
		return nullptr;
	}
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseVirtualObject() {
	// 检测虚拟对象语法 vir 名称 = listen { ... }
	if (source_.find("vir ") != std::string::npos && source_.find("=") != std::string::npos) {
		auto virNode = std::make_shared<VirtualObjectNode>();
		virNode->type = NodeType::VIRTUAL_OBJECT;
		
		// 解析虚拟对象名称
		std::regex nameRegex(R"(vir\s+(\w+)\s*=)");
		std::smatch match;
		if (std::regex_search(source_, match, nameRegex)) {
			virNode->objectName = match[1];
		}
		
		// 检查是否有listen块
		if (source_.find("listen") != std::string::npos) {
			auto listenNode = parseListenBlock();
			if (listenNode) {
				virNode->children.push_back(listenNode);
			}
		}
		
		// 检查是否有iNeverAway块
		if (source_.find("iNeverAway") != std::string::npos) {
			auto ineverNode = parseINeverAway();
			if (ineverNode) {
				virNode->children.push_back(ineverNode);
			}
		}
		
		return virNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseEnhancedSelector() {
	// 检测增强选择器语法 {{CSS选择器}}
	if (source_.find("{{") != std::string::npos && source_.find("}}") != std::string::npos) {
		auto selectorNode = std::make_shared<EnhancedSelectorNode>();
		selectorNode->type = NodeType::ENHANCED_SELECTOR;
		
		// 提取选择器内容
		std::regex selectorRegex(R"(\{\{([^}]+)\}\})");
		std::smatch match;
		if (std::regex_search(source_, match, selectorRegex)) {
			selectorNode->selector = match[1];
			
			// 解析选择器类型
			if (selectorNode->selector.front() == '.') {
				selectorNode->selectorType = "class";
			} else if (selectorNode->selector.front() == '#') {
				selectorNode->selectorType = "id";
			} else {
				selectorNode->selectorType = "tag";
			}
		}
		
		// 检查是否有链式操作
		if (source_.find("->") != std::string::npos) {
			auto chainNode = parseChainOperation();
			if (chainNode) {
				selectorNode->children.push_back(chainNode);
			}
		}
		
		return selectorNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseChainOperation() {
	// 检测链式操作语法 -> 或 .
	if (source_.find("->") != std::string::npos || source_.find(".") != std::string::npos) {
		auto chainNode = std::make_shared<ChainOperationNode>();
		chainNode->type = NodeType::CHAIN_OPERATION;
		
		// 解析链式操作
		if (source_.find("->") != std::string::npos) {
			chainNode->operatorType = "->";
			size_t arrowPos = source_.find("->");
			if (arrowPos != std::string::npos) {
				chainNode->target = source_.substr(0, arrowPos);
				chainNode->operation = source_.substr(arrowPos + 2);
			}
		} else if (source_.find(".") != std::string::npos) {
			chainNode->operatorType = ".";
			size_t dotPos = source_.find(".");
			if (dotPos != std::string::npos) {
				chainNode->target = source_.substr(0, dotPos);
				chainNode->operation = source_.substr(dotPos + 1);
			}
		}
		
		return chainNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseEventBinding() {
	// 检测事件绑定语法 &-> 事件类型
	if (source_.find("&->") != std::string::npos) {
		auto eventNode = std::make_shared<EventBindingNode>();
		eventNode->type = NodeType::EVENT_BINDING;
		
		// 解析事件绑定
		std::regex eventRegex(R"(&->\s*(\w+))");
		std::smatch match;
		if (std::regex_search(source_, match, eventRegex)) {
			eventNode->eventType = match[1];
		}
		
		return eventNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseListenBlock() {
	// 检测listen块语法 listen { 事件类型: 回调函数, ... }
	if (source_.find("listen") != std::string::npos) {
		auto listenNode = std::make_shared<ListenBlockNode>();
		listenNode->type = NodeType::LISTEN_BLOCK;
		
		// 解析事件类型和回调函数
		// 使用简单的正则表达式
		std::regex eventRegex(R"((\w+)\s*:\s*([^,}]+))");
		std::sregex_iterator iter(source_.begin(), source_.end(), eventRegex);
		std::sregex_iterator end;
		
		for (; iter != end; ++iter) {
			std::string eventType = (*iter)[1];
			std::string callback = (*iter)[2];
			
			// 去除空白字符
			eventType.erase(0, eventType.find_first_not_of(" \t\n\r"));
			eventType.erase(eventType.find_last_not_of(" \t\n\r") + 1);
			callback.erase(0, callback.find_first_not_of(" \t\n\r"));
			callback.erase(callback.find_last_not_of(" \t\n\r") + 1);
			
			// 跳过空的事件类型和特殊关键字
			if (!eventType.empty() && eventType != "target" && eventType != "vir" && eventType != "listen") {
				listenNode->events[eventType] = callback;
			}
		}
		
		return listenNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseDelegateBlock() {
	// 检测delegate块语法 delegate { target: 选择器, 事件类型: 回调函数, ... }
	if (source_.find("delegate") != std::string::npos) {
		auto delegateNode = std::make_shared<DelegateBlockNode>();
		delegateNode->type = NodeType::DELEGATE_BLOCK;
		
		// 解析target选择器
		std::regex targetRegex(R"(target\s*:\s*([^,}]+))");
		std::smatch match;
		if (std::regex_search(source_, match, targetRegex)) {
			delegateNode->target = match[1];
		}
		
		// 解析事件类型和回调函数
		std::regex eventRegex(R"((\w+)\s*:\s*([^,}]+))");
		std::string source = source_;
		
		std::sregex_iterator iter(source.begin(), source.end(), eventRegex);
		std::sregex_iterator end;
		
		for (; iter != end; ++iter) {
			std::string eventType = (*iter)[1];
			std::string callback = (*iter)[2];
			
			// 跳过target
			if (eventType == "target") continue;
			
			// 去除空白字符
			eventType.erase(0, eventType.find_first_not_of(" \t\n\r"));
			eventType.erase(eventType.find_last_not_of(" \t\n\r") + 1);
			callback.erase(0, callback.find_first_not_of(" \t\n\r"));
			callback.erase(callback.find_last_not_of(" \t\n\r") + 1);
			
			delegateNode->events[eventType] = callback;
		}
		
		return delegateNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseAnimateBlock() {
	// 检测animate块语法 animate { target: 选择器, duration: 时间, ... }
	if (source_.find("animate") != std::string::npos) {
		auto animateNode = std::make_shared<AnimateBlockNode>();
		animateNode->type = NodeType::ANIMATE_BLOCK;
		
		// 解析动画属性
		std::regex propRegex(R"((\w+)\s*:\s*([^,}]+))");
		std::string source = source_;
		
		std::sregex_iterator iter(source.begin(), source.end(), propRegex);
		std::sregex_iterator end;
		
		for (; iter != end; ++iter) {
			std::string propName = (*iter)[1];
			std::string propValue = (*iter)[2];
			
			// 去除空白字符
			propName.erase(0, propName.find_first_not_of(" \t\n\r"));
			propName.erase(propName.find_last_not_of(" \t\n\r") + 1);
			propValue.erase(0, propValue.find_first_not_of(" \t\n\r"));
			propValue.erase(propValue.find_last_not_of(" \t\n\r") + 1);
			
			animateNode->properties[propName] = propValue;
		}
		
		return animateNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseINeverAway() {
	// 检测iNeverAway语法 iNeverAway { 键<状态>: 函数, ... }
	if (source_.find("iNeverAway") != std::string::npos) {
		auto ineverNode = std::make_shared<INeverAwayNode>();
		ineverNode->type = NodeType::I_NEVER_AWAY;
		
		// 解析键值对
		std::regex keyRegex(R"((\w+)(?:<(\w+)>)?\s*:\s*([^,}]+))");
		std::string source = source_;
		
		std::sregex_iterator iter(source.begin(), source.end(), keyRegex);
		std::sregex_iterator end;
		
		for (; iter != end; ++iter) {
			std::string key = (*iter)[1];
			std::string state = (*iter)[2];
			std::string value = (*iter)[3];
			
			// 去除空白字符
			key.erase(0, key.find_first_not_of(" \t\n\r"));
			key.erase(key.find_last_not_of(" \t\n\r") + 1);
			state.erase(0, state.find_first_not_of(" \t\n\r"));
			state.erase(state.find_last_not_of(" \t\n\r") + 1);
			value.erase(0, value.find_first_not_of(" \t\n\r"));
			value.erase(value.find_last_not_of(" \t\n\r") + 1);
			
			if (!state.empty()) {
				ineverNode->statefulKeys[key + "<" + state + ">"] = value;
			} else {
				ineverNode->statelessKeys[key] = value;
			}
		}
		
		return ineverNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parsePrintMyLove() {
	// 检测printMylove语法 printMylove { url: 图片路径, mode: 模式, ... }
	if (source_.find("printMylove") != std::string::npos) {
		auto printNode = std::make_shared<PrintMyLoveNode>();
		printNode->type = NodeType::PRINT_MY_LOVE;
		
		// 解析printMylove属性
		std::regex propRegex(R"((\w+)\s*:\s*([^,}]+))");
		std::string source = source_;
		
		std::sregex_iterator iter(source.begin(), source.end(), propRegex);
		std::sregex_iterator end;
		
		for (; iter != end; ++iter) {
			std::string propName = (*iter)[1];
			std::string propValue = (*iter)[2];
			
			// 去除空白字符
			propName.erase(0, propName.find_first_not_of(" \t\n\r"));
			propName.erase(propName.find_last_not_of(" \t\n\r") + 1);
			propValue.erase(0, propValue.find_first_not_of(" \t\n\r"));
			propValue.erase(propValue.find_last_not_of(" \t\n\r") + 1);
			
			printNode->properties[propName] = propValue;
		}
		
		return printNode;
	}
	
	return nullptr;
}

void CHTLJSParser::addError(const std::string& error) {
	errors_.push_back(error);
}

bool CHTLJSParser::expectToken(CHTLJSTokenType type) {
	// 简化实现：总是返回true
	(void)type;
	return true;
}

void CHTLJSParser::skipToToken(CHTLJSTokenType type) {
	// 简化实现：无操作
	(void)type;
}

} // namespace CHTLJS