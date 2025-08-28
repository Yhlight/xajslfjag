#pragma once

#include <string>
#include <vector>
#include <optional>

namespace CHTL {

// CHTL编译阶段的细粒度状态
enum class CHTLPhase {
	None,
	InGlobalStyle,
	InLocalStyle,
	InLocalScript,
	InElement,
	InTemplate,
	InCustom,
	InOrigin,
	InNamespace,
	InConfiguration
};

struct CHTLConfigFlags {
	bool disableNameGroup = true;
	bool disableCustomOriginType = false;
	bool debugMode = false;
	bool disableDefaultNamespace = false;
	bool disableStyleAutoAddClass = false;
	bool disableStyleAutoAddId = false;
	bool disableScriptAutoAddClass = true;
	bool disableScriptAutoAddId = true;
};

struct CHTLStateFrame {
	CHTLPhase phase = CHTLPhase::None;
	std::string currentNamespace; // 允许空 = 未设置
	std::optional<std::string> usingConfigName; // use @Config X; 时设置
	CHTLConfigFlags flags;
};

class CHTLStateStack {
public:
	void push(const CHTLStateFrame& frame) { stack_.push_back(frame); }
	void pop() { if (!stack_.empty()) stack_.pop_back(); }
	CHTLStateFrame* top() { return stack_.empty() ? nullptr : &stack_.back(); }
	const CHTLStateFrame* top() const { return stack_.empty() ? nullptr : &stack_.back(); }
	bool empty() const { return stack_.empty(); }
	std::size_t size() const { return stack_.size(); }
private:
	std::vector<CHTLStateFrame> stack_;
};

// RAII态域
class CHTLStateScope {
public:
	CHTLStateScope(CHTLStateStack& s, const CHTLStateFrame& f) : stack_(s) { stack_.push(f); }
	~CHTLStateScope() { stack_.pop(); }
private:
	CHTLStateStack& stack_;
};

} // namespace CHTL