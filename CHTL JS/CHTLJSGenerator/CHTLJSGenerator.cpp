#include "CHTLJSGenerator.h"
#include <sstream>
#include <algorithm>

namespace CHTLJS {

CHTLJSGenerator::CHTLJSGenerator() {
	// 初始化生成器
}

void CHTLJSGenerator::setContext(std::shared_ptr<CHTLJSContext> context) {
	context_ = context;
}

std::string CHTLJSGenerator::generate(std::shared_ptr<CHTLJSBaseNode> ast) {
	if (!ast) {
		addError("AST节点为空");
		return "";
	}
	
	try {
		// 根据节点类型生成代码
		switch (ast->type) {
			case NodeType::VIRTUAL_OBJECT:
				return generateVirtualObject(ast);
			case NodeType::ENHANCED_SELECTOR:
				return generateEnhancedSelector(ast);
			case NodeType::CHAIN_OPERATION:
				return generateChainOperation(ast);
			case NodeType::EVENT_BINDING:
				return generateEventBinding(ast);
			case NodeType::LISTEN_BLOCK:
				return generateListenBlock(ast);
			case NodeType::DELEGATE_BLOCK:
				return generateDelegateBlock(ast);
			case NodeType::ANIMATE_BLOCK:
				return generateAnimateBlock(ast);
			case NodeType::I_NEVER_AWAY:
				return generateINeverAway(ast);
			case NodeType::PRINT_MY_LOVE:
				return generatePrintMyLove(ast);
			default:
				addError("未知的节点类型: " + std::to_string(static_cast<int>(ast->type)));
				return "";
		}
		
	} catch (const std::exception& e) {
		addError("生成异常: " + std::string(e.what()));
		return "";
	}
}

std::string CHTLJSGenerator::generateVirtualObject(std::shared_ptr<CHTLJSBaseNode> node) {
	if (auto virNode = std::dynamic_pointer_cast<VirtualObjectNode>(node)) {
		std::stringstream output;
		
		output << "vir " << virNode->objectName << " = ";
		
		// 生成子元素
		for (const auto& child : virNode->children) {
			output << generate(child);
		}
		
		output << ";\n";
		
		return output.str();
	}
	
	addError("无法转换为VirtualObjectNode");
	return "";
}

std::string CHTLJSGenerator::generateEnhancedSelector(std::shared_ptr<CHTLJSBaseNode> node) {
	if (auto selectorNode = std::dynamic_pointer_cast<EnhancedSelectorNode>(node)) {
		std::stringstream output;
		
		output << "{{" << selectorNode->selector << "}}";
		
		// 生成子元素
		for (const auto& child : selectorNode->children) {
			output << generate(child);
		}
		
		output << ";\n";
		
		return output.str();
	}
	
	addError("无法转换为EnhancedSelectorNode");
	return "";
}

std::string CHTLJSGenerator::generateChainOperation(std::shared_ptr<CHTLJSBaseNode> node) {
	if (auto chainNode = std::dynamic_pointer_cast<ChainOperationNode>(node)) {
		std::stringstream output;
		
		output << chainNode->target;
		
		if (chainNode->operatorType == "->") {
			output << "->" << chainNode->operation;
		} else if (chainNode->operatorType == ".") {
			output << "." << chainNode->operation;
		}
		
		// 生成子元素
		for (const auto& child : chainNode->children) {
			output << generate(child);
		}
		
		output << ";\n";
		
		return output.str();
	}
	
	addError("无法转换为ChainOperationNode");
	return "";
}

std::string CHTLJSGenerator::generateEventBinding(std::shared_ptr<CHTLJSBaseNode> node) {
	if (auto eventNode = std::dynamic_pointer_cast<EventBindingNode>(node)) {
		std::stringstream output;
		
		output << "&->" << eventNode->eventType;
		
		// 生成子元素
		for (const auto& child : eventNode->children) {
			output << generate(child);
		}
		
		output << ";\n";
		
		return output.str();
	}
	
	addError("无法转换为EventBindingNode");
	return "";
}

std::string CHTLJSGenerator::generateListenBlock(std::shared_ptr<CHTLJSBaseNode> node) {
	if (auto listenNode = std::dynamic_pointer_cast<ListenBlockNode>(node)) {
		std::stringstream output;
		
		output << "listen {\n";
		
		// 生成事件
		for (const auto& event : listenNode->events) {
			output << "    " << event.first << ": " << event.second << ",\n";
		}
		
		// 生成子元素
		for (const auto& child : listenNode->children) {
			output << "    " << generate(child) << "\n";
		}
		
		output << "}";
		
		return output.str();
	}
	
	addError("无法转换为ListenBlockNode");
	return "";
}

std::string CHTLJSGenerator::generateDelegateBlock(std::shared_ptr<CHTLJSBaseNode> node) {
	if (auto delegateNode = std::dynamic_pointer_cast<DelegateBlockNode>(node)) {
		std::stringstream output;
		
		output << "delegate " << delegateNode->target << " {\n";
		
		// 生成事件
		for (const auto& event : delegateNode->events) {
			output << "    " << event.first << ": " << event.second << ",\n";
		}
		
		// 生成子元素
		for (const auto& child : delegateNode->children) {
			output << "    " << generate(child) << "\n";
		}
		
		output << "}";
		
		return output.str();
	}
	
	addError("无法转换为DelegateBlockNode");
	return "";
}

std::string CHTLJSGenerator::generateAnimateBlock(std::shared_ptr<CHTLJSBaseNode> node) {
	if (auto animateNode = std::dynamic_pointer_cast<AnimateBlockNode>(node)) {
		std::stringstream output;
		
		output << "animate {\n";
		
		// 生成属性
		for (const auto& property : animateNode->properties) {
			output << "    " << property.first << ": " << property.second << ",\n";
		}
		
		// 生成子元素
		for (const auto& child : animateNode->children) {
			output << "    " << generate(child) << "\n";
		}
		
		output << "}";
		
		return output.str();
	}
	
	addError("无法转换为AnimateBlockNode");
	return "";
}

std::string CHTLJSGenerator::generateINeverAway(std::shared_ptr<CHTLJSBaseNode> node) {
	if (auto ineverNode = std::dynamic_pointer_cast<INeverAwayNode>(node)) {
		std::stringstream output;
		
		output << "iNeverAway {\n";
		
		// 生成带状态的键
		for (const auto& stateful : ineverNode->statefulKeys) {
			output << "    Void<" << stateful.first << ">: " << stateful.second << ",\n";
		}
		
		// 生成无状态的键
		for (const auto& stateless : ineverNode->statelessKeys) {
			output << "    " << stateless.first << ": " << stateless.second << ",\n";
		}
		
		// 生成子元素
		for (const auto& child : ineverNode->children) {
			output << "    " << generate(child) << "\n";
		}
		
		output << "}";
		
		return output.str();
	}
	
	addError("无法转换为INeverAwayNode");
	return "";
}

std::string CHTLJSGenerator::generatePrintMyLove(std::shared_ptr<CHTLJSBaseNode> node) {
	if (auto printNode = std::dynamic_pointer_cast<PrintMyLoveNode>(node)) {
		std::stringstream output;
		
		output << "printMylove {\n";
		
		// 生成属性
		for (const auto& property : printNode->properties) {
			output << "    " << property.first << ": " << property.second << ",\n";
		}
		
		// 生成子元素
		for (const auto& child : printNode->children) {
			output << "    " << generate(child) << "\n";
		}
		
		output << "}";
		
		return output.str();
	}
	
	addError("无法转换为PrintMyLoveNode");
	return "";
}

void CHTLJSGenerator::addError(const std::string& error) {
	errors_.push_back(error);
}

std::string CHTLJSGenerator::convertEnhancedSelector(const std::string& selector) {
	// 简化实现：返回选择器
	return selector;
}

std::string CHTLJSGenerator::expandChainOperation(const std::string& operation) {
	// 简化实现：返回操作
	return operation;
}

} // namespace CHTLJS