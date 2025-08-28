#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <vector>
#include "CompilerDispatcher/CHTLCompilerDispatcher.h"

using namespace CHTL;

void PrintUsage(const char* programName) {
    std::cout << "CHTL编译器 v1.0.0" << std::endl;
    std::cout << "用法: " << programName << " [选项] <输入文件>" << std::endl;
    std::cout << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  -o <文件>     指定输出文件" << std::endl;
    std::cout << "  -f <格式>     输出格式 (html, html5)" << std::endl;
    std::cout << "  -O <级别>     优化级别 (0-3)" << std::endl;
    std::cout << "  -d            启用调试模式" << std::endl;
    std::cout << "  -m <路径>     添加模块搜索路径" << std::endl;
    std::cout << "  -v            显示版本信息" << std::endl;
    std::cout << "  -h            显示此帮助信息" << std::endl;
    std::cout << std::endl;
    std::cout << "CHTL项目特性:" << std::endl;
    std::cout << "  • 完全模块化架构，CHTL和CHTL JS完全独立" << std::endl;
    std::cout << "  • 统一扫描器实现精准代码切割" << std::endl;
    std::cout << "  • 支持CHTL、CHTL JS、CSS、JavaScript混合编译" << std::endl;
    std::cout << "  • CMOD/CJMOD模块系统" << std::endl;
    std::cout << "  • 严格遵循CHTL语法文档规范" << std::endl;
}

void PrintVersion() {
    std::cout << "CHTL编译器 v1.0.0" << std::endl;
    std::cout << "构建日期: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "C++标准: " << __cplusplus << std::endl;
    std::cout << std::endl;
    std::cout << "Copyright (c) 2024 CHTL Team" << std::endl;
    std::cout << "MIT License" << std::endl;
    std::cout << std::endl;
    std::cout << "项目架构:" << std::endl;
    std::cout << "  • CHTL编译器: 处理CHTL语法" << std::endl;
    std::cout << "  • CHTL JS编译器: 处理CHTL JS语法" << std::endl;
    std::cout << "  • 统一扫描器: 精准代码切割" << std::endl;
    std::cout << "  • 编译器调度器: 协调各编译器" << std::endl;
    std::cout << "  • CSS/JS编译器: ANTLR4实现" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string inputFile;
    std::string outputFile;
    std::string outputFormat = "html5";
    int optimizationLevel = 1;
    bool debugMode = false;
    std::vector<std::string> modulePaths;
    
    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            PrintUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            PrintVersion();
            return 0;
        } else if (arg == "-d" || arg == "--debug") {
            debugMode = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "错误: -o 选项需要指定文件名" << std::endl;
                return 1;
            }
        } else if (arg == "-f" || arg == "--format") {
            if (i + 1 < argc) {
                outputFormat = argv[++i];
            } else {
                std::cerr << "错误: -f 选项需要指定格式" << std::endl;
                return 1;
            }
        } else if (arg == "-O" || arg == "--optimize") {
            if (i + 1 < argc) {
                optimizationLevel = std::stoi(argv[++i]);
            } else {
                std::cerr << "错误: -O 选项需要指定优化级别" << std::endl;
                return 1;
            }
        } else if (arg == "-m" || arg == "--module-path") {
            if (i + 1 < argc) {
                modulePaths.push_back(argv[++i]);
            } else {
                std::cerr << "错误: -m 选项需要指定路径" << std::endl;
                return 1;
            }
        } else if (arg[0] == '-') {
            std::cerr << "错误: 未知选项 " << arg << std::endl;
            PrintUsage(argv[0]);
            return 1;
        } else {
            if (inputFile.empty()) {
                inputFile = arg;
            } else {
                std::cerr << "错误: 只能指定一个输入文件" << std::endl;
                return 1;
            }
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "错误: 必须指定输入文件" << std::endl;
        PrintUsage(argv[0]);
        return 1;
    }
    
    // 如果没有指定输出文件，使用默认名称
    if (outputFile.empty()) {
        size_t lastDot = inputFile.find_last_of('.');
        if (lastDot != std::string::npos) {
            outputFile = inputFile.substr(0, lastDot) + ".html";
        } else {
            outputFile = inputFile + ".html";
        }
    }
    
    try {
        // 创建编译器调度器
        auto dispatcher = std::make_unique<CHTLCompilerDispatcher>();
        
        // 配置编译器
        dispatcher->SetOutputFormat(outputFormat);
        dispatcher->SetOptimizationLevel(optimizationLevel);
        dispatcher->EnableDebugMode(debugMode);
        
        // 添加模块路径
        for (const auto& path : modulePaths) {
            dispatcher->AddModulePath(path);
        }
        
        std::cout << "正在编译: " << inputFile << std::endl;
        
        // 编译文件
        auto result = dispatcher->CompileFile(inputFile);
        
        if (result->success) {
            // 写入输出文件
            std::ofstream outFile(outputFile);
            if (outFile.is_open()) {
                outFile << result->output;
                outFile.close();
                
                std::cout << "编译成功: " << outputFile << std::endl;
                
                if (debugMode) {
                    dispatcher->PrintCompilationStats();
                }
                
                // 显示警告
                if (!result->warnings.empty()) {
                    std::cout << "警告:" << std::endl;
                    for (const auto& warning : result->warnings) {
                        std::cout << "  " << warning << std::endl;
                    }
                }
                
            } else {
                std::cerr << "错误: 无法写入输出文件 " << outputFile << std::endl;
                return 1;
            }
        } else {
            std::cerr << "编译失败:" << std::endl;
            for (const auto& error : result->errors) {
                std::cerr << "  " << error << std::endl;
            }
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "编译器内部错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}