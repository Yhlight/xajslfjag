#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <memory>
#include <vector>

#include "CompilerDispatcher/CompilerDispatcher.h"
#include "Util/FileSystem.h"

// 使用中文模式
void PrintUsage(const char* program) {
    std::cout << "CHTL编译器 v1.0.0\n";
    std::cout << "用法: " << program << " [选项] <输入文件>\n";
    std::cout << "选项:\n";
    std::cout << "  -o <文件>     指定输出文件\n";
    std::cout << "  -m <目录>     指定模块目录\n";
    std::cout << "  --pack        打包CMOD/CJMOD模块\n";
    std::cout << "  --unpack      解包CMOD/CJMOD模块\n";
    std::cout << "  --debug       开启调试模式\n";
    std::cout << "  --help        显示帮助信息\n";
}

int main(int argc, char* argv[]) {
    // 设置UTF-8输出
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    if (argc < 2) {
        PrintUsage(argv[0]);
        return 1;
    }

    std::string inputFile;
    std::string outputFile;
    std::string moduleDir;
    bool packMode = false;
    bool unpackMode = false;
    bool debugMode = false;

    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "-m" && i + 1 < argc) {
            moduleDir = argv[++i];
        } else if (arg == "--pack") {
            packMode = true;
        } else if (arg == "--unpack") {
            unpackMode = true;
        } else if (arg == "--debug") {
            debugMode = true;
        } else if (arg == "--help") {
            PrintUsage(argv[0]);
            return 0;
        } else if (arg[0] != '-') {
            inputFile = arg;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "错误: 未指定输入文件\n";
        PrintUsage(argv[0]);
        return 1;
    }

    try {
        // 检查输入文件是否存在
        if (!std::filesystem::exists(inputFile)) {
            std::cerr << "错误: 输入文件不存在: " << inputFile << "\n";
            return 1;
        }

        // 设置默认输出文件名
        if (outputFile.empty()) {
            std::filesystem::path inputPath(inputFile);
            outputFile = inputPath.stem().string() + ".html";
        }

        // 创建编译器调度器
        auto dispatcher = std::make_unique<CHTL::CompilerDispatcher>();
        dispatcher->SetDebugMode(debugMode);

        // 设置模块目录
        if (!moduleDir.empty()) {
            dispatcher->AddModulePath(moduleDir);
        }

        // 处理打包/解包模式
        if (packMode) {
            std::cout << "正在打包模块: " << inputFile << "\n";
            // TODO: 实现模块打包功能
            return 0;
        }

        if (unpackMode) {
            std::cout << "正在解包模块: " << inputFile << "\n";
            // TODO: 实现模块解包功能
            return 0;
        }

        // 读取输入文件
        std::string content = CHTL::FileSystem::ReadFile(inputFile);
        
        // 编译文件
        std::cout << "正在编译: " << inputFile << "\n";
        std::string result = dispatcher->Compile(content, inputFile);

        // 写入输出文件
        CHTL::FileSystem::WriteFile(outputFile, result);
        std::cout << "编译成功，输出文件: " << outputFile << "\n";

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << "\n";
        return 1;
    }
}