#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "../CHTLJSState/CHTLJSState.h"

namespace CHTLJS {

struct VirtualEntry {
	// 记录vir虚对象键到类型（函数/对象/数组）等元信息，占位
	std::unordered_map<std::string, std::string> keyToKind;
};

struct DelegateEntry {
	// 父选择器 -> 事件 -> 目标集合（简化占位）
	std::unordered_map<std::string, std::vector<std::string>> eventToTargets;
};

class CHTLJSContext {
public:
	CHTLJSContext() = default;
	~CHTLJSContext() = default;

	CHTLJSStateStack& state() { return stateStack_; }
	const CHTLJSStateStack& state() const { return stateStack_; }

	void registerVirtual(const std::string& name, const VirtualEntry& e) { virTable_[name] = e; }
	VirtualEntry* findVirtual(const std::string& name) {
		auto it = virTable_.find(name);
		return it == virTable_.end() ? nullptr : &it->second;
	}

	DelegateEntry& delegateFor(const std::string& parentSelector) { return delegate_[parentSelector]; }

private:
	CHTLJSStateStack stateStack_;
	std::unordered_map<std::string, VirtualEntry> virTable_;
	std::unordered_map<std::string, DelegateEntry> delegate_;
};

} // namespace CHTLJS