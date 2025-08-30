#pragma once

#include "../../Util/Common.h"
#include "../../CHTL/CHTLNode/BaseNode.h"
#include <iostream>
#include <memory>
#include <vector>
#include <sstream>

namespace CHTL::Test {

// AST打印器 - 用于调试和测试AST结构
class ASTPrint {
public:
    // 打印选项
    struct PrintOptions {
        bool showNodeTypes = true;
        bool showAttributes = true;
        bool showPositions = false;
        bool showMemoryAddresses = false;
        bool useColors = false;
        bool compactFormat = false;
        int maxDepth = -1; // -1表示无限制
        String indentation = "  ";
        String nodePrefix = "├─ ";
        String lastNodePrefix = "└─ ";
        String verticalLine = "│  ";
        String emptySpace = "   ";
        
        PrintOptions() = default;
    };
    
    // 基本打印方法
    static void printAST(const std::unique_ptr<CHTL::BaseNode>& root, 
                        std::ostream& os = std::cout);
    
    static void printASTWithOptions(const std::unique_ptr<CHTL::BaseNode>& root,
                                   const PrintOptions& options,
                                   std::ostream& os = std::cout);
    
    static String astToString(const std::unique_ptr<CHTL::BaseNode>& root);
    static String astToStringWithOptions(const std::unique_ptr<CHTL::BaseNode>& root,
                                        const PrintOptions& options);
    
    // 特殊格式输出
    static void printASTAsTree(const std::unique_ptr<CHTL::BaseNode>& root,
                              std::ostream& os = std::cout);
    
    static void printASTAsJSON(const std::unique_ptr<CHTL::BaseNode>& root,
                              std::ostream& os = std::cout);
    
    static void printASTAsXML(const std::unique_ptr<CHTL::BaseNode>& root,
                             std::ostream& os = std::cout);
    
    static void printASTAsGraphviz(const std::unique_ptr<CHTL::BaseNode>& root,
                                  std::ostream& os = std::cout);
    
    // 统计信息
    static void printASTStatistics(const std::unique_ptr<CHTL::BaseNode>& root,
                                  std::ostream& os = std::cout);
    
    // 比较两个AST
    static void printASTComparison(const std::unique_ptr<CHTL::BaseNode>& expected,
                                  const std::unique_ptr<CHTL::BaseNode>& actual,
                                  std::ostream& os = std::cout);
    
    // 查找和过滤
    static void printNodesOfType(const std::unique_ptr<CHTL::BaseNode>& root,
                                CHTL::NodeType nodeType,
                                std::ostream& os = std::cout);
    
    static void printNodePath(const std::unique_ptr<CHTL::BaseNode>& root,
                             const CHTL::BaseNode* targetNode,
                             std::ostream& os = std::cout);
    
    // 验证和检查
    static void printASTValidation(const std::unique_ptr<CHTL::BaseNode>& root,
                                  std::ostream& os = std::cout);
    
    // 工具方法
    static String getNodeTypeName(CHTL::NodeType type);
    static String getNodeDescription(const CHTL::BaseNode* node);
    static String formatNodeAttributes(const CHTL::BaseNode* node);
    static String formatPosition(const CHTL::Position& pos);
    
    // 颜色定义
    struct Colors {
        static const String RESET;
        static const String NODE_TYPE;
        static const String ATTRIBUTE;
        static const String VALUE;
        static const String POSITION;
        static const String ERROR;
        static const String WARNING;
        static const String SUCCESS;
    };
    
    static String colorize(const String& text, const String& color, bool useColor = true);

private:
    static void printNodeRecursive(const CHTL::BaseNode* node,
                                  const String& prefix,
                                  bool isLast,
                                  const PrintOptions& options,
                                  std::ostream& os,
                                  int currentDepth = 0);
    
    static void printJSONNode(const CHTL::BaseNode* node,
                             std::ostream& os,
                             int indentLevel = 0);
    
    static void printXMLNode(const CHTL::BaseNode* node,
                            std::ostream& os,
                            int indentLevel = 0);
    
    static void printGraphvizNode(const CHTL::BaseNode* node,
                                 std::ostream& os,
                                 int& nodeId);
    
    static String escapeJSON(const String& text);
    static String escapeXML(const String& text);
    static String escapeGraphviz(const String& text);
    
    static std::map<CHTL::NodeType, int> collectNodeStatistics(const CHTL::BaseNode* node);
    static int calculateTreeDepth(const CHTL::BaseNode* node);
    static int countNodes(const CHTL::BaseNode* node);
};

// AST比较器
class ASTComparator {
public:
    struct Difference {
        enum Type {
            NODE_TYPE_MISMATCH,
            ATTRIBUTE_MISMATCH,
            STRUCTURE_MISMATCH,
            MISSING_NODE,
            EXTRA_NODE
        };
        
