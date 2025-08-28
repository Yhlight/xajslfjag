/**
 * CHTLLexer测试文件
 * 
 * 用于测试词法分析器的基本功能
 * 
 * @author CHTL Team
 * @version 1.0.0
 * @license MIT
 */

#include "CHTLLexer.h"
#include <iostream>
#include <string>

using namespace CHTL;

int main() {
    std::cout << "=== CHTLLexer测试程序 ===" << std::endl;
    
    // 测试源代码
    std::string testSource = R"(
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
        这是一段文本
    }
    
    style
    {
        .box
        {
            width: 300px;
            height: 200px;
            background-color: red;
        }
        
        &:hover
        {
            background-color: blue;
        }
    }
}

/* 多行注释
   测试多行注释功能
*/

[Custom] @Element Box
{
    div
    {
        style
        {
            width: 200px;
            height: 200px;
        }
    }
}

[Import] @Chtl from "./module.chtl";
use html5;
)";
    
    try {
        // 创建词法分析器
        CHTLLexer lexer(testSource);
        
        std::cout << "源代码长度: " << testSource.length() << " 字符" << std::endl;
        std::cout << "开始词法分析..." << std::endl;
        
        // 执行词法分析
        auto tokens = lexer.tokenize();
        
        std::cout << "\n词法分析完成！" << std::endl;
        std::cout << "生成Token数量: " << tokens.size() << std::endl;
        
        // 打印所有Token
        lexer.printTokens();
        
        // 检查是否有错误
        if (lexer.hasErrors()) {
            std::cout << "\n发现错误，请检查源代码！" << std::endl;
        } else {
            std::cout << "\n词法分析成功，无错误！" << std::endl;
        }
        
        // 测试Token流访问
        std::cout << "\n=== Token流访问测试 ===" << std::endl;
        lexer.reset();
        
        std::shared_ptr<Token> token;
        int tokenCount = 0;
        while ((token = lexer.getNextToken()) && tokenCount < 20) {
            std::cout << "Token[" << tokenCount << "]: " << token->toString() << std::endl;
            tokenCount++;
        }
        
        // 测试GlobalMap功能
        std::cout << "\n=== GlobalMap测试 ===" << std::endl;
        auto& globalMap = GlobalMap::getInstance();
        globalMap.printAllMappings();
        
        // 测试关键字识别
        std::cout << "\n=== 关键字识别测试 ===" << std::endl;
        std::vector<std::string> testKeywords = {
            "template", "custom", "origin", "import", "configuration", 
            "namespace", "text", "style", "script", "inherit", "delete"
        };
        
        for (const auto& keyword : testKeywords) {
            if (globalMap.isKeyword(keyword)) {
                auto type = globalMap.getKeywordType(keyword);
                std::cout << "关键字 '" << keyword << "' -> " << static_cast<int>(type) << std::endl;
            } else {
                std::cout << "非关键字: '" << keyword << "'" << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "测试过程中发生未知异常" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== 测试完成 ===" << std::endl;
    return 0;
}