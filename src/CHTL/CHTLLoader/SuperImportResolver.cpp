#include "ImportResolver.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <cmath>

namespace CHTL {

// ========== FuzzyPathMatcher 实现 ==========

bool SuperImportResolver::FuzzyPathMatcher::matches(const String& path, const String& pattern) const {
    return getMatchScore(path, pattern) >= 0.7; // 70% 相似度阈值
}

double SuperImportResolver::FuzzyPathMatcher::getMatchScore(const String& path, const String& pattern) const {
    // 使用 Jaro-Winkler 距离算法
    return calculateJaroWinkler(path, pattern);
}

StringVector SuperImportResolver::FuzzyPathMatcher::getSuggestions(const String& path, const StringVector& candidates) const {
    StringVector suggestions;
    std::vector<std::pair<String, double>> scores;
    
    for (const String& candidate : candidates) {
        double score = getMatchScore(path, candidate);
        if (score >= 0.5) { // 50% 相似度阈值
            scores.emplace_back(candidate, score);
        }
    }
    
    // 按分数排序
    std::sort(scores.begin(), scores.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // 返回前10个建议
    size_t maxSuggestions = std::min(scores.size(), size_t(10));
    for (size_t i = 0; i < maxSuggestions; ++i) {
        suggestions.push_back(scores[i].first);
    }
    
    return suggestions;
}

double SuperImportResolver::FuzzyPathMatcher::calculateLevenshteinDistance(const String& s1, const String& s2) const {
    size_t len1 = s1.length();
    size_t len2 = s2.length();
    
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));
    
    for (size_t i = 0; i <= len1; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= len2; ++j) dp[0][j] = j;
    
    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            if (s1[i-1] == s2[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                dp[i][j] = 1 + std::min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
            }
        }
    }
    
    return double(dp[len1][len2]);
}

double SuperImportResolver::FuzzyPathMatcher::calculateJaroWinkler(const String& s1, const String& s2) const {
    if (s1 == s2) return 1.0;
    
    size_t len1 = s1.length();
    size_t len2 = s2.length();
    
    if (len1 == 0 || len2 == 0) return 0.0;
    
    size_t matchWindow = std::max(len1, len2) / 2 - 1;
    if (matchWindow == 0 && len1 == len2 && len1 == 1) matchWindow = 1;
    
    std::vector<bool> s1Matches(len1, false);
    std::vector<bool> s2Matches(len2, false);
    
    size_t matches = 0;
    for (size_t i = 0; i < len1; ++i) {
        size_t start = (i >= matchWindow) ? i - matchWindow : 0;
        size_t end = std::min(i + matchWindow + 1, len2);
        
        for (size_t j = start; j < end; ++j) {
            if (s2Matches[j] || s1[i] != s2[j]) continue;
            s1Matches[i] = true;
            s2Matches[j] = true;
            matches++;
            break;
        }
    }
    
    if (matches == 0) return 0.0;
    
    // 计算转置
    size_t transpositions = 0;
    size_t k = 0;
    for (size_t i = 0; i < len1; ++i) {
        if (!s1Matches[i]) continue;
        while (!s2Matches[k]) k++;
        if (s1[i] != s2[k]) transpositions++;
        k++;
    }
    
    double jaro = (double(matches) / len1 + double(matches) / len2 + 
                   (matches - transpositions / 2.0) / matches) / 3.0;
    
    // Winkler 修正
    size_t prefixLen = 0;
    for (size_t i = 0; i < std::min(len1, len2) && i < 4; ++i) {
        if (s1[i] == s2[i]) prefixLen++;
        else break;
    }
    
    return jaro + (0.1 * prefixLen * (1.0 - jaro));
}

// ========== GlobPathMatcher 实现 ==========

bool SuperImportResolver::GlobPathMatcher::matches(const String& path, const String& pattern) const {
    return matchGlob(path, pattern);
}

