#pragma once
#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTL JS/CHTLJSNode/BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {
namespace Test {

/**
 * AST图形生成器
 * 生成AST的可视化图形表示
 */
class ASTGraph {
public:
    /**
     * 生成DOT格式的AST图
     */
    static std::string generateDOTGraph(const std::shared_ptr<CHTL::BaseNode>& root);
    static std::string generateDOTGraph(const std::shared_ptr<CHTLJS::BaseNode>& root);
    
    /**
     * 生成SVG格式的AST图
     */
    static std::string generateSVGGraph(const std::shared_ptr<CHTL::BaseNode>& root);
    static std::string generateSVGGraph(const std::shared_ptr<CHTLJS::BaseNode>& root);
    
    /**
     * 导出图形到文件
     */
    static bool exportGraphToFile(const std::shared_ptr<CHTL::BaseNode>& root, 
                                 const std::string& filename, 
                                 const std::string& format = "dot");
    static bool exportGraphToFile(const std::shared_ptr<CHTLJS::BaseNode>& root, 
                                 const std::string& filename, 
                                 const std::string& format = "dot");

private:
    static std::string generateNodeDOT(const std::shared_ptr<CHTL::BaseNode>& node, size_t& nodeId);
    static std::string generateNodeDOT(const std::shared_ptr<CHTLJS::BaseNode>& node, size_t& nodeId);
    static std::string escapeLabel(const std::string& label);
};

} // namespace Test
} // namespace CHTL