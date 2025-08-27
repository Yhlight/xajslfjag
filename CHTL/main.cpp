#include <iostream>
#include <string>
#include <vector>
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLLexer/GlobalMap.h"
#include "Util/FileSystem/FileSystem.h"

void printUsage(const std::string& programName) {
    std::cout << "CHTL Compiler v1.0.0\n";
    std::cout << "Usage: " << programName << " [options] <input-file>\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -o <file>    Output file name\n";
    std::cout << "  -m <dir>     Module search directory\n";
    std::cout << "  -d           Enable debug mode\n";
    std::cout << "  -h, --help   Show this help message\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << programName << " input.chtl\n";
    std::cout << "  " << programName << " -o output.html input.chtl\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string inputFile;
    std::string outputFile;
    std::string moduleDir;
    bool debugMode = false;
    
    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "-m" && i + 1 < argc) {
            moduleDir = argv[++i];
        } else if (arg == "-d") {
            debugMode = true;
        } else if (arg[0] != '-') {
            inputFile = arg;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            return 1;
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }
    
    // 检查输入文件是否存在
    if (!CHTL::Util::FileSystem::exists(inputFile)) {
        std::cerr << "Error: Input file '" << inputFile << "' does not exist\n";
        return 1;
    }
    
    // 读取输入文件
    std::string sourceCode = CHTL::Util::FileSystem::readFile(inputFile);
    if (sourceCode.empty()) {
        std::cerr << "Error: Failed to read input file or file is empty\n";
        return 1;
    }
    
    // 如果没有指定输出文件，使用默认名称
    if (outputFile.empty()) {
        outputFile = CHTL::Util::FileSystem::getFileNameWithoutExtension(inputFile) + ".html";
    }
    
    std::cout << "Compiling: " << inputFile << " -> " << outputFile << "\n";
    
    // 创建词法分析器
    CHTL::Lexer lexer(sourceCode);
    
    try {
        // 进行词法分析
        auto tokens = lexer.tokenize();
        
        if (debugMode) {
            std::cout << "\n=== Tokens ===\n";
            for (const auto& token : tokens) {
                if (token.type != CHTL::TokenType::WHITESPACE && 
                    token.type != CHTL::TokenType::NEWLINE) {
                    std::cout << "[" << token.getTypeString() << "] " 
                              << token.value << " (Line: " << token.line 
                              << ", Col: " << token.column << ")\n";
                }
            }
        }
        
        // TODO: 实现语法分析器
        // CHTL::Parser parser(tokens);
        // auto ast = parser.parse();
        
        // TODO: 实现代码生成器
        // CHTL::Generator generator;
        // std::string output = generator.generate(ast);
        
        // 临时输出
        std::string output = "<!DOCTYPE html>\n<html>\n<head>\n<title>CHTL Compiled</title>\n</head>\n<body>\n<h1>CHTL Compiler is under development</h1>\n</body>\n</html>";
        
        // 写入输出文件
        if (!CHTL::Util::FileSystem::writeFile(outputFile, output)) {
            std::cerr << "Error: Failed to write output file\n";
            return 1;
        }
        
        std::cout << "Compilation successful!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error during compilation: " << e.what() << "\n";
        std::cerr << lexer.getErrorContext() << "\n";
        return 1;
    }
    
    // 清理
    CHTL::GlobalMap::destroyInstance();
    
    return 0;
}