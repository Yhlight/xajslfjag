#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>

#include "CompilerDispatcher/CompilerDispatcher.h"
#include "CHTL/CHTLContext/Context.h"
#include "Util/FileSystem/FileSystem.h"

using namespace CHTL;

/// 显示帮助信息
void ShowHelp() {
    std::cout << "CHTL编译器 v1.0.0\n";
    std::cout << "用法: chtl [选项] <输入文件>\n\n";
    std::cout << "选项:\n";
    std::cout << "  -h, --help              显示此帮助信息\n";
    std::cout << "  -o, --output <文件>      指定输出文件\n";
    std::cout << "  -d, --debug             启用调试模式\n";
    std::cout << "  -p, --parallel          启用并行编译\n";
    std::cout << "  -s, --stats             显示编译统计信息\n";
    std::cout << "  -v, --verbose           详细输出\n";
    std::cout << "  --version               显示版本信息\n\n";
    std::cout << "示例:\n";
    std::cout << "  chtl input.chtl -o output.html\n";
    std::cout << "  chtl input.chtl --debug --stats\n";
}

/// 显示版本信息
void ShowVersion() {
    std::cout << "CHTL编译器 v1.0.0\n";
    std::cout << "基于C++17构建，支持完全模块化的CHTL和CHTL JS编译\n";
    std::cout << "Copyright (C) 2024 CHTL Team\n";
}

/// 编译配置结构
struct CompileOptions {
    std::string inputFile;
    std::string outputFile;
    bool debugMode = false;
    bool parallelMode = true;
    bool showStats = false;
    bool verbose = false;
};

/// 解析命令行参数
CompileOptions ParseArguments(int argc, char* argv[]) {
    CompileOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            ShowHelp();
            exit(0);
        }
        else if (arg == "--version") {
            ShowVersion();
            exit(0);
        }
        else if (arg == "-d" || arg == "--debug") {
            options.debugMode = true;
        }
        else if (arg == "-p" || arg == "--parallel") {
            options.parallelMode = true;
        }
        else if (arg == "-s" || arg == "--stats") {
            options.showStats = true;
        }
        else if (arg == "-v" || arg == "--verbose") {
            options.verbose = true;
        }
        else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            } else {
                std::cerr << "错误: -o/--output 需要一个参数\n";
                exit(1);
            }
        }
        else if (arg[0] != '-') {
            if (options.inputFile.empty()) {
                options.inputFile = arg;
            } else {
                std::cerr << "错误: 只能指定一个输入文件\n";
                exit(1);
            }
        }
        else {
            std::cerr << "错误: 未知选项 " << arg << "\n";
            exit(1);
        }
    }
    
    if (options.inputFile.empty()) {
        std::cerr << "错误: 需要指定输入文件\n";
        std::cerr << "使用 --help 查看帮助信息\n";
        exit(1);
    }
    
    // 如果没有指定输出文件，基于输入文件名生成
    if (options.outputFile.empty()) {
        size_t lastDot = options.inputFile.find_last_of('.');
        if (lastDot != std::string::npos) {
            options.outputFile = options.inputFile.substr(0, lastDot) + ".html";
        } else {
            options.outputFile = options.inputFile + ".html";
        }
    }
    
    return options;
}

/// 读取文件内容
std::string ReadFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filePath);
    }
    
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    return content;
}

/// 写入文件内容
void WriteFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("无法创建文件: " + filePath);
    }
    
    file << content;
}

/// 主编译函数
int Compile(const CompileOptions& options) {
    try {
        if (options.verbose) {
            std::cout << "正在编译文件: " << options.inputFile << "\n";
            std::cout << "输出文件: " << options.outputFile << "\n";
        }
        
        // 读取源代码
        std::string sourceCode = ReadFile(options.inputFile);
        
        // 创建编译上下文
        auto context = std::make_shared<Context>();
        context->SetCurrentFilePath(options.inputFile);
        context->SetDebugMode(options.debugMode);
        
        // 创建编译器调度器
        DispatcherConfig config;
        config.enableParallelCompilation = options.parallelMode;
        config.enableDebugInfo = options.debugMode;
        
        auto dispatcher = options.debugMode ? 
            CompilerFactory::CreateDebug() : 
            CompilerFactory::CreateDefault();
        
        dispatcher->SetConfig(config);
        
        // 执行编译
        if (options.verbose) {
            std::cout << "开始编译...\n";
        }
        
        auto result = dispatcher->Compile(sourceCode, context);
        
        // 检查编译结果
        if (!result.success) {
            std::cerr << "编译失败！\n";
            
            // 输出错误信息
            if (!result.errors.empty()) {
                std::cerr << "\n错误信息:\n";
                for (const auto& error : result.errors) {
                    std::cerr << "  " << error << "\n";
                }
            }
            
            // 输出调度器错误
            const auto& dispatcherErrors = dispatcher->GetErrors();
            if (!dispatcherErrors.empty()) {
                std::cerr << "\n调度器错误:\n";
                for (const auto& error : dispatcherErrors) {
                    std::cerr << "  " << error << "\n";
                }
            }
            
            return 1;
        }
        
        // 输出警告信息
        if (!result.warnings.empty() && options.verbose) {
            std::cout << "\n警告信息:\n";
            for (const auto& warning : result.warnings) {
                std::cout << "  " << warning << "\n";
            }
        }
        
        // 写入输出文件
        WriteFile(options.outputFile, result.content);
        
        if (options.verbose) {
            std::cout << "编译成功完成！\n";
        }
        
        // 显示统计信息
        if (options.showStats) {
            std::cout << "\n编译统计信息:\n";
            std::cout << dispatcher->GetStatistics() << "\n";
            
            // 显示结果元数据
            if (!result.metadata.empty()) {
                std::cout << "结果元数据:\n";
                for (const auto& pair : result.metadata) {
                    std::cout << "  " << pair.first << ": " << pair.second << "\n";
                }
            }
        }
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "编译过程中发生异常: " << e.what() << "\n";
        return 1;
    }
}

/// 主函数
int main(int argc, char* argv[]) {
    // 设置UTF-8输出（Windows）
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif
    
    try {
        // 解析命令行参数
        CompileOptions options = ParseArguments(argc, argv);
        
        // 执行编译
        return Compile(options);
    }
    catch (const std::exception& e) {
        std::cerr << "程序异常: " << e.what() << "\n";
        return 1;
    }
}