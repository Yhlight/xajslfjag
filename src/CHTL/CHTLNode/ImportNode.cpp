#include "ImportNode.h"
#include "../../Util/StringUtils.h"
#include <algorithm>

namespace CHTL {

// ImportNode构造函数
ImportNode::ImportNode(NodeType type, const String& iType, const String& source, const Position& pos)
    : BaseNode(type, source, pos), importType(iType), sourcePath(source), 
      hasAlias(false), isWildcardImport(false), isResolved(false) {
}

// 虚析构函数
ImportNode::~ImportNode() = default;

// 导入项管理
void ImportNode::addImportItem(const String& item) {
    if (!item.empty() && std::find(importItems.begin(), importItems.end(), item) == importItems.end()) {
        importItems.push_back(item);
    }
}

void ImportNode::removeImportItem(const String& item) {
    auto it = std::find(importItems.begin(), importItems.end(), item);
    if (it != importItems.end()) {
        importItems.erase(it);
    }
}

bool ImportNode::hasImportItem(const String& item) const {
    return std::find(importItems.begin(), importItems.end(), item) != importItems.end();
}

// 排除项管理
void ImportNode::addExceptItem(const String& item) {
    if (!item.empty() && std::find(exceptItems.begin(), exceptItems.end(), item) == exceptItems.end()) {
        exceptItems.push_back(item);
    }
}

void ImportNode::removeExceptItem(const String& item) {
    auto it = std::find(exceptItems.begin(), exceptItems.end(), item);
    if (it != exceptItems.end()) {
        exceptItems.erase(it);
    }
}

bool ImportNode::hasExceptItem(const String& item) const {
    return std::find(exceptItems.begin(), exceptItems.end(), item) != exceptItems.end();
}

// 类型检查
bool ImportNode::isFileImport() const {
    return importType == "file" || importType.empty();
}

bool ImportNode::isModuleImport() const {
    return importType == "module" || importType == "cmod" || importType == "cjmod";
}

bool ImportNode::isTemplateImport() const {
    return importType == "template";
}

bool ImportNode::isCustomImport() const {
    return importType == "custom";
}

bool ImportNode::isOriginImport() const {
    return importType == "origin" || getType() == NodeType::IMPORT_ORIGIN;
}

bool ImportNode::isConfigImport() const {
    return importType == "config" || getType() == NodeType::IMPORT_CONFIG;
}

// 验证
bool ImportNode::validateImport() const {
    // 基本验证：路径不能为空
    if (sourcePath.empty()) {
        return false;
    }
    
    // 检查路径格式
    if (Util::StringUtils::starts_with(sourcePath, "./") || 
        Util::StringUtils::starts_with(sourcePath, "../") ||
        sourcePath.find("://") != String::npos) {
        // 相对路径或绝对路径都是有效的
        return true;
    }
    
    // 模块导入验证
    if (isModuleImport()) {
        // 模块名称应该是有效的标识符
        return !sourcePath.empty() && std::isalpha(sourcePath[0]);
    }
    
    return true;
}

StringVector ImportNode::getImportValidationErrors() const {
    StringVector errors;
    
    if (sourcePath.empty()) {
        errors.push_back("Import path cannot be empty");
    }
    
    if (isModuleImport() && !sourcePath.empty() && !std::isalpha(sourcePath[0])) {
        errors.push_back("Module name must start with a letter: " + sourcePath);
    }
    
    // 检查循环依赖等更复杂的验证可以在这里添加
    
    return errors;
}

// 重写BaseNode的虚函数
std::unique_ptr<BaseNode> ImportNode::clone() const {
    auto cloned = std::make_unique<ImportNode>(getType(), importType, sourcePath, position);
    
    // 复制所有属性
    cloned->importType = importType;
    cloned->importItems = importItems;
    cloned->exceptItems = exceptItems;
    cloned->isWildcardImport = isWildcardImport;
    cloned->resolvedPath = resolvedPath;
    cloned->isResolved = isResolved;
    cloned->importMetadata = importMetadata;
    cloned->hasAlias = hasAlias;
    
    // 复制子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// 重写BaseNode的虚函数
bool ImportNode::validate() const {
    return validateImport();
}

StringVector ImportNode::getValidationErrors() const {
    return getImportValidationErrors();
}

String ImportNode::generateImportStatement() const {
    String statement = "import ";
    
    if (!importItems.empty()) {
        statement += "{";
        for (size_t i = 0; i < importItems.size(); ++i) {
            if (i > 0) statement += ", ";
            statement += importItems[i];
        }
        statement += "} from ";
    }
    
    statement += "\"" + sourcePath + "\"";
    
    if (hasAlias) {
        statement += " as " + aliasName;
    }
    
    if (!exceptItems.empty()) {
        statement += " except {";
        for (size_t i = 0; i < exceptItems.size(); ++i) {
            if (i > 0) statement += ", ";
            statement += exceptItems[i];
        }
        statement += "}";
    }
    
    statement += ";";
    return statement;
}

String ImportNode::toString() const {
    String result = "ImportNode{";
    result += "type=" + std::to_string(static_cast<int>(getType()));
    result += ", path=" + sourcePath;
    if (hasAlias) {
        result += ", alias=" + aliasName;
    }
    if (!importType.empty()) {
        result += ", importType=" + importType;
    }
    if (isWildcardImport) {
        result += ", wildcard=true";
    }
    if (!importItems.empty()) {
        result += ", items=" + std::to_string(importItems.size());
    }
    if (!exceptItems.empty()) {
        result += ", except=" + std::to_string(exceptItems.size());
    }
    result += "}";
    return result;
}

} // namespace CHTL