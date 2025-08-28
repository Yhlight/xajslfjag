#include "ImportManager.h"
#include <algorithm>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

namespace CHTL {

static std::vector<std::string> candidateExtensionsFor(ImportType type) {
	// 根据文档推断后缀优先级
	switch (type) {
		case ImportType::CHTL: return {".cmod", ".chtl"};
		case ImportType::CJMOD: return {".cjmod"};
		case ImportType::HTML: return {".html", ".htm"};
		case ImportType::STYLE: return {".css"};
		case ImportType::JAVASCRIPT: return {".js"};
		case ImportType::CONFIG: return {".ini", ".conf", ".cfg"};
	}
	return {};
}

ImportInfo ImportManager::parseImport(const std::string& importStmt) {
	ImportInfo info;
	if (importStmt.find("[Import]") != std::string::npos) {
		if (importStmt.find("@Html") != std::string::npos) info.type = ImportType::HTML;
		else if (importStmt.find("@Style") != std::string::npos) info.type = ImportType::STYLE;
		else if (importStmt.find("@JavaScript") != std::string::npos) info.type = ImportType::JAVASCRIPT;
		else if (importStmt.find("@Chtl") != std::string::npos) info.type = ImportType::CHTL;
		else if (importStmt.find("@CJmod") != std::string::npos) info.type = ImportType::CJMOD;
		else if (importStmt.find("@Config") != std::string::npos) info.type = ImportType::CONFIG;
		info.hasAs = importStmt.find(" as ") != std::string::npos;
	}
	return info;
}

std::string ImportManager::resolvePath(const ImportInfo& info, const std::string& currentFile) {
	(void)currentFile;
	// ch tl:: 前缀处理
	if (info.from.find("chtl::") == 0) {
		return searchInOfficialModules(info.from.substr(6), info.type);
	}
	
	bool hasExtension = info.from.find('.') != std::string::npos;
	std::vector<std::string> searchOrder = { searchInOfficialModules(info.from, info.type),
		searchInLocalModules(info.from, info.type), searchInCurrentDir(info.from, info.type) };
	
	for (const auto& path : searchOrder) {
		if (!path.empty()) return path;
	}
	
	// 如果无后缀，尝试追加候选后缀
	if (!hasExtension) {
		for (const auto& ext : candidateExtensionsFor(info.type)) {
			std::string trial = info.from + ext;
			std::string resolved = searchInOfficialModules(trial, info.type);
			if (!resolved.empty()) return resolved;
			resolved = searchInLocalModules(trial, info.type);
			if (!resolved.empty()) return resolved;
			resolved = searchInCurrentDir(trial, info.type);
			if (!resolved.empty()) return resolved;
		}
	}
	return "";
}

std::vector<std::string> ImportManager::expandWildcard(const std::string& pattern, const std::string& basePath) {
	std::vector<std::string> results;
	if (!fs::exists(basePath)) return results;
	
	for (auto const& entry : fs::recursive_directory_iterator(basePath)) {
		if (!entry.is_regular_file()) continue;
		const std::string pathStr = entry.path().string();
		if (matchWildcard(pathStr, basePath + "/" + pattern)) {
			results.push_back(pathStr);
		}
	}
	deduplicate(results);
	return results;
}

bool ImportManager::hasCircularDependency(const std::string& file, const std::string& target) {
	if (file == target) return true;
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
	(std::void_t<ImportType>)type;
	std::string officialPath = "./module";
	fs::path p(name);
	if (p.is_absolute()) return fs::exists(p) ? p.string() : "";
	fs::path candidate = fs::path(officialPath) / p;
	if (fs::exists(candidate)) return candidate.string();
	return "";
}

std::string ImportManager::searchInLocalModules(const std::string& name, ImportType type) {
	(std::void_t<ImportType>)type;
	std::string localPath = "./module";
	fs::path p(name);
	fs::path candidate = fs::path(localPath) / p;
	if (fs::exists(candidate)) return candidate.string();
	return "";
}

std::string ImportManager::searchInCurrentDir(const std::string& name, ImportType type) {
	(std::void_t<ImportType>)type;
	fs::path p(name);
	if (fs::exists(p)) return p.string();
	return "";
}

bool ImportManager::matchWildcard(const std::string& text, const std::string& pattern) {
	// 简单*通配：将pattern转为正则
	std::string rx;
	rx.reserve(pattern.size() * 2);
	for (char c : pattern) {
		switch (c) {
			case '*': rx += ".*"; break;
			case '.': rx += "\\."; break;
			case '?': rx += "."; break;
			case '\\': rx += "\\\\"; break;
			case '/': rx += "/"; break;
			default: rx += c; break;
		}
	}
	std::regex re(rx);
	return std::regex_match(text, re);
}

void ImportManager::deduplicate(std::vector<std::string>& items) {
	std::sort(items.begin(), items.end());
	items.erase(std::unique(items.begin(), items.end()), items.end());
}

void ImportManager::addSearchPath(const std::string& path) {
	auto normalized = normalizePath(path);
	if (std::find(searchPaths_.begin(), searchPaths_.end(), normalized) == searchPaths_.end()) {
		searchPaths_.push_back(normalized);
	}
}

void ImportManager::removeSearchPath(const std::string& path) {
	auto normalized = normalizePath(path);
	searchPaths_.erase(std::remove(searchPaths_.begin(), searchPaths_.end(), normalized), searchPaths_.end());
}

std::vector<std::string> ImportManager::getSearchPaths() const {
	return searchPaths_;
}

std::string ImportManager::resolveAlias(const std::string& alias) const {
	auto it = aliases_.find(alias);
	return it != aliases_.end() ? it->second : "";
}

void ImportManager::addAlias(const std::string& alias, const std::string& path) {
	aliases_[alias] = normalizePath(path);
}

std::string ImportManager::resolveRelativePath(const std::string& relativePath, const std::string& baseFile) const {
	if (relativePath.empty() || baseFile.empty()) return "";
	
	// 处理相对路径
	if (relativePath[0] == '.' || relativePath[0] == '/') {
		fs::path basePath(baseFile);
		fs::path relative(relativePath);
		
		if (relativePath[0] == '.') {
			// 相对路径
			fs::path resolved = basePath.parent_path() / relative;
			return resolved.string();
		} else {
			// 绝对路径
			return relative.string();
		}
	}
	
	return relativePath;
}

std::string ImportManager::searchInMultiplePaths(const std::string& name, ImportType type) {
	// 在多根目录中搜索
	for (const auto& searchPath : searchPaths_) {
		fs::path candidate = fs::path(searchPath) / name;
		if (fs::exists(candidate)) {
			return candidate.string();
		}
		
		// 尝试添加后缀
		for (const auto& ext : candidateExtensionsFor(type)) {
			fs::path candidateWithExt = fs::path(searchPath) / (name + ext);
			if (fs::exists(candidateWithExt)) {
				return candidateWithExt.string();
			}
		}
	}
	
	return "";
}

std::string ImportManager::normalizePath(const std::string& path) {
	fs::path p(path);
	return p.lexically_normal().string();
}

} // namespace CHTL