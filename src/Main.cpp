#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include "Dispatcher/CompilerDispatcher.h"
#include "Common/SourceFile.h"
#include "Common/Error.h"

using namespace CHTL;

// 打印使用帮助
void PrintUsage(const char* programName) {
    std::cout << "CHTL编译器 v1.0.0\n";
    std::cout << "用法: " << programName << " [选项] <输入文件>\n";
    std::cout << "\n选项:\n";
    std::cout << "  -o <文件>     指定输出文件（默认: output.html）\n";
    std::cout << "  -d, --debug   启用调试模式\n";
    std::cout << "  -h, --help    显示此帮助信息\n";
    std::cout << "  -v, --version 显示版本信息\n";
}

// 读取文件内容
std::string ReadFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// 写入文件
bool WriteFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

int main(int argc, char* argv[]) {
    // 解析命令行参数
    std::string inputFile;
    std::string outputFile = "output.html";
    bool debugMode = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            PrintUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            std::cout << "CHTL编译器 v1.0.0\n";
            return 0;
        } else if (arg == "-d" || arg == "--debug") {
            debugMode = true;
        } else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg[0] != '-') {
            inputFile = arg;
        } else {
            std::cerr << "错误: 未知选项 " << arg << "\n";
            PrintUsage(argv[0]);
            return 1;
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "错误: 未指定输入文件\n";
        PrintUsage(argv[0]);
        return 1;
    }
    
    // 读取输入文件
    std::string content = ReadFile(inputFile);
    if (content.empty()) {
        std::cerr << "错误: 无法读取文件 " << inputFile << "\n";
        return 1;
    }
    
    // 创建源文件对象
    auto sourceFile = std::make_shared<SourceFile>(inputFile, content);
    
    // 创建错误收集器
    ErrorCollector errorCollector;
    
    // 创建编译器调度器
    CompilerDispatcher dispatcher;
    if (debugMode) {
        dispatcher.SetDebugMode(true);
        std::cout << "调试模式已启用\n";
    }
    
    // 编译
    std::cout << "正在编译 " << inputFile << "...\n";
    std::string result = dispatcher.Compile(sourceFile, errorCollector);
    
    // 检查错误
    if (errorCollector.HasErrors()) {
        std::cerr << "\n编译错误:\n";
        errorCollector.PrintErrors();
        return 1;
    }
    
    // 写入输出文件
    if (!WriteFile(outputFile, result)) {
        std::cerr << "错误: 无法写入文件 " << outputFile << "\n";
        return 1;
    }
    
    std::cout << "编译成功! 输出文件: " << outputFile << "\n";
    
    // 打印警告（如果有）
    const auto& errors = errorCollector.GetErrors();
    int warningCount = 0;
    for (const auto& error : errors) {
        if (error->GetLevel() == ErrorLevel::WARNING) {
            warningCount++;
        }
    }
    
    if (warningCount > 0) {
        std::cout << "警告: " << warningCount << " 个\n";
        if (debugMode) {
            errorCollector.PrintErrors();
        }
    }
    
    return 0;
}