double SuperImportResolver::GlobPathMatcher::getMatchScore(const String& path, const String& pattern) const {
    return matches(path, pattern) ? 1.0 : 0.0;
}

StringVector SuperImportResolver::GlobPathMatcher::getSuggestions(const String& path, const StringVector& candidates) const {
    StringVector suggestions;
    
    for (const String& candidate : candidates) {
        if (matches(candidate, path)) {
            suggestions.push_back(candidate);
        }
    }
    
    return suggestions;
}

bool SuperImportResolver::GlobPathMatcher::matchGlob(const String& path, const String& pattern) const {
    size_t pathIdx = 0, patternIdx = 0;
    size_t pathLen = path.length(), patternLen = pattern.length();
    size_t starIdx = SIZE_MAX, match = 0;
    
    while (pathIdx < pathLen) {
        if (patternIdx < patternLen && (pattern[patternIdx] == '?' || 
                                        pattern[patternIdx] == path[pathIdx])) {
            pathIdx++;
            patternIdx++;
        } else if (patternIdx < patternLen && pattern[patternIdx] == '*') {
            // 处理 ** 递归通配符
            if (patternIdx + 1 < patternLen && pattern[patternIdx + 1] == '*') {
                return matchRecursiveWildcard(path, pattern);
            }
            
            starIdx = patternIdx;
            match = pathIdx;
            patternIdx++;
        } else if (starIdx != SIZE_MAX) {
            patternIdx = starIdx + 1;
            match++;
            pathIdx = match;
        } else if (patternIdx < patternLen && pattern[patternIdx] == '[') {
            // 字符类匹配
            size_t classEnd = pattern.find(']', patternIdx);
            if (classEnd != String::npos) {
                String charClass = pattern.substr(patternIdx + 1, classEnd - patternIdx - 1);
                if (matchCharacterClass(path[pathIdx], charClass)) {
                    pathIdx++;
                    patternIdx = classEnd + 1;
                } else if (starIdx != SIZE_MAX) {
                    patternIdx = starIdx + 1;
                    match++;
                    pathIdx = match;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else if (patternIdx < patternLen && pattern[patternIdx] == '{') {
            // 大括号扩展
            return matchBraceExpansion(path, pattern);
        } else {
            return false;
        }
    }
    
    // 跳过模式末尾的 *
    while (patternIdx < patternLen && pattern[patternIdx] == '*') {
        patternIdx++;
    }
    
    return patternIdx == patternLen;
}

bool SuperImportResolver::GlobPathMatcher::matchRecursiveWildcard(const String& path, const String& pattern) const {
    // 简化的 ** 匹配实现
    size_t doubleStarPos = pattern.find("**");
    if (doubleStarPos == String::npos) {
        return matchGlob(path, pattern);
    }
    
    String prefix = pattern.substr(0, doubleStarPos);
    String suffix = pattern.substr(doubleStarPos + 2);
    
    // 检查前缀匹配
    if (!prefix.empty() && path.substr(0, prefix.length()) != prefix) {
        return false;
    }
    
    // 检查后缀匹配
    if (!suffix.empty()) {
        if (path.length() < suffix.length()) {
            return false;
        }
        String pathSuffix = path.substr(path.length() - suffix.length());
        return matchGlob(pathSuffix, suffix);
    }
    
    return true;
}

bool SuperImportResolver::GlobPathMatcher::matchCharacterClass(char c, const String& charClass) const {
    if (charClass.empty()) return false;
    
    bool negated = charClass[0] == '^' || charClass[0] == '!';
    size_t start = negated ? 1 : 0;
    
    bool matched = false;
    for (size_t i = start; i < charClass.length(); ++i) {
        if (i + 2 < charClass.length() && charClass[i + 1] == '-') {
            // 范围匹配 a-z
            if (c >= charClass[i] && c <= charClass[i + 2]) {
                matched = true;
                break;
            }
            i += 2;
        } else if (c == charClass[i]) {
            matched = true;
            break;
        }
    }
    
    return negated ? !matched : matched;
}

bool SuperImportResolver::GlobPathMatcher::matchBraceExpansion(const String& path, const String& pattern) const {
    // 简化的大括号扩展实现
    size_t braceStart = pattern.find('{');
    size_t braceEnd = pattern.find('}', braceStart);
    
    if (braceStart == String::npos || braceEnd == String::npos) {
        return matchGlob(path, pattern);
    }
    
    String prefix = pattern.substr(0, braceStart);
    String suffix = pattern.substr(braceEnd + 1);
    String options = pattern.substr(braceStart + 1, braceEnd - braceStart - 1);
    
    // 分割选项
    std::istringstream iss(options);
    String option;
    while (std::getline(iss, option, ',')) {
        String expandedPattern = prefix + option + suffix;
        if (matchGlob(path, expandedPattern)) {
            return true;
        }
    }
    
    return false;
}

// ========== SemanticVersionResolver 实现 ==========

StringVector SuperImportResolver::SemanticVersionResolver::getAvailableVersions(const String& moduleName) const {
    // 简化实现 - 在实际应用中应该从包管理器或文件系统中获取
    StringVector versions = {"1.0.0", "1.1.0", "1.2.0", "2.0.0", "2.1.0"};
    return versions;
}

SuperImportResolver::VersionResolver::VersionInfo 
SuperImportResolver::SemanticVersionResolver::resolveVersion(const String& moduleName, const String& versionSpec) const {
    VersionInfo info;
    
    auto availableVersions = getAvailableVersions(moduleName);
    
    // 解析版本规范
    if (versionSpec == "latest" || versionSpec.empty()) {
        return getLatestVersion(moduleName);
    }
    
    // 精确匹配
    for (const String& version : availableVersions) {
        if (version == versionSpec) {
            info.version = version;
            info.path = "/modules/" + moduleName + "/" + version;
            info.isStable = true;
            info.isLatest = (version == availableVersions.back());
            info.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            );
            return info;
        }
    }
    
    // 范围匹配（简化）
    if (versionSpec.find("^") == 0) {
        String baseVersion = versionSpec.substr(1);
        auto base = parseVersion(baseVersion);
        
        for (const String& version : availableVersions) {
            auto current = parseVersion(version);
            if (current.major == base.major && 
                compareVersions(current, base) >= 0) {
                info.version = version;
                info.path = "/modules/" + moduleName + "/" + version;
                info.isStable = true;
                info.isLatest = false;
                return info;
            }
        }
    }
    
    return info; // 空信息表示未找到
}

SuperImportResolver::VersionResolver::VersionInfo 
SuperImportResolver::SemanticVersionResolver::getLatestVersion(const String& moduleName) const {
    VersionInfo info;
    auto versions = getAvailableVersions(moduleName);
    
    if (!versions.empty()) {
        String latestVersion = versions.back();
        info.version = latestVersion;
        info.path = "/modules/" + moduleName + "/" + latestVersion;
        info.isStable = true;
        info.isLatest = true;
        info.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        );
    }
    
    return info;
}

bool SuperImportResolver::SemanticVersionResolver::isVersionCompatible(const String& required, const String& available) const {
    auto reqVersion = parseVersion(required);
    auto availVersion = parseVersion(available);
    
    // 简化的兼容性检查
    return reqVersion.major == availVersion.major && 
           compareVersions(availVersion, reqVersion) >= 0;
}

SuperImportResolver::SemanticVersionResolver::SemanticVersion 
SuperImportResolver::SemanticVersionResolver::parseVersion(const String& version) const {
    SemanticVersion sv = {0, 0, 0, "", ""};
    
    std::regex versionRegex(R"((\d+)\.(\d+)\.(\d+)(?:-([^+]+))?(?:\+(.+))?)");
    std::smatch match;
    
    if (std::regex_match(version, match, versionRegex)) {
        sv.major = std::stoi(match[1].str());
        sv.minor = std::stoi(match[2].str());
        sv.patch = std::stoi(match[3].str());
        sv.prerelease = match[4].str();
        sv.build = match[5].str();
    }
    
    return sv;
}

int SuperImportResolver::SemanticVersionResolver::compareVersions(const SemanticVersion& v1, const SemanticVersion& v2) const {
    if (v1.major != v2.major) return v1.major - v2.major;
    if (v1.minor != v2.minor) return v1.minor - v2.minor;
    if (v1.patch != v2.patch) return v1.patch - v2.patch;
    
    // 预发布版本比较
    if (v1.prerelease.empty() && !v2.prerelease.empty()) return 1;
    if (!v1.prerelease.empty() && v2.prerelease.empty()) return -1;
    if (!v1.prerelease.empty() && !v2.prerelease.empty()) {
        return v1.prerelease.compare(v2.prerelease);
    }
    
    return 0;
}

// ========== StandardConditionalProcessor 实现 ==========

bool SuperImportResolver::StandardConditionalProcessor::evaluateCondition(const String& condition, const StringUnorderedMap& context) const {
    String expr = substituteVariables(condition, context);
    return evaluateExpression(expr, context);
}

String SuperImportResolver::StandardConditionalProcessor::resolveConditionalImport(const ImportCondition& condition, 
                                                                                   const StringUnorderedMap& context) const {
    if (evaluateCondition(condition.condition, context)) {
        return substituteVariables(condition.truePath, context);
    } else if (!condition.falsePath.empty()) {
        return substituteVariables(condition.falsePath, context);
    } else {
        return substituteVariables(condition.defaultPath, context);
    }
}

bool SuperImportResolver::StandardConditionalProcessor::evaluateExpression(const String& expr, const StringUnorderedMap& context) const {
    // 简化的表达式求值
    if (expr == "true") return true;
    if (expr == "false") return false;
    
    // 变量检查
    auto it = context.find(expr);
    if (it != context.end()) {
        return it->second == "true" || it->second == "1";
    }
    
    // 比较操作
    std::regex comparisonRegex(R"((\w+)\s*(==|!=|>|<|>=|<=)\s*(.+))");
    std::smatch match;
    
    if (std::regex_match(expr, match, comparisonRegex)) {
        String left = match[1].str();
        String op = match[2].str();
        String right = match[3].str();
        
        auto leftIt = context.find(left);
        if (leftIt != context.end()) {
            String leftValue = leftIt->second;
            
            if (op == "==") return leftValue == right;
            if (op == "!=") return leftValue != right;
            // 其他比较操作的简化实现
        }
    }
    
    return false;
}

String SuperImportResolver::StandardConditionalProcessor::substituteVariables(const String& str, const StringUnorderedMap& context) const {
    String result = str;
    
    std::regex varRegex(R"(\$\{(\w+)\})");
    std::smatch match;
    
    while (std::regex_search(result, match, varRegex)) {
        String varName = match[1].str();
        String replacement = "";
        
        auto it = context.find(varName);
        if (it != context.end()) {
            replacement = it->second;
        }
        
        result.replace(match.position(), match.length(), replacement);
    }
    
    return result;
}

// ========== SuperImportResolver 主要实现 ==========

SuperImportResolver::SuperImportResolver(const ImportResolveOptions& options,
                                         const AdvancedResolveOptions& advancedOptions)
    : AdvancedImportResolver(options), advancedOptions(advancedOptions) {
    
    // 设置默认组件
    pathMatcher = std::make_unique<GlobPathMatcher>();
    versionResolver = std::make_unique<SemanticVersionResolver>();
    conditionalProcessor = std::make_unique<StandardConditionalProcessor>();
}

void SuperImportResolver::setPathMatcher(std::unique_ptr<PathMatcher> matcher) {
    pathMatcher = std::move(matcher);
}

void SuperImportResolver::setVersionResolver(std::unique_ptr<VersionResolver> resolver) {
    versionResolver = std::move(resolver);
}

void SuperImportResolver::setConditionalProcessor(std::unique_ptr<ConditionalImportProcessor> processor) {
    conditionalProcessor = std::move(processor);
}

std::vector<ResolvedPath> SuperImportResolver::resolveSuperWildcard(const String& pattern, const String& currentDirectory) const {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<ResolvedPath> results;
    
    // 使用配置的路径匹配器
    results = resolveWithMatcher(pattern, currentDirectory);
    
    // 应用过滤器
    std::vector<ResolvedPath> filtered;
    for (const auto& path : results) {
        if (!shouldExcludePath(path.resolvedPath)) {
            filtered.push_back(path);
        }
    }
    
    // 记录性能统计
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    recordResolution(pattern, duration);
    
    return filtered;
}

std::vector<ResolvedPath> SuperImportResolver::resolveWithFuzzyMatching(const String& pattern, double minScore) const {
    if (!advancedOptions.enableFuzzyMatching) {
        return {};
    }
    
    auto fuzzyMatcher = std::make_unique<FuzzyPathMatcher>();
    std::vector<ResolvedPath> results;
    
    // 获取候选路径（简化实现）
    StringVector candidates = {"src/main.chtl", "lib/utils.chtl", "components/Button.chtl"};
    
    for (const String& candidate : candidates) {
        double score = fuzzyMatcher->getMatchScore(candidate, pattern);
        if (score >= minScore) {
            ResolvedPath resolved;
            resolved.originalPath = pattern;
            resolved.resolvedPath = candidate;
            resolved.pathType = PathType::RELATIVE;
            resolved.importType = ImportTypeDetail::TEMPLATE_CHTL;
            resolved.exists = true;
            results.push_back(resolved);
        }
    }
    
    return results;
}

ResolvedPath SuperImportResolver::resolveVersionedImport(const String& moduleName, const String& versionSpec) const {
    if (!advancedOptions.enableVersionResolution || !versionResolver) {
        return {};
    }
    
    auto versionInfo = versionResolver->resolveVersion(moduleName, versionSpec);
    
    ResolvedPath resolved;
    if (!versionInfo.version.empty()) {
        resolved.originalPath = moduleName + "@" + versionSpec;
        resolved.resolvedPath = versionInfo.path;
        resolved.pathType = PathType::MODULE;
        resolved.importType = ImportTypeDetail::MODULE_CHTL;
        resolved.exists = true;
        
        // 添加版本信息到元数据
        resolved.metadata["version"] = versionInfo.version;
        resolved.metadata["isLatest"] = versionInfo.isLatest ? "true" : "false";
        resolved.metadata["isStable"] = versionInfo.isStable ? "true" : "false";
    }
    
    return resolved;
}

StringVector SuperImportResolver::getPathSuggestions(const String& partialPath, size_t maxSuggestions) const {
    if (!pathMatcher) {
        return {};
    }
    
    // 获取候选路径（在实际实现中应该从文件系统获取）
    StringVector candidates = {
        "src/components/Button.chtl",
        "src/components/Input.chtl", 
        "src/utils/helpers.chtl",
        "lib/framework.chtl",
        "config/settings.chtl"
    };
    
    auto suggestions = pathMatcher->getSuggestions(partialPath, candidates);
    
    if (suggestions.size() > maxSuggestions) {
        suggestions.resize(maxSuggestions);
    }
    
    return suggestions;
}

// 工具方法实现
StringVector SuperImportResolver::expandBraceExpansion(const String& pattern) {
    StringVector results;
    
    size_t braceStart = pattern.find('{');
    size_t braceEnd = pattern.find('}', braceStart);
    
    if (braceStart == String::npos || braceEnd == String::npos) {
        results.push_back(pattern);
        return results;
    }
    
    String prefix = pattern.substr(0, braceStart);
    String suffix = pattern.substr(braceEnd + 1);
    String options = pattern.substr(braceStart + 1, braceEnd - braceStart - 1);
    
    std::istringstream iss(options);
    String option;
    while (std::getline(iss, option, ',')) {
        results.push_back(prefix + option + suffix);
    }
    
    return results;
}

String SuperImportResolver::normalizePath(const String& path, bool resolveDots) {
    String normalized = path;
    
    // 标准化路径分隔符
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    
    if (resolveDots) {
        // 解析 . 和 .. 
        std::vector<String> parts;
        std::istringstream iss(normalized);
        String part;
        
        while (std::getline(iss, part, '/')) {
            if (part == "." || part.empty()) {
                continue;
            } else if (part == "..") {
                if (!parts.empty()) {
                    parts.pop_back();
                }
            } else {
                parts.push_back(part);
            }
        }
        
        normalized = "";
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i > 0) normalized += "/";
            normalized += parts[i];
        }
    }
    
    return normalized;
}

