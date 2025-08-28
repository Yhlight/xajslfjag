#include "NamespaceManager.h"
#include <algorithm>
#include <stdexcept>

namespace CHTL {

NamespaceManager::NamespaceManager() : defaultNamespace_("") {}

bool NamespaceManager::createNamespace(const std::string& name, const std::string& parent) {
	if (namespaceExists(name)) {
		return false; // 命名空间已存在
	}
	
	if (!parent.empty() && !namespaceExists(parent)) {
		return false; // 父命名空间不存在
	}
	
	if (!parent.empty() && hasCircularDependency(name, parent)) {
		return false; // 循环依赖
	}
	
	NamespaceInfo info;
	info.name = name;
	info.parent = parent;
	info.isDefault = false;
	
	// 添加到父命名空间的子列表
	if (!parent.empty()) {
		auto it = namespaces_.find(parent);
		if (it != namespaces_.end()) {
			it->second.children.push_back(name);
		}
	}
	
	namespaces_[name] = info;
	return true;
}

bool NamespaceManager::mergeNamespace(const std::string& target, const std::string& source) {
	if (!namespaceExists(target) || !namespaceExists(source)) {
		return false; // 命名空间不存在
	}
	
	if (target == source) {
		return false; // 不能合并自身
	}
	
	auto& targetInfo = namespaces_[target];
	auto& sourceInfo = namespaces_[source];
	
	// 检测冲突
	auto conflicts = detectConflicts(target, source);
	if (!conflicts.empty()) {
		return false; // 存在冲突，无法合并
	}
	
	// 合并符号
	for (const auto& symbol : sourceInfo.symbols) {
		targetInfo.symbols[symbol.first] = symbol.second;
	}
	
	// 合并子命名空间
	for (const auto& child : sourceInfo.children) {
		if (std::find(targetInfo.children.begin(), targetInfo.children.end(), child) == targetInfo.children.end()) {
			targetInfo.children.push_back(child);
			// 更新子命名空间的父引用
			auto it = namespaces_.find(child);
			if (it != namespaces_.end()) {
				it->second.parent = target;
			}
		}
	}
	
	// 删除源命名空间
	namespaces_.erase(source);
	
	return true;
}

std::vector<std::string> NamespaceManager::detectConflicts(const std::string& namespace1, const std::string& namespace2) {
	std::vector<std::string> conflicts;
	
	if (!namespaceExists(namespace1) || !namespaceExists(namespace2)) {
		return conflicts;
	}
	
	const auto& info1 = namespaces_.at(namespace1);
	const auto& info2 = namespaces_.at(namespace2);
	
	// 检查符号冲突
	for (const auto& symbol : info1.symbols) {
		if (info2.symbols.find(symbol.first) != info2.symbols.end()) {
			conflicts.push_back(symbol.first);
		}
	}
	
	return conflicts;
}

bool NamespaceManager::addSymbol(const std::string& namespaceName, const std::string& symbol, const std::string& value) {
	if (!namespaceExists(namespaceName)) {
		return false;
	}
	
	namespaces_[namespaceName].symbols[symbol] = value;
	return true;
}

std::string NamespaceManager::findSymbol(const std::string& namespaceName, const std::string& symbol) {
	if (!namespaceExists(namespaceName)) {
		return "";
	}
	
	// 在当前命名空间中查找
	const auto& info = namespaces_.at(namespaceName);
	auto it = info.symbols.find(symbol);
	if (it != info.symbols.end()) {
		return it->second;
	}
	
	// 在父命名空间中查找
	if (!info.parent.empty()) {
		return findSymbol(info.parent, symbol);
	}
	
	return "";
}

const NamespaceInfo* NamespaceManager::getNamespace(const std::string& name) const {
	auto it = namespaces_.find(name);
	return it != namespaces_.end() ? &it->second : nullptr;
}

void NamespaceManager::setDefaultNamespace(const std::string& name) {
	if (namespaceExists(name)) {
		// 清除之前的默认命名空间
		for (auto& pair : namespaces_) {
			pair.second.isDefault = false;
		}
		
		// 设置新的默认命名空间
		namespaces_[name].isDefault = true;
		defaultNamespace_ = name;
	}
}

bool NamespaceManager::namespaceExists(const std::string& name) const {
	return namespaces_.find(name) != namespaces_.end();
}

std::vector<std::string> NamespaceManager::getAllNamespaces() const {
	std::vector<std::string> result;
	for (const auto& pair : namespaces_) {
		result.push_back(pair.first);
	}
	return result;
}

bool NamespaceManager::hasCircularDependency(const std::string& name, const std::string& parent) {
	if (name == parent) return true;
	
	auto ancestors = getAncestors(parent);
	return std::find(ancestors.begin(), ancestors.end(), name) != ancestors.end();
}

std::vector<std::string> NamespaceManager::getAncestors(const std::string& name) {
	std::vector<std::string> ancestors;
	
	auto it = namespaces_.find(name);
	while (it != namespaces_.end() && !it->second.parent.empty()) {
		ancestors.push_back(it->second.parent);
		it = namespaces_.find(it->second.parent);
	}
	
	return ancestors;
}

bool NamespaceManager::isDescendant(const std::string& descendant, const std::string& ancestor) {
	auto ancestors = getAncestors(descendant);
	return std::find(ancestors.begin(), ancestors.end(), ancestor) != ancestors.end();
}

} // namespace CHTL