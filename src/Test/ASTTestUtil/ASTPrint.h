#pragma once
#include "../../CHTL/CHTLNode/BaseNode.h"
#include <string>
#include <iostream>
#include <vector>
#include <memory>

namespace CHTL {
namespace Test {

/**
 * AST打印样式枚举
 */
enum class ASTPrintStyle {
    TREE,           // 树状结构
    INDENTED,       // 缩进结构
    XML,            // XML格式
    JSON,           // JSON格式
    DOT,            // Graphviz DOT格式
    YAML,           // YAML格式
    COMPACT,        // 紧凑格式
    DETAILED        // 详细格式
};

/**
 * AST打印工具类
 * 提供多种格式的AST可视化功能
 */
class ASTPrint {
public:
    /**
     * 构造函数
     */
    ASTPrint(ASTPrintStyle style = ASTPrintStyle::TREE);
    
    /**
     * 设置打印样式
     */
    void setStyle(ASTPrintStyle style);
    
    /**
     * 启用/禁用颜色
     */
    void setUseColors(bool useColors);
    
    /**
     * 设置最大深度
     */
    void setMaxDepth(int maxDepth);
    
    /**
     * 设置是否显示属性
     */
    void setShowAttributes(bool show);
    
    /**
     * 设置是否显示位置信息
     */
    void setShowPositions(bool show);
    
    /**
     * 设置是否显示类型信息
     */
    void setShowTypes(bool show);
    
    /**
     * 打印AST到控制台
     */
    void printAST(const NodePtr& root);
    
    /**
     * 生成AST字符串
     */
    std::string generateAST(const NodePtr& root);
    
    /**
     * 保存AST到文件
     */
    bool saveASTToFile(const NodePtr& root, const std::string& filename);
    
    /**
     * 打印AST节点信息
     */
    void printNodeInfo(const NodePtr& node);
    
    /**
     * 打印AST统计信息
     */
    void printASTStatistics(const NodePtr& root);
    
    /**
     * 按页打印AST（分页显示）
     */
    void printASTPaged(const NodePtr& root, size_t pageSize = 20);

private:
    ASTPrintStyle m_style;      // 打印样式
    bool m_useColors;           // 是否使用颜色
    int m_maxDepth;            // 最大深度
    bool m_showAttributes;      // 是否显示属性
    bool m_showPositions;       // 是否显示位置
    bool m_showTypes;          // 是否显示类型
    
    /**
     * 生成树状结构
     */
    std::string generateTreeStructure(const NodePtr& node, int depth = 0, const std::string& prefix = "") const;
    
    /**
     * 生成缩进结构
     */
    std::string generateIndentedStructure(const NodePtr& node, int depth = 0) const;
    
    /**
     * 生成XML格式
     */
    std::string generateXMLFormat(const NodePtr& node, int depth = 0) const;
    
    /**
     * 生成JSON格式
     */
    std::string generateJSONFormat(const NodePtr& node, int depth = 0) const;
    
    /**
     * 生成DOT格式
     */
    std::string generateDOTFormat(const NodePtr& root) const;
    
    /**
     * 生成YAML格式
     */
    std::string generateYAMLFormat(const NodePtr& node, int depth = 0) const;
    
    /**
     * 生成紧凑格式
     */
    std::string generateCompactFormat(const NodePtr& node) const;
    
    /**
     * 生成详细格式
     */
    std::string generateDetailedFormat(const NodePtr& node, int depth = 0) const;
    
    /**
     * 获取节点的颜色代码
     */
    std::string getNodeColor(CHTLNodeType type) const;
    
    /**
     * 重置颜色
     */
    std::string resetColor() const;
    
    /**
     * 获取缩进字符串
     */
    std::string getIndent(int depth) const;
    
    /**
     * 转义XML字符
     */
    std::string escapeXML(const std::string& text) const;
    
    /**
     * 转义JSON字符
     */
    std::string escapeJSON(const std::string& text) const;
    
    /**
     * 格式化节点名称
     */
    std::string formatNodeName(const NodePtr& node) const;
    
    /**
     * 获取节点属性字符串
     */
    std::string getNodeAttributesString(const NodePtr& node) const;
    
    /**
     * 获取节点位置字符串
     */
    std::string getNodePositionString(const NodePtr& node) const;
    
    /**
     * 计算AST统计信息
     */
    void calculateASTStatistics(const NodePtr& node, std::unordered_map<CHTLNodeType, size_t>& typeCounts, 
                               size_t& totalNodes, int& maxDepth, int currentDepth = 0) const;
};

/**
 * AST比较工具类
 */
class ASTComparator {
public:
    /**
     * 比较选项
     */
    struct CompareOptions {
        bool ignorePositions = true;        // 忽略位置信息
        bool ignoreAttributes = false;      // 忽略属性
        bool ignoreOrder = false;          // 忽略子节点顺序
        bool ignoreWhitespace = true;      // 忽略空白文本节点
        bool caseSensitive = true;         // 大小写敏感
    };
    
    /**
     * 比较两个AST
     */
    static bool compareAST(const NodePtr& tree1, const NodePtr& tree2, const CompareOptions& options = CompareOptions());
    
    /**
     * 查找AST差异
     */
    static std::vector<std::string> findASTDifferences(const NodePtr& tree1, const NodePtr& tree2, const CompareOptions& options = CompareOptions());
    
    /**
     * 打印AST差异
     */
    static void printASTDifferences(const NodePtr& tree1, const NodePtr& tree2, const CompareOptions& options = CompareOptions());
    
    /**
     * 生成差异报告
     */
    static std::string generateDifferenceReport(const NodePtr& tree1, const NodePtr& tree2, const CompareOptions& options = CompareOptions());

private:
    /**
     * 递归比较节点
     */
    static bool compareNodes(const NodePtr& node1, const NodePtr& node2, const CompareOptions& options, 
                           std::vector<std::string>& differences, const std::string& path = "");
    
    /**
     * 比较节点属性
     */
    static bool compareNodeAttributes(const NodePtr& node1, const NodePtr& node2, const CompareOptions& options,
                                    std::vector<std::string>& differences, const std::string& path);
    
    /**
     * 比较子节点
     */
    static bool compareChildren(const NodePtr& node1, const NodePtr& node2, const CompareOptions& options,
                              std::vector<std::string>& differences, const std::string& path);
};

/**
 * AST验证工具类
 */
class ASTValidator {
public:
    /**
     * 验证AST的完整性
     */
    static bool validateASTIntegrity(const NodePtr& root, std::vector<std::string>& errors);
    
    /**
     * 验证AST的语义正确性
     */
    static bool validateASTSemantics(const NodePtr& root, std::vector<std::string>& errors);
    
    /**
     * 验证节点关系
     */
    static bool validateNodeRelationships(const NodePtr& root, std::vector<std::string>& errors);
    
    /**
     * 检查循环引用
     */
    static bool checkCircularReferences(const NodePtr& root, std::vector<std::string>& errors);
    
    /**
     * 验证CHTL语法规则
     */
    static bool validateCHTLRules(const NodePtr& root, std::vector<std::string>& errors);

private:
    /**
     * 递归验证节点
     */
    static bool validateNodeRecursive(const NodePtr& node, std::vector<std::string>& errors, 
                                     std::unordered_set<const BaseNode*>& visited, const std::string& path = "");
};

} // namespace Test
} // namespace CHTL