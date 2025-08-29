#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include "../CHTL/CHTLContext/Context.h"
#include "../CHTL/CHTLIOStream/CHTLFileSystem.h"
#include "../Error/ErrorReport.h"

void printUsage(const char* program) {
    std::cout << "CHTL Compiler v1.0.0\n";
    std::cout << "Usage: " << program << " <input-file> [output-file]\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help         Show this help\n";
    std::cout << "  -v, --version      Show version\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = "output.html";
    
    if (argc >= 3) {
        outputFile = argv[2];
    }
    
    // 检查帮助选项
    if (inputFile == "-h" || inputFile == "--help") {
        printUsage(argv[0]);
        return 0;
    }
    
    // 检查版本选项
    if (inputFile == "-v" || inputFile == "--version") {
        std::cout << "CHTL Compiler version 1.0.0\n";
        return 0;
    }
    
    try {
        // 读取输入文件
        auto content = CHTL::File::readToString(inputFile);
        if (!content) {
            std::cerr << "Error: Cannot read file: " << inputFile << std::endl;
            return 1;
        }
        
        // 创建编译上下文
        auto context = std::make_shared<CHTL::CompileContext>(inputFile);
        
        // 词法分析
        std::cout << "Lexing..." << std::endl;
        CHTL::Lexer lexer(*content, context);
        auto tokens = lexer.tokenizeAll();
        
        // 语法分析
        std::cout << "Parsing..." << std::endl;
        auto lexerPtr = std::make_shared<CHTL::Lexer>(*content, context);
        CHTL::Parser parser(lexerPtr, context);
        auto ast = parser.parse();
        
        if (!ast) {
            std::cerr << "Error: Parsing failed\n";
            return 1;
        }
        
        // 代码生成
        std::cout << "Generating..." << std::endl;
        CHTL::Generator generator(context);
        auto result = generator.generate(ast);
        
        // 写入输出文件
        if (!CHTL::File::writeString(outputFile, result)) {
            std::cerr << "Error: Cannot write file: " << outputFile << std::endl;
            return 1;
        }
        
        std::cout << "Successfully compiled to: " << outputFile << std::endl;
        
        // TODO: 实现错误统计和报告
        // 目前简单返回成功
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}