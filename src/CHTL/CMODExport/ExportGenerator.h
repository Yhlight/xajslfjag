#ifndef CHTL_EXPORT_GENERATOR_H
#define CHTL_EXPORT_GENERATOR_H

#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include "../CHTLNode/ProgramNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ConfigNode.h"
#include "../CHTLNode/NamespaceNode.h"

namespace CHTL {

// Export块生成器 - 自动生成CMOD的[Export]块
class ExportGenerator {
public:
    // 从AST生成Export块内容
    static std::string generateExportBlock(const ProgramNode* program);
    
private:
    // 收集所有可导出的元素
    struct ExportableItems {
        std::vector<std::string> customStyles;
        std::vector<std::string> customElements;
        std::vector<std::string> customVars;
        std::vector<std::string> templateStyles;
        std::vector<std::string> templateElements;
        std::vector<std::string> templateVars;
        std::vector<std::string> originHtml;
        std::vector<std::string> originStyle;
        std::vector<std::string> originJavascript;
        std::vector<std::string> originCustom;  // 自定义类型如@Vue
        std::vector<std::string> configurations;
    };
    
    // 遍历AST收集可导出项
    static ExportableItems collectExportableItems(const ProgramNode* program);
    
    // 递归收集命名空间内的项
    static void collectFromNamespace(const NamespaceNode* ns, ExportableItems& items);
    
    // 格式化输出
    static std::string formatExportBlock(const ExportableItems& items);
    
    // 格式化单个类型的导出项
    static std::string formatExportSection(const std::string& prefix, 
                                          const std::vector<std::string>& items);
};

} // namespace CHTL

#endif // CHTL_EXPORT_GENERATOR_H