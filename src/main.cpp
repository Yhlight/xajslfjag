#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

#include "CompilerDispatcher/Dispatcher.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "Util/Common.h"

using namespace CHTL;

// 命令行参数结构
struct CommandLineArgs {
    String inputFile;
    String outputFile;
    String outputDir = "./output";
    bool verbose = false;
    bool debug = false;
    bool minify = false;
    bool generateSourceMaps = false;
    bool strictMode = false;
    bool showHelp = false;
    bool showVersion = false;
    StringVector includePaths;
    StringVector defines;
    String configFile;
    
    // 编译选项
    bool enableOptimizations = true;
    bool enableCaching = true;
    bool enableParallelCompilation = true;
    size_t threadCount = 0; // 0 = auto
    
    // 输出选项
    bool generateHTML = true;
    bool generateCSS = true;
    bool generateJS = true;
    bool separateOutputFiles = false;
};

// 版本信息
constexpr const char* CHTL_VERSION = "1.0.0";
constexpr const char* CHTL_BUILD_DATE = __DATE__;
constexpr const char* CHTL_BUILD_TIME = __TIME__;

// 显示帮助信息
void showHelp() {
    std::cout << "CHTL Compiler " << CHTL_VERSION << "\n";
    std::cout << "基于C++17实现的超文本语言编译器\n\n";
    
    std::cout << "用法: chtl [选项] <输入文件>\n\n";
    
    std::cout << "选项:\n";
    std::cout << "  -h, --help              显示此帮助信息\n";
    std::cout << "  -v, --version           显示版本信息\n";
    std::cout << "  -o, --output <文件>     指定输出文件\n";
    std::cout << "  -d, --output-dir <目录> 指定输出目录 (默认: ./output)\n";
    std::cout << "  --verbose               详细输出\n";
    std::cout << "  --debug                 启用调试模式\n";
    std::cout << "  --minify                压缩输出\n";
    std::cout << "  --source-maps           生成源映射\n";
    std::cout << "  --strict                严格模式\n";
    std::cout << "  -I <路径>               添加包含路径\n";
    std::cout << "  -D <定义>               添加预处理器定义\n";
    std::cout << "  -c, --config <文件>     指定配置文件\n";
    std::cout << "  -j, --threads <数量>    编译线程数 (0=自动)\n\n";
    
    std::cout << "编译选项:\n";
    std::cout << "  --no-optimizations      禁用优化\n";
    std::cout << "  --no-cache              禁用缓存\n";
    std::cout << "  --no-parallel           禁用并行编译\n\n";
    
    std::cout << "输出选项:\n";
    std::cout << "  --no-html               不生成HTML\n";
    std::cout << "  --no-css                不生成CSS\n";
    std::cout << "  --no-js                 不生成JavaScript\n";
    std::cout << "  --separate              分离输出文件\n\n";
    
    std::cout << "示例:\n";
    std::cout << "  chtl input.chtl -o output.html\n";
    std::cout << "  chtl input.chtl -d ./build --minify\n";
    std::cout << "  chtl input.chtl --debug --verbose\n";
}

// 显示版本信息
void showVersion() {
    std::cout << "CHTL编译器 版本 " << CHTL_VERSION << "\n";
    std::cout << "构建时间: " << CHTL_BUILD_DATE << " " << CHTL_BUILD_TIME << "\n";
    std::cout << "C++标准: " << __cplusplus << "\n";
    
#ifdef __GNUC__
    std::cout << "编译器: GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << "\n";
#elif defined(__clang__)
    std::cout << "编译器: Clang " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__ << "\n";
#elif defined(_MSC_VER)
    std::cout << "编译器: MSVC " << _MSC_VER << "\n";
#endif
    
    std::cout << "\n使用MIT开源协议\n";
    std::cout << "项目主页: https://github.com/CHTL-Team/CHTL\n";
}

