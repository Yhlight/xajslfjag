#include <iostream>
#include <string>

#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/ConfigNode.h"
#include "../Error/ErrorReport.h"

using namespace CHTL;

void printConfigurationDetails(std::shared_ptr<ConfigurationNode> configNode) {
    if (!configNode) {
        std::cout << "  配置节点为空" << std::endl;
        return;
    }
    
    std::cout << "  配置名称: " << (configNode->isNamed() ? configNode->getConfigName() : "无名") << std::endl;
    std::cout << "  配置类型: " << static_cast<int>(configNode->getConfigType()) << std::endl;
    std::cout << "  子节点数量: " << configNode->getChildren().size() << std::endl;
    
    // 获取所有配置项
    auto allConfigs = configNode->getAllConfigs();
    std::cout << "  配置项数量: " << allConfigs.size() << std::endl;
    
    for (const auto& pair : allConfigs) {
        std::cout << "    " << pair.first << " = ";
        const auto& value = pair.second;
        switch (value.type) {
            case ConfigValueType::BOOLEAN:
                std::cout << (value.getBool() ? "true" : "false");
                break;
            case ConfigValueType::INTEGER:
                std::cout << value.getInt();
                break;
            case ConfigValueType::STRING:
            case ConfigValueType::IDENTIFIER:
                std::cout << "\"" << value.getString() << "\"";
                break;
            case ConfigValueType::ARRAY:
                std::cout << "[";
                const auto& arr = value.getArray();
                for (size_t i = 0; i < arr.size(); ++i) {
                    std::cout << arr[i];
                    if (i < arr.size() - 1) std::cout << ", ";
                }
                std::cout << "]";
                break;
        }
        std::cout << std::endl;
    }
    
    // 测试特定配置方法
    std::cout << "  索引起始计数: " << configNode->getIndexInitialCount() << std::endl;
    std::cout << "  调试模式: " << (configNode->isDebugMode() ? "是" : "否") << std::endl;
}

int main() {
    std::string configCode = R"(
[Configuration]
{
    INDEX_INITIAL_COUNT = 1;
    DEBUG_MODE = true;
    DISABLE_NAME_GROUP = false;
}
)";
    
    std::cout << "测试配置解析详情:" << std::endl;
    std::cout << "==================" << std::endl;
    
    try {
        ErrorReporter errorReporter;
        Lexer lexer(configCode);
        auto tokens = lexer.tokenize();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            errorReporter.printAll();
            return 1;
        }
        
        std::cout << "✓ 词法分析成功，生成 " << tokens.size() << " 个token" << std::endl;
        
        CHTLParser parser;
        auto ast = parser.parse(tokens);
        
        if (!ast.success) {
            std::cout << "❌ 语法分析失败" << std::endl;
            return 1;
        }
        
        std::cout << "✓ 语法分析成功" << std::endl;
        std::cout << "  根节点: " << (ast.rootNode ? "存在" : "不存在") << std::endl;
        std::cout << "  处理token数: " << ast.tokensProcessed << std::endl;
        std::cout << "  创建节点数: " << ast.nodesCreated << std::endl;
        
        if (ast.rootNode) {
            std::cout << "  根节点子节点数: " << ast.rootNode->getChildren().size() << std::endl;
        }
        
        std::cout << "  配置节点数: " << ast.configs.size() << std::endl;
        
        for (auto configPtr : ast.configs) {
            std::cout << "  配置节点类型: " << static_cast<int>(configPtr->getNodeType()) << std::endl;
            
            if (auto configNode = std::dynamic_pointer_cast<ConfigurationNode>(configPtr)) {
                std::cout << "✓ 找到配置节点!" << std::endl;
                printConfigurationDetails(configNode);
            }
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ 异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}