bool SuperImportResolver::isValidPath(const String& path) {
    if (path.empty()) return false;
    
    // 检查非法字符
    static const String invalidChars = "<>:\"|?*";
    for (char c : invalidChars) {
        if (path.find(c) != String::npos) {
            return false;
        }
    }
    
    return true;
}

// 内部方法实现
std::vector<ResolvedPath> SuperImportResolver::resolveWithMatcher(const String& pattern, const String& currentDirectory) const {
    std::vector<ResolvedPath> results;
    
    if (!pathMatcher) {
        return results;
    }
    
    // 获取候选文件（简化实现）
    StringVector candidates = {
        "src/main.chtl",
        "src/components/Button.chtl",
        "lib/utils.chtl",
        "config/app.json"
    };
    
    for (const String& candidate : candidates) {
        if (pathMatcher->matches(candidate, pattern)) {
            results.push_back(createResolvedPath(candidate, pattern));
        }
    }
    
    // 限制结果数量
    if (results.size() > advancedOptions.maxWildcardResults) {
        results.resize(advancedOptions.maxWildcardResults);
    }
    
    return results;
}

bool SuperImportResolver::shouldExcludePath(const String& path) const {
    // 检查排除模式
    if (matchesExcludePattern(path)) {
        return true;
    }
    
    // 检查包含模式
    if (!advancedOptions.includePatterns.empty() && !matchesIncludePattern(path)) {
        return true;
    }
    
    // 检查扩展名
    String ext = getFileExtension(path);
    if (!advancedOptions.forbiddenExtensions.empty()) {
        if (std::find(advancedOptions.forbiddenExtensions.begin(), 
                     advancedOptions.forbiddenExtensions.end(), ext) != 
            advancedOptions.forbiddenExtensions.end()) {
            return true;
        }
    }
    
    if (!advancedOptions.requiredExtensions.empty()) {
        if (std::find(advancedOptions.requiredExtensions.begin(), 
                     advancedOptions.requiredExtensions.end(), ext) == 
            advancedOptions.requiredExtensions.end()) {
            return true;
        }
    }
    
    return false;
}

