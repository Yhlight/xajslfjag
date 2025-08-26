#include "Core/Common.h"
#include "Core/Logger.h"
#include "Core/Timer.h"
#include "Dispatcher/CompilerDispatcher.h"
#include <iostream>
#include <cstring>

using namespace CHTL;

// 显示帮助信息
void ShowHelp(const char* program) {
    std::cout << "CHTL编译器 版本 " << VERSION << "\n";
    std::cout << "用法: " << program << " [选项] <输入文件>\n\n";
    std::cout << "选项:\n";
    std::cout << "  -o, --output <文件>    指定输出文件 (默认: output.html)\n";
    std::cout << "  -m, --module <目录>    指定模块搜索目录\n";
    std::cout << "  -c, --config <文件>    指定配置文件\n";
    std::cout << "  --pack-cmod <目录>     打包CMOD模块\n";
    std::cout << "  --pack-cjmod <目录>    打包CJMOD模块\n";
    std::cout << "  --unpack <文件>        解包模块文件\n";
    std::cout << "  --debug               启用调试模式\n";
    std::cout << "  --timer               显示编译时间\n";
    std::cout << "  -h, --help            显示此帮助信息\n";
    std::cout << "  -v, --version         显示版本信息\n";
}

// 显示版本信息
void ShowVersion() {
    std::cout << "CHTL编译器 版本 " << VERSION << "\n";
    std::cout << "平台: " << PLATFORM << "\n";
    std::cout << "C++标准: C++17\n";
}

// 命令行参数结构
struct CommandLineArgs {
    String inputFile;
    String outputFile = "output.html";
    Vector<String> modulePaths;
    String configFile;
    String packCmodDir;
    String packCjmodDir;
    String unpackFile;
    bool debug = false;
    bool showTimer = false;
    bool showHelp = false;
    bool showVersion = false;
};

// 解析命令行参数
CommandLineArgs ParseCommandLine(int argc, char* argv[]) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        String arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            args.showHelp = true;
            return args;
        }
        else if (arg == "-v" || arg == "--version") {
            args.showVersion = true;
            return args;
        }
        else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                args.outputFile = argv[++i];
            }
        }
        else if (arg == "-m" || arg == "--module") {
            if (i + 1 < argc) {
                args.modulePaths.push_back(argv[++i]);
            }
        }
        else if (arg == "-c" || arg == "--config") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        }
        else if (arg == "--pack-cmod") {
            if (i + 1 < argc) {
                args.packCmodDir = argv[++i];
            }
        }
        else if (arg == "--pack-cjmod") {
            if (i + 1 < argc) {
                args.packCjmodDir = argv[++i];
            }
        }
        else if (arg == "--unpack") {
            if (i + 1 < argc) {
                args.unpackFile = argv[++i];
            }
        }
        else if (arg == "--debug") {
            args.debug = true;
        }
        else if (arg == "--timer") {
            args.showTimer = true;
        }
        else if (arg[0] != '-') {
            args.inputFile = arg;
        }
    }
    
    return args;
}

int main(int argc, char* argv[]) {
    try {
        // 初始化UTF-8支持
        InitializeUTF8Support();
        
        // 解析命令行参数
        CommandLineArgs args = ParseCommandLine(argc, argv);
        
        // 显示帮助或版本
        if (args.showHelp) {
            ShowHelp(argv[0]);
            return 0;
        }
        
        if (args.showVersion) {
            ShowVersion();
            return 0;
        }
        
        // 初始化日志系统
        Logger::Initialize(args.debug ? LogLevel::Debug : LogLevel::Info);
        
        // 创建编译计时器
        UniquePtr<CompileTimer> timer;
        if (args.showTimer) {
            timer = std::make_unique<CompileTimer>();
            timer->Start();
        }
        
        // 处理模块打包/解包
        if (!args.packCmodDir.empty()) {
            // TODO: 实现CMOD打包
            Logger::Info("打包CMOD模块: " + args.packCmodDir);
            return 0;
        }
        
        if (!args.packCjmodDir.empty()) {
            // TODO: 实现CJMOD打包
            Logger::Info("打包CJMOD模块: " + args.packCjmodDir);
            return 0;
        }
        
        if (!args.unpackFile.empty()) {
            // TODO: 实现模块解包
            Logger::Info("解包模块: " + args.unpackFile);
            return 0;
        }
        
        // 检查输入文件
        if (args.inputFile.empty()) {
            Logger::Error("错误: 未指定输入文件");
            ShowHelp(argv[0]);
            return 1;
        }
        
        // 创建编译器调度器
        CompilerDispatcher dispatcher;
        
        // 设置模块路径
        for (const auto& path : args.modulePaths) {
            dispatcher.AddModulePath(path);
        }
        
        // 设置配置文件
        if (!args.configFile.empty()) {
            dispatcher.SetConfigFile(args.configFile);
        }
        
        // 编译文件
        Logger::Info("编译文件: " + args.inputFile);
        auto result = dispatcher.CompileFile(args.inputFile, args.outputFile);
        
        if (result.IsError()) {
            Logger::Error("编译失败: " + result.Error());
            return 1;
        }
        
        Logger::Info("编译成功: " + args.outputFile);
        
        // 显示编译时间
        if (timer) {
            timer->Stop();
            Logger::Info("编译耗时: " + timer->GetElapsedString());
        }
        
        return 0;
    }
    catch (const CHTLException& e) {
        Logger::Error("CHTL错误: " + String(e.what()));
        if (!e.GetLocation().filename.empty()) {
            Logger::Error("位置: " + e.GetLocation().filename + 
                         ":" + std::to_string(e.GetLocation().range.start.line) +
                         ":" + std::to_string(e.GetLocation().range.start.column));
        }
        return 1;
    }
    catch (const std::exception& e) {
        Logger::Error("未知错误: " + String(e.what()));
        return 1;
    }
}