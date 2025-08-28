#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

namespace CHTL {

struct NamespaceInfo {
	std::string name;
	std::string parent;
	std::vector<std::string> children;
	std::map<std::string, std::string> symbols; // symbol -> value
	bool isDefault;
	
	NamespaceInfo() : isDefault(false) {}
};

class NamespaceManager {
public:
	NamespaceManager();
	~NamespaceManager() = default;
	
	// 创建命名空间
	bool createNamespace(const std::string& name, const std::string& parent = "");
	
	// 合并命名空间
	bool mergeNamespace(const std::string& target, const std::string& source);
	
	// 检测冲突
	std::vector<std::string> detectConflicts(const std::string& namespace1, const std::string& namespace2);
	
	// 添加符号
	bool addSymbol(const std::string& namespaceName, const std::string& symbol, const std::string& value);
	
	// 查找符号
	std::string findSymbol(const std::string& namespaceName, const std::string& symbol);
	
	// 获取命名空间信息
	const NamespaceInfo* getNamespace(const std::string& name) const;
	
	// 设置默认命名空间
	void setDefaultNamespace(const std::string& name);
	
	// 获取默认命名空间
	std::string getDefaultNamespace() const { return defaultNamespace_; }
	
	// 检查命名空间是否存在
	bool namespaceExists(const std::string& name) const;
	
	// 获取所有命名空间
	std::vector<std::string> getAllNamespaces() const;

private:
	std::map<std::string, NamespaceInfo> namespaces_;
	std::string defaultNamespace_;
	
	// 辅助方法
	bool hasCircularDependency(const std::string& name, const std::string& parent);
	std::vector<std::string> getAncestors(const std::string& name);
	bool isDescendant(const std::string& descendant, const std::string& ancestor);
};

} // namespace CHTL