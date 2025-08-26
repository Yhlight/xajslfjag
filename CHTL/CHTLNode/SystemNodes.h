#ifndef CHTL_SYSTEM_NODES_H
#define CHTL_SYSTEM_NODES_H

#include "Node.h"
#include "BasicNodes.h"

namespace CHTL {

// 原始嵌入节点
class OriginEmbedNode : public Node {
private:
    std::string embedType;      // @Html, @Style, @JavaScript, 或自定义
    std::string name;           // 可选的名称
    std::string content;        // 原始内容
    bool hasName;
    
public:
    OriginEmbedNode(const std::string& type, const Token& token);
    
    void setEmbedType(const std::string& type) { embedType = type; }
    std::string getEmbedType() const { return embedType; }
    
    void setName(const std::string& n) { name = n; hasName = true; }
    std::string getName() const { return name; }
    bool getHasName() const { return hasName; }
    
    void setContent(const std::string& c) { content = c; }
    std::string getContent() const { return content; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// 配置节点
class ConfigurationNode : public Node {
private:
    std::string name;  // 配置名称（可选）
    bool hasName;
    std::shared_ptr<BlockNode> body;
    
public:
    ConfigurationNode(const Token& token);
    
    void setName(const std::string& n) { name = n; hasName = true; }
    std::string getName() const { return name; }
    bool getHasName() const { return hasName; }
    
    void setBody(std::shared_ptr<BlockNode> b) { body = b; }
    std::shared_ptr<BlockNode> getBody() const { return body; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// 配置选项节点
class ConfigOptionNode : public Node {
private:
    std::string key;
    std::string value;
    
public:
    ConfigOptionNode(const std::string& key, const std::string& value, const Token& token);
    
    std::string getKey() const { return key; }
    std::string getValue() const { return value; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// Name组节点
class NameGroupNode : public Node {
private:
    std::unordered_map<std::string, std::vector<std::string>> nameOptions;
    
public:
    NameGroupNode(const Token& token);
    
    void addNameOption(const std::string& key, const std::vector<std::string>& values);
    std::unordered_map<std::string, std::vector<std::string>> getNameOptions() const { return nameOptions; }
    std::vector<std::string> getOption(const std::string& key) const;
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// OriginType组节点
class OriginTypeNode : public Node {
private:
    std::unordered_map<std::string, std::string> originTypes;
    
public:
    OriginTypeNode(const Token& token);
    
    void addOriginType(const std::string& key, const std::string& value);
    std::unordered_map<std::string, std::string> getOriginTypes() const { return originTypes; }
    std::string getOriginType(const std::string& key) const;
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// 导入语句节点
class ImportStatementNode : public Node {
public:
    enum ImportTargetType {
        FILE,           // 导入文件
        TEMPLATE,       // 导入模板
        CUSTOM,         // 导入自定义
        ORIGIN,         // 导入原始嵌入
        CONFIG,         // 导入配置
        MODULE          // 导入模块
    };
    
private:
    ImportTargetType targetType;
    std::string importType;      // @Html, @Style, @JavaScript, @Chtl, @CJmod等
    std::string itemName;        // 具体项名（可选）
    std::string fromPath;        // from路径
    std::string asName;          // as别名（可选）
    bool isWildcard;             // 是否为通配符导入
    bool hasFullPrefix;          // 是否使用全缀名
    
public:
    ImportStatementNode(const Token& token);
    
    void setTargetType(ImportTargetType type) { targetType = type; }
    ImportTargetType getTargetType() const { return targetType; }
    
    void setImportType(const std::string& type) { importType = type; }
    std::string getImportType() const { return importType; }
    
    void setItemName(const std::string& name) { itemName = name; }
    std::string getItemName() const { return itemName; }
    
    void setFromPath(const std::string& path) { fromPath = path; }
    std::string getFromPath() const { return fromPath; }
    
    void setAsName(const std::string& name) { asName = name; }
    std::string getAsName() const { return asName; }
    bool hasAs() const { return !asName.empty(); }
    
    void setIsWildcard(bool wildcard) { isWildcard = wildcard; }
    bool getIsWildcard() const { return isWildcard; }
    
    void setHasFullPrefix(bool full) { hasFullPrefix = full; }
    bool getHasFullPrefix() const { return hasFullPrefix; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// use语句节点
class UseStatementNode : public Node {
private:
    std::string target;  // "html5" 或 "@Config Basic" 等
    
public:
    UseStatementNode(const std::string& target, const Token& token);
    
    std::string getTarget() const { return target; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// 命名空间声明节点
class NamespaceDeclarationNode : public Node {
private:
    std::string name;
    std::shared_ptr<BlockNode> body;  // 可选的花括号内容
    bool hasBody;
    
public:
    NamespaceDeclarationNode(const std::string& name, const Token& token);
    
    std::string getName() const { return name; }
    
    void setBody(std::shared_ptr<BlockNode> b) { body = b; hasBody = true; }
    std::shared_ptr<BlockNode> getBody() const { return body; }
    bool getHasBody() const { return hasBody; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// except约束节点
class ExceptConstraintNode : public Node {
public:
    enum ConstraintType {
        PRECISE,    // 精确约束（具体元素/模板/自定义）
        TYPE,       // 类型约束（@Html, [Custom], [Template]）
        GLOBAL      // 全局约束（在命名空间中）
    };
    
private:
    ConstraintType constraintType;
    std::vector<std::string> targets;  // 约束目标
    
public:
    ExceptConstraintNode(ConstraintType type, const Token& token);
    
    ConstraintType getConstraintType() const { return constraintType; }
    
    void addTarget(const std::string& target) { targets.push_back(target); }
    std::vector<std::string> getTargets() const { return targets; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// from子句节点（用于命名空间引用）
class FromClauseNode : public Node {
private:
    std::string namespacePath;
    
public:
    FromClauseNode(const std::string& path, const Token& token);
    
    std::string getNamespacePath() const { return namespacePath; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

} // namespace CHTL

#endif // CHTL_SYSTEM_NODES_H