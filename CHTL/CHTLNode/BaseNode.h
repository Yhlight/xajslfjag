#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

enum class NodeType {
	ELEMENT,
	TEXT,
	COMMENT,
	STYLE,
	SCRIPT,
	ORIGIN,
	TEMPLATE,
	CUSTOM,
	IMPORT,
	NAMESPACE,
	CONFIG,
	OPERATOR
};

class BaseNode {
public:
	NodeType type;
	virtual ~BaseNode() = default;
	std::vector<std::shared_ptr<BaseNode>> children;
};

class ElementNode : public BaseNode {
public:
	std::string tagName;
	std::unordered_map<std::string, std::string> attributes; // key:value (CE对等式: :/=)
	
	ElementNode() {
		type = NodeType::ELEMENT;
	}
};

class TextNode : public BaseNode {
public:
	std::string textContent;
	
	TextNode() {
		type = NodeType::TEXT;
	}
};

class CommentNode : public BaseNode {
public:
	std::string content;
	
	CommentNode() {
		type = NodeType::COMMENT;
	}
};

class StyleNode : public BaseNode {
public:
	bool isLocal = false; // 局部样式块
	std::string raw;      // 原始样式文本（在统一合并时交给CSS编译器）
	
	StyleNode() {
		type = NodeType::STYLE;
	}
};

class ScriptNode : public BaseNode {
public:
	bool isLocal = false; // 局部脚本块
	std::string raw;      // 原始脚本文本（交由JS/CHTL JS编译器调度）
	
	ScriptNode() {
		type = NodeType::SCRIPT;
	}
};

class OriginNode : public BaseNode {
public:
	std::string originType; // @Html/@Style/@JavaScript/自定义
	std::string originName; // 可选名称
	std::string content;    // 原样输出
	
	OriginNode() {
		type = NodeType::ORIGIN;
	}
};

class TemplateNode : public BaseNode {
public:
	std::string templateType;   // @Style/@Element/@Var
	std::string templateName;
	std::string inheritType;    // 继承类型
	std::string inheritName;    // 继承名称
	
	TemplateNode() {
		type = NodeType::TEMPLATE;
	}
};

class CustomNode : public BaseNode {
public:
	std::string customType;   // @Style/@Element/@Var
	std::string customName;
	std::string operation;    // delete/insert等操作
	std::string operationTarget; // 操作目标
	
	CustomNode() {
		type = NodeType::CUSTOM;
	}
};

class ImportNode : public BaseNode {
public:
	std::string importType;   // @Html/@Style/@JavaScript/@Chtl/@CJmod/@Config/以及[Custom]/[Template]/[Origin]
	std::string what;   // 目标名/通配
	std::string from;   // 路径或模块名
	std::string asName; // 可选
	bool hasAs = false; // 是否有as关键字
	
	ImportNode() {
		type = NodeType::IMPORT;
	}
};

class NamespaceNode : public BaseNode {
public:
	std::string namespaceName;
	
	NamespaceNode() {
		type = NodeType::NAMESPACE;
	}
};

class ConfigNode : public BaseNode {
public:
	std::string configName; // 可选 @Config Name
	
	ConfigNode() {
		type = NodeType::CONFIG;
	}
};

class OperatorNode : public BaseNode {
public:
	std::string op; // delete/insert/use/...
	std::string target;
	
	OperatorNode() {
		type = NodeType::OPERATOR;
	}
};

} // namespace CHTL