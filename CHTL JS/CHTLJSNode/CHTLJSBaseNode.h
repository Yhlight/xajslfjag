#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTLJS {

enum class NodeType {
	VIRTUAL_OBJECT,
	ENHANCED_SELECTOR,
	CHAIN_OPERATION,
	EVENT_BINDING,
	LISTEN_BLOCK,
	DELEGATE_BLOCK,
	ANIMATE_BLOCK,
	I_NEVER_AWAY,
	PRINT_MY_LOVE
};

// 基础节点类
class CHTLJSBaseNode {
public:
	NodeType type;
	std::vector<std::shared_ptr<CHTLJSBaseNode>> children;
	
	virtual ~CHTLJSBaseNode() = default;
};

// 虚拟对象节点
class VirtualObjectNode : public CHTLJSBaseNode {
public:
	std::string objectName;
	
	VirtualObjectNode() {
		type = NodeType::VIRTUAL_OBJECT;
	}
};

// 增强选择器节点
class EnhancedSelectorNode : public CHTLJSBaseNode {
public:
	std::string selector;
	std::string selectorType; // "class", "id", "tag"
	
	EnhancedSelectorNode() {
		type = NodeType::ENHANCED_SELECTOR;
	}
};

// 链式操作节点
class ChainOperationNode : public CHTLJSBaseNode {
public:
	std::string target;
	std::string operation;
	std::string operatorType; // "->" or "."
	
	ChainOperationNode() {
		type = NodeType::CHAIN_OPERATION;
	}
};

// 事件绑定节点
class EventBindingNode : public CHTLJSBaseNode {
public:
	std::string eventType;
	
	EventBindingNode() {
		type = NodeType::EVENT_BINDING;
	}
};

// 监听块节点
class ListenBlockNode : public CHTLJSBaseNode {
public:
	std::unordered_map<std::string, std::string> events; // 事件类型 -> 回调函数
	
	ListenBlockNode() {
		type = NodeType::LISTEN_BLOCK;
	}
};

// 委托块节点
class DelegateBlockNode : public CHTLJSBaseNode {
public:
	std::string target;
	std::unordered_map<std::string, std::string> events; // 事件类型 -> 回调函数
	
	DelegateBlockNode() {
		type = NodeType::DELEGATE_BLOCK;
	}
};

// 动画块节点
class AnimateBlockNode : public CHTLJSBaseNode {
public:
	std::unordered_map<std::string, std::string> properties; // 属性名 -> 属性值
	
	AnimateBlockNode() {
		type = NodeType::ANIMATE_BLOCK;
	}
};

// iNeverAway节点
class INeverAwayNode : public CHTLJSBaseNode {
public:
	std::unordered_map<std::string, std::string> statefulKeys; // 带状态的键
	std::unordered_map<std::string, std::string> statelessKeys; // 无状态的键
	
	INeverAwayNode() {
		type = NodeType::I_NEVER_AWAY;
	}
};

// printMylove节点
class PrintMyLoveNode : public CHTLJSBaseNode {
public:
	std::unordered_map<std::string, std::string> properties; // 属性名 -> 属性值
	
	PrintMyLoveNode() {
		type = NodeType::PRINT_MY_LOVE;
	}
};

} // namespace CHTLJS