bool SuperImportResolver::matchesIncludePattern(const String& path) const {
    for (const String& pattern : advancedOptions.includePatterns) {
        if (pathMatcher && pathMatcher->matches(path, pattern)) {
            return true;
        }
    }
    return advancedOptions.includePatterns.empty();
}

bool SuperImportResolver::matchesExcludePattern(const String& path) const {
    for (const String& pattern : advancedOptions.excludePatterns) {
        if (pathMatcher && pathMatcher->matches(path, pattern)) {
            return true;
        }
    }
    return false;
}

ResolvedPath SuperImportResolver::createResolvedPath(const String& path, const String& originalPattern) const {
    ResolvedPath resolved;
    resolved.originalPath = originalPattern.empty() ? path : originalPattern;
    resolved.resolvedPath = path;
    resolved.pathType = detectPathType(path);
    resolved.importType = detectImportType(path);
    resolved.exists = true; // 简化实现
    
    return resolved;
}

void SuperImportResolver::recordResolution(const String& pattern, std::chrono::milliseconds time) const {
    resolutionCounts[pattern]++;
    resolutionTimes[pattern] = time;
}

// ========== ImportResolverFactory 实现 ==========

std::unique_ptr<AdvancedImportResolver> ImportResolverFactory::createResolver(
    ResolverType type, const ImportResolveOptions& options) {
    
    switch (type) {
        case ResolverType::SUPER:
            return std::make_unique<SuperImportResolver>(options);
        case ResolverType::ADVANCED:
        case ResolverType::BASIC:
        default:
            return std::make_unique<AdvancedImportResolver>(options);
    }
}

