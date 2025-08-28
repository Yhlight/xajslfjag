#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

class Node {
public:
	virtual ~Node() = default;
	std::vector<std::shared_ptr<Node>> children;
};

class EnhancedSelectorNode : public Node {
public:
	std::string selector; // {{...}}
};

class VirtualDefineNode : public Node {
public:
	std::string name; // vir Name
};

class ListenNode : public Node {
public:
	// key: event, value: handler原文/引用名
	std::vector<std::pair<std::string, std::string>> handlers;
};

class DelegateNode : public Node {
public:
	std::string parent;
	std::vector<std::string> targets;
	std::vector<std::pair<std::string, std::string>> handlers;
};

class AnimateNode : public Node {
public:
	std::string target; // 选择器或列表简化占位
};

} // namespace CHTLJS