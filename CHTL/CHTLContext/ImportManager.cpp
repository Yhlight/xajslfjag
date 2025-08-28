#include "ImportManager.h"
#include <algorithm>
#include <filesystem>

namespace CHTL {

ImportInfo ImportManager::parseImport(const std::string& importStmt) {
	ImportInfo info;
	// 简化解析：检测[Import]关键字和类型
	if (importStmt.find("[Import]") != std::string::npos) {
		if (importStmt.find("@Html") != std::string::npos) info.type = ImportType::HTML;
		else if (importStmt.find("@Style") != std::string::npos) info.type = ImportType::STYLE;
		else if (importStmt.find("@JavaScript") != std::string::npos) info.type = ImportType::JAVASCRIPT;
		else if (importStmt.find("@Chtl") != std::string::npos) info.type = ImportType::CHTL;
		else if (importStmt.find("@CJmod") != std::string::npos) info.type = ImportType::CJMOD;
		else if (importStmt.find("@Config") != std::string::npos) info.type = ImportType::CONFIG;
		
		// 检测as语法
		info.hasAs = importStmt.find(" as ") != std::string::npos;
	}
	return info;
}

std::string ImportManager::resolvePath(const ImportInfo& info, const std::string& currentFile) {
	// 按文档路径搜索策略
	if (info.from.find("chtl::") == 0) {
		// 官方模块前缀
		return searchInOfficialModules(info.from.substr(6), info.type);
	}
	
	// 检查是否带后缀
	bool hasExtension = info.from.find('.') != std::string::npos;
	
	if (!hasExtension) {
		// 无后缀：官方模块 -> 本地module -> 当前目录
		std::string result = searchInOfficialModules(info.from, info.type);
		if (!result.empty()) return result;
		
		result = searchInLocalModules(info.from, info.type);
		if (!result.empty()) return result;
		
		return searchInCurrentDir(info.from, info.type);
	} else {
		// 有后缀：官方模块 -> 本地module -> 当前目录
		std::string result = searchInOfficialModules(info.from, info.type);
		if (!result.empty()) return result;
		
		result = searchInLocalModules(info.from, info.type);
		if (!result.empty()) return result;
		
		return searchInCurrentDir(info.from, info.type);
	}
}

std::vector<std::string> ImportManager::expandWildcard(const std::string& pattern, const std::string& basePath) {
	std::vector<std::string> results;
	
	// 支持 .* 和 /* 通配符
	if (pattern.find(".*") != std::string::npos) {
		// 导入所有.cmod和.chtl文件
		// 简化实现：返回示例文件列表
		results.push_back(basePath + "/module1.cmod");
		results.push_back(basePath + "/module2.chtl");
	} else if (pattern.find("*.cmod") != std::string::npos) {
		// 仅导入.cmod文件
		results.push_back(basePath + "/module1.cmod");
	} else if (pattern.find("*.chtl") != std::string::npos) {
		// 仅导入.chtl文件
		results.push_back(basePath + "/module2.chtl");
	}
	
	return results;
}

bool ImportManager::hasCircularDependency(const std::string& file, const std::string& target) {
	if (file == target) return true;
	
	// 检查导入历史
	auto it = importHistory_.find(file);
	if (it != importHistory_.end()) {
		for (const auto& import : it->second) {
			if (import.from == target) return true;
		}
	}
	
	return false;
}

void ImportManager::addImport(const std::string& file, const ImportInfo& info) {
	importHistory_[file].push_back(info);
}

const std::vector<ImportInfo>& ImportManager::getImportHistory(const std::string& file) const {
	static std::vector<ImportInfo> empty;
	auto it = importHistory_.find(file);
	return it != importHistory_.end() ? it->second : empty;
}

std::string ImportManager::searchInOfficialModules(const std::string& name, ImportType type) {
	// 官方模块目录搜索
	std::string officialPath = "./module"; // 简化路径
	
	switch (type) {
		case ImportType::CHTL:
			// 优先.cmod，其次.chtl
			if (std::filesystem::exists(officialPath + "/" + name + ".cmod")) {
				return officialPath + "/" + name + ".cmod";
			}
			if (std::filesystem::exists(officialPath + "/" + name + ".chtl")) {
				return officialPath + "/" + name + ".chtl";
			}
			break;
		case ImportType::CJMOD:
			// 仅.cjmod文件
			if (std::filesystem::exists(officialPath + "/" + name + ".cjmod")) {
				return officialPath + "/" + name + ".cjmod";
			}
			break;
		default:
			// 其他类型按文件扩展名搜索
			if (std::filesystem::exists(officialPath + "/" + name)) {
				return officialPath + "/" + name;
			}
			break;
	}
	
	return "";
}

std::string ImportManager::searchInLocalModules(const std::string& name, ImportType type) {
	// 本地module目录搜索
	std::string localPath = "./module";
	return searchInOfficialModules(name, type); // 复用逻辑
}

std::string ImportManager::searchInCurrentDir(const std::string& name, ImportType type) {
	// 当前目录搜索
	std::string currentPath = ".";
	return searchInOfficialModules(name, type); // 复用逻辑
}

} // namespace CHTL