std::unique_ptr<SuperImportResolver> ImportResolverFactory::createSuperResolver(
    const ImportResolveOptions& options,
    const SuperImportResolver::AdvancedResolveOptions& advancedOptions) {
    
    return std::make_unique<SuperImportResolver>(options, advancedOptions);
}

std::unique_ptr<SuperImportResolver> ImportResolverFactory::createWebResolver() {
    return std::make_unique<SuperImportResolver>(getWebOptions(), getWebAdvancedOptions());
}

std::unique_ptr<SuperImportResolver> ImportResolverFactory::createLibraryResolver() {
    return std::make_unique<SuperImportResolver>(getLibraryOptions(), getLibraryAdvancedOptions());
}

std::unique_ptr<SuperImportResolver> ImportResolverFactory::createSecureResolver() {
    return std::make_unique<SuperImportResolver>(getSecureOptions(), getSecureAdvancedOptions());
}

ImportResolveOptions ImportResolverFactory::getWebOptions() {
    ImportResolveOptions options;
    options.searchPaths = {"src", "node_modules", "public"};
    options.enableWildcards = true;
    options.followSymlinks = true;
    options.maxSearchDepth = 8;
    return options;
}

SuperImportResolver::AdvancedResolveOptions ImportResolverFactory::getWebAdvancedOptions() {
    SuperImportResolver::AdvancedResolveOptions options;
    options.enableFuzzyMatching = true;
    options.enableVersionResolution = true;
    options.requiredExtensions = {".chtl", ".js", ".ts", ".css", ".scss"};
    options.excludePatterns = {"node_modules/**", ".git/**", "**/*.test.*"};
    return options;
}

