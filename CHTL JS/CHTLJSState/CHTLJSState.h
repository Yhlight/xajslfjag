#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

namespace CHTLJS {

enum class CHTLJSPhase {
	None,
	InListen,
	InDelegate,
	InAnimate,
	InVirtualDefine
};

struct CHTLJSFlags {
	// 局部script内选择器优先：id > class（与文档一致）
	bool preferIdInLocalScript = true;
	// vir虚对象启用
	bool enableVirtualObject = true;
};

struct CHTLJSStateFrame {
	CHTLJSPhase phase = CHTLJSPhase::None;
	CHTLJSFlags flags;
};

class CHTLJSStateStack {
public:
	void push(const CHTLJSStateFrame& f) { stack_.push_back(f); }
	void pop() { if (!stack_.empty()) stack_.pop_back(); }
	CHTLJSStateFrame* top() { return stack_.empty() ? nullptr : &stack_.back(); }
	bool empty() const { return stack_.empty(); }
private:
	std::vector<CHTLJSStateFrame> stack_;
};

} // namespace CHTLJS