#include "src/CHTL/CHTLConfig/ConfigNode.h"
#include <iostream>

void testConfigSystem() {
    using namespace CHTL;
    
    std::cout << "Testing Configuration System:\n";
    std::cout << "=============================\n";
    
    // 创建基础配置
    auto config = std::make_unique<ConfigNode>();
    config->setConfigValue("INDEX_INITIAL_COUNT", "1");
    config->setConfigValue("DISABLE_NAME_GROUP", "false");
    config->setConfigValue("DEBUG_MODE", "true");
    
    // Name配置
    config->setNameConfig("CUSTOM_STYLE", "[@Style, @style, @CSS]");
    config->setNameConfig("TEMPLATE_ELEMENT", "@Element");
    
    // OriginType配置
    config->setOriginTypeConfig("ORIGINTYPE_VUE", "@Vue");
    config->setOriginTypeConfig("ORIGINTYPE_MARKDOWN", "@Markdown");
    
    std::cout << "Config: " << config->toString() << "\n";
    std::cout << "DEBUG_MODE: " << config->getConfigValue("DEBUG_MODE") << "\n";
    std::cout << "CUSTOM_STYLE: " << config->getNameConfig("CUSTOM_STYLE") << "\n";
    std::cout << "VUE type: " << config->getOriginTypeConfig("ORIGINTYPE_VUE") << "\n";
    
    // 创建命名配置
    auto namedConfig = std::make_unique<ConfigNode>("Development");
    namedConfig->setConfigValue("DEBUG_MODE", "true");
    namedConfig->setConfigValue("MINIFY_OUTPUT", "false");
    
    std::cout << "\nNamed config: " << namedConfig->toString() << "\n";
    std::cout << "Is named: " << namedConfig->isNamed << "\n";
    
    // 创建use语句
    auto useHtml5 = std::make_unique<UseNode>("html5");
    auto useConfig = std::make_unique<UseNode>("@Config", "Development");
    
    std::cout << "\nUse statements:\n";
    std::cout << useHtml5->toString() << "\n";
    std::cout << useConfig->toString() << "\n";
}

int main() {
    testConfigSystem();
    return 0;
}