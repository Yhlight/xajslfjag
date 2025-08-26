#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "scanner/CHTLUnifiedScanner.h"
#include "dispatcher/CompilerDispatcher.h"

void printUsage(const char* program) {
    std::cout << "CHTL Compiler v1.0.0\n\n";
    std::cout << "Usage: " << program << " [options] <input-file>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o <file>    Output file (default: output.html)\n";
    std::cout << "  -m <path>    Module path (default: ./modules)\n";
    std::cout << "  --debug      Enable debug mode\n";
    std::cout << "  --strict     Enable strict mode\n";
    std::cout << "  --help       Show this help message\n";
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    // 解析命令行参数
    std::string inputFile;
    std::string outputFile = "output.html";
    std::string modulePath = "./modules";
    bool debugMode = false;
    bool strictMode = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "-m" && i + 1 < argc) {
            modulePath = argv[++i];
        } else if (arg == "--debug") {
            debugMode = true;
        } else if (arg == "--strict") {
            strictMode = true;
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
    std::ifstream testFile(inputFile);
    if (!testFile.is_open()) {
        std::cerr << "Error: Cannot open input file: " << inputFile << "\n";
        return 1;
    }
    testFile.close();
    
    try {
        // 创建编译器调度器
        chtl::DispatcherConfig config;
        config.strictMode = strictMode;
        config.enableParallelCompilation = false; // 暂时禁用并行编译
        
        chtl::CompilerDispatcher dispatcher(config);
        
        if (debugMode) {
            std::cout << "Debug mode enabled\n";
            std::cout << "Input file: " << inputFile << "\n";
            std::cout << "Output file: " << outputFile << "\n";
            std::cout << "Module path: " << modulePath << "\n";
        }
        
        // 编译文件
        auto result = dispatcher.dispatchFile(inputFile);
        
        if (!result.success) {
            std::cerr << "Compilation failed:\n";
            for (const auto& error : result.errors) {
                std::cerr << "  Error: " << error << "\n";
            }
            return 1;
        }
        
        // 显示警告
        if (!result.warnings.empty()) {
            std::cout << "Warnings:\n";
            for (const auto& warning : result.warnings) {
                std::cout << "  Warning: " << warning << "\n";
            }
        }
        
        // 生成输出文件
        std::ofstream output(outputFile);
        if (!output.is_open()) {
            std::cerr << "Error: Cannot create output file: " << outputFile << "\n";
            return 1;
        }
        
        // 构建完整的HTML
        output << "<!DOCTYPE html>\n";
        output << "<html>\n";
        output << "<head>\n";
        output << "    <meta charset=\"UTF-8\">\n";
        output << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        
        // 输出CSS
        if (!result.css.empty()) {
            output << "    <style>\n";
            output << result.css;
            output << "    </style>\n";
        }
        
        output << "</head>\n";
        output << "<body>\n";
        
        // 输出HTML内容
        output << result.html;
        
        // 输出JavaScript
        if (!result.javascript.empty()) {
            output << "    <script>\n";
            output << result.javascript;
            output << "    </script>\n";
        }
        
        output << "</body>\n";
        output << "</html>\n";
        
        output.close();
        
        std::cout << "Compilation successful. Output written to: " << outputFile << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}