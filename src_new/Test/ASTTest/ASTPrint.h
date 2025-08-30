#pragma once
#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTL JS/CHTLJSNode/BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {
namespace Test {

/**
 * AST打印工具
 * 提供AST树的格式化输出
 */
class ASTPrint {
public:
    /**
     * 打印CHTL AST树
     */
    static void printCHTLAST(const std::shared_ptr<CHTL::BaseNode>& root);
    
    /**
     * 打印CHTL JS AST树
     */
    static void printCHTLJSAST(const std::shared_ptr<CHTLJS::BaseNode>& root);
    
    /**
     * 生成AST文本表示
     */
    static std::string generateASTText(const std::shared_ptr<CHTL::BaseNode>& root);
    static std::string generateASTText(const std::shared_ptr<CHTLJS::BaseNode>& root);
    
    /**
     * 导出AST到文件
     */
    static bool exportASTToFile(const std::shared_ptr<CHTL::BaseNode>& root, const std::string& filename);
    static bool exportASTToFile(const std::shared_ptr<CHTLJS::BaseNode>& root, const std::string& filename);

private:
    static void printNodeRecursive(const std::shared_ptr<CHTL::BaseNode>& node, size_t depth);
    static void printNodeRecursive(const std::shared_ptr<CHTLJS::BaseNode>& node, size_t depth);
    static std::string getIndentation(size_t depth);
};

} // namespace Test
} // namespace CHTL