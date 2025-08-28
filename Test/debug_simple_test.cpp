#include <iostream>
#include <string>

int main() {
    std::string testSelector = "{{.container button}}";
    
    std::cout << "测试选择器: '" << testSelector << "'" << std::endl;
    std::cout << "长度: " << testSelector.length() << std::endl;
    
    // 移除{{}}包装
    std::string cleanSelector = testSelector;
    if (cleanSelector.length() >= 4 && 
        cleanSelector.substr(0, 2) == "{{" && 
        cleanSelector.substr(cleanSelector.length() - 2) == "}}") {
        cleanSelector = cleanSelector.substr(2, cleanSelector.length() - 4);
    }
    
    std::cout << "清理后: '" << cleanSelector << "'" << std::endl;
    std::cout << "清理后长度: " << cleanSelector.length() << std::endl;
    
    // 检查空格位置
    size_t spacePos = cleanSelector.find(' ');
    std::cout << "空格位置: " << spacePos << std::endl;
    
    if (spacePos != std::string::npos) {
        std::string parentSelector = cleanSelector.substr(0, spacePos);
        std::string childSelector = cleanSelector.substr(spacePos + 1);
        
        std::cout << "父选择器: '" << parentSelector << "'" << std::endl;
        std::cout << "子选择器: '" << childSelector << "'" << std::endl;
        
        // 检查每个字符
        std::cout << "父选择器字符: ";
        for (size_t i = 0; i < parentSelector.length(); ++i) {
            char c = parentSelector[i];
            std::cout << "'" << c << "' (ASCII: " << (int)c << ") ";
        }
        std::cout << std::endl;
        
        std::cout << "子选择器字符: ";
        for (size_t i = 0; i < childSelector.length(); ++i) {
            char c = childSelector[i];
            std::cout << "'" << c << "' (ASCII: " << (int)c << ") ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}