// 解析命令行参数
CommandLineArgs parseCommandLine(int argc, char* argv[]) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        String arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            args.showHelp = true;
        } else if (arg == "-v" || arg == "--version") {
            args.showVersion = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                args.outputFile = argv[++i];
            } else {
                std::cerr << "错误: -o 选项需要参数\n";
                exit(1);
            }
        } else if (arg == "-d" || arg == "--output-dir") {
            if (i + 1 < argc) {
                args.outputDir = argv[++i];
            } else {
                std::cerr << "错误: -d 选项需要参数\n";
                exit(1);
            }
        } else if (arg == "--verbose") {
            args.verbose = true;
        } else if (arg == "--debug") {
            args.debug = true;
        } else if (arg == "--minify") {
            args.minify = true;
        } else if (arg == "--source-maps") {
            args.generateSourceMaps = true;
        } else if (arg == "--strict") {
            args.strictMode = true;
        } else if (arg == "-I") {
            if (i + 1 < argc) {
                args.includePaths.push_back(argv[++i]);
            } else {
                std::cerr << "错误: -I 选项需要参数\n";
                exit(1);
            }
        } else if (arg == "-D") {
            if (i + 1 < argc) {
                args.defines.push_back(argv[++i]);
            } else {
                std::cerr << "错误: -D 选项需要参数\n";
                exit(1);
            }
        } else if (arg == "-c" || arg == "--config") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            } else {
                std::cerr << "错误: -c 选项需要参数\n";
                exit(1);
            }
        } else if (arg == "-j" || arg == "--threads") {
            if (i + 1 < argc) {
                args.threadCount = std::stoul(argv[++i]);
            } else {
                std::cerr << "错误: -j 选项需要参数\n";
                exit(1);
            }
        } else if (arg == "--no-optimizations") {
            args.enableOptimizations = false;
        } else if (arg == "--no-cache") {
            args.enableCaching = false;
        } else if (arg == "--no-parallel") {
            args.enableParallelCompilation = false;
        } else if (arg == "--no-html") {
            args.generateHTML = false;
        } else if (arg == "--no-css") {
            args.generateCSS = false;
        } else if (arg == "--no-js") {
            args.generateJS = false;
        } else if (arg == "--separate") {
            args.separateOutputFiles = true;
        } else if (arg.empty() || arg[0] == '-') {
            std::cerr << "错误: 未知选项 " << arg << "\n";
            exit(1);
        } else {
            if (args.inputFile.empty()) {
                args.inputFile = arg;
            } else {
                std::cerr << "错误: 只能指定一个输入文件\n";
                exit(1);
            }
        }
    }
    
    return args;
}

// 配置编译器
DispatcherConfig createDispatcherConfig(const CommandLineArgs& args) {
    DispatcherConfig config;
    
    // 基础配置
    config.enableParallelCompilation = args.enableParallelCompilation;
    config.enableCaching = args.enableCaching;
    config.enableOptimization = args.enableOptimizations;
    config.strictMode = args.strictMode;
    
    if (args.threadCount > 0) {
        config.maxWorkerThreads = args.threadCount;
    }
    
    // 生成器配置
    config.generatorConfig.minifyOutput = args.minify;
    config.generatorConfig.generateSourceMaps = args.generateSourceMaps;
    config.generatorConfig.includeDebugInfo = args.debug;
    
    // 解析器配置
    config.parserConfig.strictMode = args.strictMode;
    config.parserConfig.enableRecovery = !args.strictMode;
    
    return config;
}

// 创建输出文件名
String createOutputFileName(const CommandLineArgs& args, const String& extension) {
    if (!args.outputFile.empty()) {
        // 如果指定了输出文件，使用该文件名但替换扩展名
        String baseName = args.outputFile;
        size_t dotPos = baseName.find_last_of('.');
        if (dotPos != String::npos) {
            baseName = baseName.substr(0, dotPos);
        }
        return baseName + extension;
    } else {
        // 根据输入文件生成输出文件名
        String baseName = args.inputFile;
        size_t dotPos = baseName.find_last_of('.');
        if (dotPos != String::npos) {
            baseName = baseName.substr(0, dotPos);
        }
        size_t slashPos = baseName.find_last_of("/\\");
        if (slashPos != String::npos) {
            baseName = baseName.substr(slashPos + 1);
        }
        return args.outputDir + "/" + baseName + extension;
    }
}