ImportResolveOptions ImportResolverFactory::getSecureOptions() {
    ImportResolveOptions options;
    options.searchPaths = {"src", "lib"};
    options.enableWildcards = false;
    options.followSymlinks = false;
    options.strictTypeChecking = true;
    options.allowMissingFiles = false;
    return options;
}

SuperImportResolver::AdvancedResolveOptions ImportResolverFactory::getSecureAdvancedOptions() {
    SuperImportResolver::AdvancedResolveOptions options;
    options.preventPathTraversal = true;
    options.validateFilePermissions = true;
    options.checkFileIntegrity = true;
    options.enableFuzzyMatching = false;
    options.trustedDirectories = {"src", "lib", "config"};
    return options;
}

ImportResolveOptions ImportResolverFactory::getLibraryOptions() {
    ImportResolveOptions options;
    options.searchPaths = {"src", "lib", "dist"};
    options.enableWildcards = true;
    options.cacheResults = true;
    return options;
}

SuperImportResolver::AdvancedResolveOptions ImportResolverFactory::getLibraryAdvancedOptions() {
    SuperImportResolver::AdvancedResolveOptions options;
    options.enableVersionResolution = true;
    options.enableConditionalImports = false;
    options.requiredExtensions = {".chtl", ".cmod"};
    return options;
}

SuperImportResolver::AdvancedResolveOptions ImportResolverFactory::getFrameworkAdvancedOptions() {
    SuperImportResolver::AdvancedResolveOptions options;
    options.enableFuzzyMatching = true;
    options.enableVersionResolution = true;
    options.enableConditionalImports = true;
    options.maxWildcardResults = 500;
    return options;
}

} // namespace CHTL