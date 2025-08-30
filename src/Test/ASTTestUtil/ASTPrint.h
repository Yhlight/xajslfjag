#ifndef AST_PRINT_H
#define AST_PRINT_H

#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include <vector>
#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTLJS/CHTLJSNode/BaseNode.h"

namespace CHTL {
namespace Test {

// AST打印器
class ASTPrinter {
public:
    enum class Format {
        TREE,        // 树形格式
        INDENTED,    // 缩进格式
        LISP,        // LISP格式
        JSON,        // JSON格式
        XML,         // XML格式
        DOT          // Graphviz DOT格式
    };
    
    ASTPrinter(Format format = Format::TREE) : format_(format) {}
    
    // 打印CHTL AST
    void print(const BaseNode* node);
    void print(const std::shared_ptr<BaseNode>& node) { print(node.get()); }
    
    // 打印CHTL JS AST
    void print(const CHTLJSBaseNode* node);
    void print(const std::shared_ptr<CHTLJSBaseNode>& node) { print(node.get()); }
    
    // 设置输出流
    void setOutputStream(std::ostream& os) { os_ = &os; }
    
    // 设置格式
    void setFormat(Format format) { format_ = format; }
    
    // 设置选项
    void setShowTypes(bool show) { showTypes_ = show; }
    void setShowValues(bool show) { showValues_ = show; }
    void setShowPositions(bool show) { showPositions_ = show; }
    void setMaxDepth(int depth) { maxDepth_ = depth; }
    void setColorOutput(bool enable) { colorOutput_ = enable; }
    
    // 生成输出字符串
    std::string toString(const BaseNode* node);
    std::string toString(const CHTLJSBaseNode* node);

private:
    Format format_;
    std::ostream* os_ = &std::cout;
    bool showTypes_ = true;
    bool showValues_ = true;
    bool showPositions_ = false;
    int maxDepth_ = -1;  // -1表示无限制
    bool colorOutput_ = true;
    
    // 不同格式的打印方法
    void printTree(const BaseNode* node, const std::string& prefix = "", bool isLast = true);
    void printIndented(const BaseNode* node, int indent = 0);
    void printLisp(const BaseNode* node);
    void printJSON(const BaseNode* node, int indent = 0);
    void printXML(const BaseNode* node, int indent = 0);
    void printDOT(const BaseNode* node);
    
    // 辅助方法
    std::string nodeTypeToString(NodeType type);
    std::string escapeString(const std::string& str);
    std::string colorize(const std::string& text, const std::string& color);
    std::string getColorForNodeType(NodeType type);
    void printDOTNode(const BaseNode* node, int& nodeId);
    void printDOTEdges(const BaseNode* node, int parentId, int& nodeId);
};

// AST比较器
class ASTComparator {
public:
    struct Difference {
        std::string path;       // 节点路径
        std::string field;      // 差异字段
        std::string expected;   // 期望值
        std::string actual;     // 实际值
    };
    
    // 比较两个AST
    static std::vector<Difference> compare(const BaseNode* expected, const BaseNode* actual);
    static std::vector<Difference> compare(const CHTLJSBaseNode* expected, const CHTLJSBaseNode* actual);
    
    // 检查AST是否相等
    static bool equals(const BaseNode* a, const BaseNode* b);
    static bool equals(const CHTLJSBaseNode* a, const CHTLJSBaseNode* b);
    
    // 打印差异
    static void printDifferences(const std::vector<Difference>& diffs);
    
private:
    static void compareNodes(const BaseNode* expected, const BaseNode* actual,
                           const std::string& path, std::vector<Difference>& diffs);
};

// AST遍历器
class ASTTraverser {
public:
    using NodeVisitor = std::function<bool(const BaseNode*, int depth)>;
    using CHTLJSNodeVisitor = std::function<bool(const CHTLJSBaseNode*, int depth)>;
    
    // 前序遍历
    static void preOrder(const BaseNode* root, NodeVisitor visitor);
    static void preOrder(const CHTLJSBaseNode* root, CHTLJSNodeVisitor visitor);
    
