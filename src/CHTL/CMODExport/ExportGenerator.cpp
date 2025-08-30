#include "ExportGenerator.h"
#include <algorithm>

namespace CHTL {

std::string ExportGenerator::generateExportBlock(const ProgramNode* program) {
    if (!program) return "";
    
    // 收集所有可导出的项
    ExportableItems items = collectExportableItems(program);
    
    // 生成格式化的Export块
    return formatExportBlock(items);
}

ExportGenerator::ExportableItems ExportGenerator::collectExportableItems(const ProgramNode* program) {
    ExportableItems items;
    
    // 遍历所有顶层节点
    for (const auto& node : program->getChildren()) {
        if (!node) continue;
        
        switch (node->getType()) {
            case NodeType::CUSTOM: {
                auto customNode = static_cast<CustomNode*>(node.get());
                const std::string& name = customNode->getName();
                
                switch (customNode->getCustomType()) {
                    case CustomType::STYLE:
                        items.customStyles.push_back(name);
                        break;
                    case CustomType::ELEMENT:
                        items.customElements.push_back(name);
                        break;
                    case CustomType::VAR:
                        items.customVars.push_back(name);
                        break;
                }
                break;
            }
            
            case NodeType::TEMPLATE: {
                auto templateNode = static_cast<TemplateNode*>(node.get());
                const std::string& name = templateNode->getName();
                
                switch (templateNode->getTemplateType()) {
                    case TemplateType::STYLE:
                        items.templateStyles.push_back(name);
                        break;
                    case TemplateType::ELEMENT:
                        items.templateElements.push_back(name);
                        break;
                    case TemplateType::VAR:
                        items.templateVars.push_back(name);
                        break;
                }
                break;
            }
            
            case NodeType::ORIGIN: {
                auto originNode = static_cast<OriginNode*>(node.get());
                if (originNode->getName().has_value()) {
                    const std::string& name = originNode->getName().value();
                    
                    switch (originNode->getOriginType()) {
                        case OriginType::HTML:
                            items.originHtml.push_back(name);
                            break;
                        case OriginType::STYLE:
                            items.originStyle.push_back(name);
                            break;
                        case OriginType::JAVASCRIPT:
                            items.originJavascript.push_back(name);
                            break;
                        case OriginType::CUSTOM:
                            items.originCustom.push_back(name);
                            break;
                    }
                }
                break;
            }
            
            case NodeType::CONFIGURATION: {
                auto configNode = static_cast<ConfigNode*>(node.get());
                if (!configNode->getName().empty()) {
                    items.configurations.push_back(configNode->getName());
                }
                break;
            }
            
            case NodeType::NAMESPACE: {
                auto namespaceNode = static_cast<NamespaceNode*>(node.get());
                collectFromNamespace(namespaceNode, items);
                break;
            }
        }
    }
    
    // 去重并排序
    auto removeDuplicates = [](std::vector<std::string>& vec) {
        std::sort(vec.begin(), vec.end());
        vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
    };
    
    removeDuplicates(items.customStyles);
    removeDuplicates(items.customElements);
    removeDuplicates(items.customVars);
    removeDuplicates(items.templateStyles);
    removeDuplicates(items.templateElements);
    removeDuplicates(items.templateVars);
    removeDuplicates(items.originHtml);
    removeDuplicates(items.originStyle);
    removeDuplicates(items.originJavascript);
    removeDuplicates(items.originCustom);
    removeDuplicates(items.configurations);
    
    return items;
}

void ExportGenerator::collectFromNamespace(const NamespaceNode* ns, ExportableItems& items) {
    if (!ns) return;
    
    // 递归收集命名空间内的项
    for (const auto& node : ns->getContent()) {
        if (!node) continue;
        
        // 处理命名空间内的节点（与顶层相同的逻辑）
        // 这里简化处理，实际应该保留命名空间路径
        switch (node->getType()) {
            case NodeType::CUSTOM:
            case NodeType::TEMPLATE:
            case NodeType::ORIGIN:
            case NodeType::CONFIGURATION:
                // 递归处理
                break;
            case NodeType::NAMESPACE:
                collectFromNamespace(static_cast<NamespaceNode*>(node.get()), items);
                break;
        }
    }
}

std::string ExportGenerator::formatExportBlock(const ExportableItems& items) {
    std::stringstream ss;
    
    ss << "[Export]\n{\n";
    
    // 输出[Custom]部分
    if (!items.customStyles.empty()) {
        ss << formatExportSection("    [Custom] @Style", items.customStyles);
    }
    if (!items.customElements.empty()) {
        ss << formatExportSection("    [Custom] @Element", items.customElements);
    }
    if (!items.customVars.empty()) {
        ss << formatExportSection("    [Custom] @Var", items.customVars);
    }
    
    // 输出[Template]部分
    if (!items.templateStyles.empty() || !items.templateElements.empty() || !items.templateVars.empty()) {
        ss << "\n";
        if (!items.templateStyles.empty()) {
            ss << "    [Template] @Style ...;\n";
        }
        if (!items.templateElements.empty()) {
            ss << "    [Template] @Element ...;\n";
        }
        if (!items.templateVars.empty()) {
            ss << "    [Template] @Var ...;\n";
        }
    }
    
    // 输出[Origin]部分
    if (!items.originHtml.empty() || !items.originStyle.empty() || 
        !items.originJavascript.empty() || !items.originCustom.empty()) {
        ss << "\n";
        if (!items.originHtml.empty()) {
            ss << "    [Origin] @Html ...;\n";
        }
        if (!items.originStyle.empty()) {
            ss << "    [Origin] @Style ...;\n";
        }
        if (!items.originJavascript.empty()) {
            ss << "    [Origin] @Javascript ...;\n";
        }
        if (!items.originCustom.empty()) {
            // 自定义类型
            for (const auto& customType : items.originCustom) {
                ss << "    [Origin] @" << customType << " ...;\n";
            }
        }
    }
    
    // 输出[Configuration]部分
    if (!items.configurations.empty()) {
        ss << "\n    [Configuration] @Config ...;\n";
    }
    
    ss << "}\n";
    
    return ss.str();
}

std::string ExportGenerator::formatExportSection(const std::string& prefix, 
                                                const std::vector<std::string>& items) {
    if (items.empty()) return "";
    
    std::stringstream ss;
    ss << prefix << " ";
    
    // 每行最多输出5个项，以保持可读性
    size_t count = 0;
    for (size_t i = 0; i < items.size(); ++i) {
        if (count > 0 && count % 5 == 0) {
            ss << ",\n" << std::string(prefix.length() + 1, ' ');
        } else if (i > 0) {
            ss << ", ";
        }
        ss << items[i];
        count++;
    }
    
    ss << ";\n";
    return ss.str();
}

} // namespace CHTL