// 写入输出文件
bool writeOutputFiles(const CommandLineArgs& args, const CompilationResult& result) {
    bool success = true;
    
    try {
        if (args.separateOutputFiles) {
            // 分离输出文件
            if (args.generateHTML && !result.html.empty()) {
                String htmlFile = createOutputFileName(args, ".html");
                Utils::writeFile(htmlFile, result.html);
                if (args.verbose) {
                    std::cout << "HTML 输出: " << htmlFile << "\n";
                }
            }
            
            if (args.generateCSS && !result.css.empty()) {
                String cssFile = createOutputFileName(args, ".css");
                Utils::writeFile(cssFile, result.css);
                if (args.verbose) {
                    std::cout << "CSS 输出: " << cssFile << "\n";
                }
            }
            
            if (args.generateJS && !result.javascript.empty()) {
                String jsFile = createOutputFileName(args, ".js");
                Utils::writeFile(jsFile, result.javascript);
                if (args.verbose) {
                    std::cout << "JavaScript 输出: " << jsFile << "\n";
                }
            }
        } else {
            // 合并输出到HTML文件
            String outputFile = createOutputFileName(args, ".html");
            String combinedOutput = result.html;
            
            if (args.generateCSS && !result.css.empty()) {
                combinedOutput += "\n<style>\n" + result.css + "\n</style>\n";
            }
            
            if (args.generateJS && !result.javascript.empty()) {
                combinedOutput += "\n<script>\n" + result.javascript + "\n</script>\n";
            }
            
            Utils::writeFile(outputFile, combinedOutput);
            if (args.verbose) {
                std::cout << "输出文件: " << outputFile << "\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "写入输出文件时出错: " << e.what() << "\n";
        success = false;
    }
    
    return success;
}

// 主编译函数
int compileFile(const CommandLineArgs& args) {
    if (args.verbose) {
        std::cout << "开始编译: " << args.inputFile << "\n";
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // 创建编译器配置
        DispatcherConfig config = createDispatcherConfig(args);
        
        // 创建编译器调度器
        auto dispatcher = std::make_unique<CompilerDispatcher>(config);
        
        if (args.debug) {
            dispatcher->enableDebugMode(true);
        }
        
        // 编译文件
        CompilationResult result = dispatcher->compileFile(args.inputFile);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        if (!result.success) {
            std::cerr << "编译失败:\n";
            for (const auto& error : result.errors) {
                std::cerr << "错误: " << error << "\n";
            }
            return 1;
        }
        
        // 显示警告
        if (!result.warnings.empty()) {
            for (const auto& warning : result.warnings) {
                std::cout << "警告: " << warning << "\n";
            }
        }
        
        // 写入输出文件
        if (!writeOutputFiles(args, result)) {
            return 1;
        }
        
        if (args.verbose) {
            std::cout << "编译成功完成\n";
            std::cout << "编译时间: " << duration.count() << "ms\n";
            std::cout << "输入大小: " << result.inputSize << " 字节\n";
            std::cout << "输出大小: " << result.outputSize << " 字节\n";
            
            if (result.inputSize > 0) {
                double ratio = static_cast<double>(result.outputSize) / result.inputSize;
                std::cout << "压缩比: " << std::fixed << std::setprecision(2) << ratio << "\n";
            }
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "编译过程中发生异常: " << e.what() << "\n";
        return 1;
    }
}

// 主函数
int main(int argc, char* argv[]) {
    try {
        // 解析命令行参数
        CommandLineArgs args = parseCommandLine(argc, argv);
        
        if (args.showHelp) {
            showHelp();
            return 0;
        }
        
        if (args.showVersion) {
            showVersion();
            return 0;
        }
        
        if (args.inputFile.empty()) {
            std::cerr << "错误: 未指定输入文件\n";
            std::cerr << "使用 --help 查看帮助信息\n";
            return 1;
        }
        
        // 检查输入文件是否存在
        if (!Utils::fileExists(args.inputFile)) {
            std::cerr << "错误: 输入文件不存在: " << args.inputFile << "\n";
            return 1;
        }
        
        // 确保输出目录存在
        if (!args.outputDir.empty()) {
            // 这里应该实现目录创建逻辑
            // 为了简化，假设目录已存在
        }
        
        // 执行编译
        return compileFile(args);
        
    } catch (const std::exception& e) {
        std::cerr << "程序异常: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "未知异常\n";
        return 1;
    }
}