        Type type;
        String path;
        String description;
        String expectedValue;
        String actualValue;
        
        Difference(Type t, const String& p, const String& desc)
            : type(t), path(p), description(desc) {}
    };
    
    static std::vector<Difference> compareASTs(const std::unique_ptr<CHTL::BaseNode>& expected,
                                              const std::unique_ptr<CHTL::BaseNode>& actual);
    
    static bool areASTsEqual(const std::unique_ptr<CHTL::BaseNode>& ast1,
                            const std::unique_ptr<CHTL::BaseNode>& ast2,
                            bool ignorePositions = true);
    
    static void printDifferences(const std::vector<Difference>& differences,
                                std::ostream& os = std::cout);
    
    static String differencesToString(const std::vector<Difference>& differences);

private:
    static void compareNodesRecursive(const CHTL::BaseNode* expected,
                                     const CHTL::BaseNode* actual,
                                     const String& path,
                                     std::vector<Difference>& differences);
    
    static bool compareNodeAttributes(const CHTL::BaseNode* node1,
                                     const CHTL::BaseNode* node2);
};

// AST验证器
class ASTValidator {
public:
    struct ValidationRule {
        String name;
        std::function<bool(const CHTL::BaseNode*)> predicate;
        String errorMessage;
        
        ValidationRule(const String& n,
                      std::function<bool(const CHTL::BaseNode*)> pred,
                      const String& msg)
            : name(n), predicate(pred), errorMessage(msg) {}
    };
    
    struct ValidationResult {
        bool isValid;
        std::vector<String> errors;
        std::vector<String> warnings;
        std::vector<const CHTL::BaseNode*> errorNodes;
        
        ValidationResult() : isValid(true) {}
    };
    
    // 添加验证规则
    void addRule(const ValidationRule& rule);
    void addStructuralRule(const String& name,
                          std::function<bool(const std::unique_ptr<CHTL::BaseNode>&)> predicate,
                          const String& errorMessage);
    
    // 执行验证
    ValidationResult validate(const std::unique_ptr<CHTL::BaseNode>& ast);
    
    // 预定义规则
    static ValidationRule createNodeTypeValidationRule();
    static ValidationRule createParentChildValidationRule();
    static ValidationRule createAttributeValidationRule();
    static ValidationRule createCircularReferenceRule();
    
    void addCommonRules();

private:
    std::vector<ValidationRule> nodeRules_;
    std::vector<std::pair<String, std::function<bool(const std::unique_ptr<CHTL::BaseNode>&)>>> structuralRules_;
    std::vector<String> structuralErrorMessages_;
    
    void validateNodeRecursive(const CHTL::BaseNode* node,
                              ValidationResult& result);
    
    bool checkCircularReferences(const CHTL::BaseNode* node,
                               std::set<const CHTL::BaseNode*>& visited);
};

// AST遍历器
class ASTTraverser {
public:
    // 遍历回调
    using NodeCallback = std::function<void(const CHTL::BaseNode*, int depth)>;
    using NodePredicate = std::function<bool(const CHTL::BaseNode*)>;
    
    // 遍历方法
    static void traversePreOrder(const std::unique_ptr<CHTL::BaseNode>& root,
                                NodeCallback callback);
    
    static void traversePostOrder(const std::unique_ptr<CHTL::BaseNode>& root,
                                 NodeCallback callback);
    
    static void traverseBreadthFirst(const std::unique_ptr<CHTL::BaseNode>& root,
                                    NodeCallback callback);
    
    // 查找方法
    static std::vector<const CHTL::BaseNode*> findNodes(const std::unique_ptr<CHTL::BaseNode>& root,
                                                        NodePredicate predicate);
    
    static const CHTL::BaseNode* findFirstNode(const std::unique_ptr<CHTL::BaseNode>& root,
                                              NodePredicate predicate);
    
    static std::vector<const CHTL::BaseNode*> findNodesByType(const std::unique_ptr<CHTL::BaseNode>& root,
                                                              CHTL::NodeType nodeType);
    
    // 路径查找
    static std::vector<const CHTL::BaseNode*> getNodePath(const std::unique_ptr<CHTL::BaseNode>& root,
                                                          const CHTL::BaseNode* targetNode);
    
    static String getNodePathString(const std::unique_ptr<CHTL::BaseNode>& root,
                                   const CHTL::BaseNode* targetNode);

private:
    static void traversePreOrderRecursive(const CHTL::BaseNode* node,
                                         NodeCallback callback,
                                         int depth);
    
    static void traversePostOrderRecursive(const CHTL::BaseNode* node,
                                          NodeCallback callback,
                                          int depth);
    
    static bool findNodePathRecursive(const CHTL::BaseNode* current,
                                     const CHTL::BaseNode* target,
                                     std::vector<const CHTL::BaseNode*>& path);
};

} // namespace CHTL::Test