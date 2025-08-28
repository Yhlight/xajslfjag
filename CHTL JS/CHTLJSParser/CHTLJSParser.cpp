#include "CHTLJSParser.h"
#include <sstream>

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
	// 检测虚拟对象语法
	if (source_.find("vir ") != std::string::npos) {
		auto virNode = std::make_shared<VirtualObjectNode>();
		virNode->type = NodeType::VIRTUAL_OBJECT;
		
		// 解析虚拟对象名称
		size_t virPos = source_.find("vir ");
		if (virPos != std::string::npos) {
			size_t start = virPos + 4;
			size_t end = source_.find(" ", start);
			if (end != std::string::npos) {
				virNode->objectName = source_.substr(start, end - start);
			}
		}
		
		// 检查是否有listen块
		if (source_.find("listen") != std::string::npos) {
			auto listenNode = parseListenBlock();
			if (listenNode) {
				virNode->children.push_back(listenNode);
			}
		}
		
		return virNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseEnhancedSelector() {
	// 检测增强选择器语法 {{}}
	if (source_.find("{{") != std::string::npos && source_.find("}}") != std::string::npos) {
		auto selectorNode = std::make_shared<EnhancedSelectorNode>();
		selectorNode->type = NodeType::ENHANCED_SELECTOR;
		
		// 提取选择器内容
		size_t start = source_.find("{{") + 2;
		size_t end = source_.find("}}");
		if (end != std::string::npos && end > start) {
			selectorNode->selector = source_.substr(start, end - start);
		}
		
		return selectorNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseChainOperation() {
	// 检测链式操作语法 ->
	if (source_.find("->") != std::string::npos) {
		auto chainNode = std::make_shared<ChainOperationNode>();
		chainNode->type = NodeType::CHAIN_OPERATION;
		
		// 解析链式操作
		size_t arrowPos = source_.find("->");
		if (arrowPos != std::string::npos) {
			chainNode->target = source_.substr(0, arrowPos);
			chainNode->operation = source_.substr(arrowPos + 2);
		}
		
		return chainNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseEventBinding() {
	// 检测事件绑定语法 &->
	if (source_.find("&->") != std::string::npos) {
		auto eventNode = std::make_shared<EventBindingNode>();
		eventNode->type = NodeType::EVENT_BINDING;
		
		// 解析事件绑定
		size_t bindingPos = source_.find("&->");
		if (bindingPos != std::string::npos) {
			eventNode->eventType = source_.substr(bindingPos + 3);
		}
		
		return eventNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseListenBlock() {
	// 检测listen块语法
	if (source_.find("listen") != std::string::npos) {
		auto listenNode = std::make_shared<ListenBlockNode>();
		listenNode->type = NodeType::LISTEN_BLOCK;
		
		// 解析事件类型
		if (source_.find("click:") != std::string::npos) {
			listenNode->eventType = "click";
		} else if (source_.find("hover:") != std::string::npos) {
			listenNode->eventType = "hover";
		} else if (source_.find("input:") != std::string::npos) {
			listenNode->eventType = "input";
		}
		
		return listenNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseDelegateBlock() {
	// 检测delegate块语法
	if (source_.find("delegate") != std::string::npos) {
		auto delegateNode = std::make_shared<DelegateBlockNode>();
		delegateNode->type = NodeType::DELEGATE_BLOCK;
		return delegateNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseAnimateBlock() {
	// 检测animate块语法
	if (source_.find("animate") != std::string::npos) {
		auto animateNode = std::make_shared<AnimateBlockNode>();
		animateNode->type = NodeType::ANIMATE_BLOCK;
		return animateNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseINeverAway() {
	// 检测iNeverAway语法
	if (source_.find("iNeverAway") != std::string::npos) {
		auto ineverNode = std::make_shared<INeverAwayNode>();
		ineverNode->type = NodeType::I_NEVER_AWAY;
		return ineverNode;
	}
	
	return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parsePrintMyLove() {
	// 检测printMylove语法
	if (source_.find("printMylove") != std::string::npos) {
		auto printNode = std::make_shared<PrintMyLoveNode>();
		printNode->type = NodeType::PRINT_MY_LOVE;
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