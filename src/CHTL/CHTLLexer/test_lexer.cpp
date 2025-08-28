#include "CHTLLexer.h"
#include <iostream>
#include <sstream>

int main() {
    try {
        // 测试字符串输入
        std::string testCode = R"(
// 这是一个测试文件
-- 这是生成器注释
[Template] @Style DefaultText
{
    color: "black";
    line-height: 1.6;
}

div
{
    id: box;
    class: welcome;
    
    text
    {
        HelloWorld
    }
    
    style
    {
        .box
        {
            width: 100px;
            height: 200px;
        }
        
        &:hover
        {
            background-color: red;
        }
    }
}
)";
        
        std::istringstream stream(testCode);
        CHTL::CHTLLexer lexer(stream);
        
        // 设置全局状态表
        auto globalMap = std::make_shared<CHTL::CHTLGlobalMap>();
        lexer.setGlobalMap(globalMap);
        
        std::cout << "开始词法分析..." << std::endl;
        
        // 获取所有Token
        auto tokens = lexer.getAllTokens();
        
        std::cout << "词法分析完成，共生成 " << tokens.size() << " 个Token:" << std::endl;
        std::cout << "==========================================" << std::endl;
        
        for (size_t i = 0; i < tokens.size(); ++i) {
            const auto& token = tokens[i];
            std::cout << "[" << i << "] " 
                      << CHTL::CHTLTokenUtils::tokenTypeToString(token.type)
                      << " '" << token.value << "' "
                      << "(" << token.line << ":" << token.column << ")"
                      << std::endl;
        }
        
        std::cout << "==========================================" << std::endl;
        
        // 打印全局状态表状态
        globalMap->printStatus();
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}