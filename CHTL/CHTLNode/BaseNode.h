#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class BaseNode {
public:
	virtual ~BaseNode() = default;
	std::vector<std::shared_ptr<BaseNode>> children;
};

class ElementNode : public BaseNode {
public:
	std::string tag;
	std::vector<std::pair<std::string, std::string>> attributes; // key:value (CE对等式: :/=
};

class TextNode : public BaseNode {
public:
	std::string text;
};

class CommentNode : public BaseNode {
public:
	std::string content;
};

class StyleNode : public BaseNode {
public:
	bool isLocal = false; // 局部样式块
	std::string raw;      // 原始样式文本（在统一合并时交给CSS编译器）
};

class ScriptNode : public BaseNode {
public:
	bool isLocal = false; // 局部脚本块
	std::string raw;      // 原始脚本文本（交由JS/CHTL JS编译器调度）
};

class OriginNode : public BaseNode {
public:
	std::string originType; // @Html/@Style/@JavaScript/自定义
	std::string name;       // 可选名称
	std::string content;    // 原样输出
};

class TemplateNode : public BaseNode {
public:
	std::string kind;   // @Style/@Element/@Var
	std::string name;
};

class CustomNode : public BaseNode {
public:
	std::string kind;   // @Style/@Element/@Var
	std::string name;
};

class ImportNode : public BaseNode {
public:
	std::string kind;   // @Html/@Style/@JavaScript/@Chtl/@CJmod/@Config/以及[Custom]/[Template]/[Origin]
	std::string what;   // 目标名/通配
	std::string from;   // 路径或模块名
	std::string asName; // 可选
};

class NamespaceNode : public BaseNode {
public:
	std::string name;
};

class ConfigNode : public BaseNode {
public:
	std::string name; // 可选 @Config Name
};

class OperatorNode : public BaseNode {
public:
	std::string op; // delete/insert/use/...
	std::string target;
};

} // namespace CHTL