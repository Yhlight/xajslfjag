#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace CHTL {

enum class ImportType {
	HTML, STYLE, JAVASCRIPT, CHTL, CJMOD, CONFIG
};

struct ImportInfo {
	ImportType type;
	std::string what;      // 目标名/通配
	std::string from;      // 路径或模块名
	std::string asName;    // 可选别名
	bool hasAs = false;    // 是否有as语法
};

class ImportManager {
public:
	ImportManager() = default;
	
	// 解析导入语句
	ImportInfo parseImport(const std::string& importStmt);
	
	// 路径搜索策略
	std::string resolvePath(const ImportInfo& info, const std::string& currentFile);
	
	// 通配符展开
	std::vector<std::string> expandWildcard(const std::string& pattern, const std::string& basePath);
	
	// 循环依赖检测
	bool hasCircularDependency(const std::string& file, const std::string& target);
	
	// 添加导入记录
	void addImport(const std::string& file, const ImportInfo& info);
	
	// 获取导入历史
	const std::vector<ImportInfo>& getImportHistory(const std::string& file) const;

private:
	std::unordered_map<std::string, std::vector<ImportInfo>> importHistory_;
	std::unordered_set<std::string> processingFiles_; // 用于循环检测
	
	// 路径解析辅助
	std::string searchInOfficialModules(const std::string& name, ImportType type);
	std::string searchInLocalModules(const std::string& name, ImportType type);
	std::string searchInCurrentDir(const std::string& name, ImportType type);
};

} // namespace CHTL