    // 后序遍历
    static void postOrder(const BaseNode* root, NodeVisitor visitor);
    
    // 层序遍历
    static void levelOrder(const BaseNode* root, NodeVisitor visitor);
    
    // 查找节点
    static std::vector<const BaseNode*> findByType(const BaseNode* root, NodeType type);
    static std::vector<const BaseNode*> findByValue(const BaseNode* root, const std::string& value);
    
    // 路径查找
    static const BaseNode* findByPath(const BaseNode* root, const std::string& path);
    
    // 获取节点路径
    static std::string getPath(const BaseNode* root, const BaseNode* target);

private:
    static void preOrderImpl(const BaseNode* node, NodeVisitor visitor, int depth);
    static void postOrderImpl(const BaseNode* node, NodeVisitor visitor, int depth);
};

// AST统计器
class ASTStatistics {
public:
    struct Stats {
        size_t totalNodes = 0;
        size_t maxDepth = 0;
        size_t maxWidth = 0;
        std::map<NodeType, size_t> nodeTypeCounts;
        std::map<std::string, size_t> nodeValueCounts;
        double averageFanout = 0.0;
    };
    
    // 计算AST统计信息
    static Stats calculate(const BaseNode* root);
    static Stats calculate(const CHTLJSBaseNode* root);
    
    // 打印统计信息
    static void printStats(const Stats& stats);
    
    // 获取AST复杂度
    static double getComplexity(const BaseNode* root);
    
private:
    static void calculateImpl(const BaseNode* node, Stats& stats, int depth);
};

// AST验证器
class ASTValidator {
public:
    struct ValidationError {
        std::string path;
        std::string message;
        std::string suggestion;
    };
    
    // 验证AST结构
    static std::vector<ValidationError> validate(const BaseNode* root);
    static std::vector<ValidationError> validate(const CHTLJSBaseNode* root);
    
    // 特定验证
    static bool validateStructure(const BaseNode* root);
    static bool validateSemantics(const BaseNode* root);
    static bool validateConstraints(const BaseNode* root);
    
    // 打印验证错误
    static void printErrors(const std::vector<ValidationError>& errors);
    
private:
    static void validateNode(const BaseNode* node, const std::string& path,
                           std::vector<ValidationError>& errors);
};

// AST转换器
class ASTTransformer {
public:
    using TransformFunc = std::function<std::shared_ptr<BaseNode>(const BaseNode*)>;
    
    // 转换AST
    static std::shared_ptr<BaseNode> transform(const BaseNode* root, TransformFunc func);
    
    // 预定义转换
    static std::shared_ptr<BaseNode> simplify(const BaseNode* root);
    static std::shared_ptr<BaseNode> normalize(const BaseNode* root);
    static std::shared_ptr<BaseNode> optimize(const BaseNode* root);
    
    // 节点替换
    static std::shared_ptr<BaseNode> replaceNode(const BaseNode* root,
                                                const BaseNode* target,
                                                const BaseNode* replacement);
    
    // 节点删除
    static std::shared_ptr<BaseNode> removeNode(const BaseNode* root,
                                               const BaseNode* target);
    
private:
    static std::shared_ptr<BaseNode> transformImpl(const BaseNode* node, TransformFunc func);
};

// AST可视化器
class ASTVisualizer {
public:
    // 生成可视化文件
    static void generateDOT(const BaseNode* root, const std::string& filename);
    static void generateSVG(const BaseNode* root, const std::string& filename);
    static void generateHTML(const BaseNode* root, const std::string& filename);
    
    // 交互式可视化
    static void startInteractive(const BaseNode* root);
    
    // 生成AST图表
    static void generateChart(const BaseNode* root, const std::string& filename);
    
private:
    static std::string generateDOTContent(const BaseNode* root);
    static std::string generateHTMLContent(const BaseNode* root);
};

} // namespace Test
} // namespace CHTL

#endif // AST_PRINT_H