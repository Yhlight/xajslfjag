#include <iostream>
#include <string>
#include "../CHTL/CHTLContext/SelectorAutomation.h"
#include "../CHTL/CHTLContext/ConfigurationManager.h"

using namespace CHTL;

int main() {
    std::cout << "调试选择器类型检测" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    SelectorAutomation automation(configManager);
    
    std::string testSelector = ".box > div";
    
    std::cout << "测试选择器: '" << testSelector << "'" << std::endl;
    std::cout << "长度: " << testSelector.length() << std::endl;
    
    // 检查每个字符
    for (size_t i = 0; i < testSelector.length(); ++i) {
        char c = testSelector[i];
        std::cout << "位置 " << i << ": '" << c << "' (ASCII: " << (int)c << ")" << std::endl;
    }
    
    // 检查是否包含特定字符
    std::cout << "包含空格: " << (testSelector.find(' ') != std::string::npos) << std::endl;
    std::cout << "包含>: " << (testSelector.find('>') != std::string::npos) << std::endl;
    std::cout << "包含+: " << (testSelector.find('+') != std::string::npos) << std::endl;
    std::cout << "包含~: " << (testSelector.find('~') != std::string::npos) << std::endl;
    
    // 测试选择器类型
    auto type = automation.getSelectorType(testSelector);
    std::cout << "选择器类型: " << (int)type << std::endl;
    
    return 0;
}