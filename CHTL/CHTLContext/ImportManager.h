#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace CHTL {

enum class ImportType {
	HTML,
	STYLE,
	JAVASCRIPT,
	CHTL,
	CJMOD,
	CONFIG
};

struct ImportInfo {
	ImportType type{ImportType::CHTL};
	std::string what;
	std::string from;
	std::string asName;
	bool hasAs{false};
};

class ImportManager {
public:
	ImportInfo parseImport(const std::string& importStmt);
	std::string resolvePath(const ImportInfo& info, const std::string& currentFile);
	std::vector<std::string> expandWildcard(const std::string& pattern, const std::string& basePath);
	bool hasCircularDependency(const std::string& file, const std::string& target);
	void addImport(const std::string& file, const ImportInfo& info);
	const std::vector<ImportInfo>& getImportHistory(const std::string& file) const;

private:
	std::unordered_map<std::string, std::vector<ImportInfo>> importHistory_;
	std::unordered_set<std::string> visitedPaths_;
	
	// 路径搜索
	std::string searchInOfficialModules(const std::string& name, ImportType type);
	std::string searchInLocalModules(const std::string& name, ImportType type);
	std::string searchInCurrentDir(const std::string& name, ImportType type);
	
	// 工具方法
	static bool matchWildcard(const std::string& text, const std::string& pattern);
	static void deduplicate(std::vector<std::string>& items);
};

} // namespace CHTL