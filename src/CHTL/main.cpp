#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <thread>
#include <chrono>
#include "../CompilerDispatcher/CompilerDispatcher.h"
#include "../CHTL/CHTLIOStream/CHTLFileSystem.h"

void printUsage(const char* program) {
    std::cout << "CHTL Compiler v1.0.0\n";
    std::cout << "Usage: " << program << " [options] <input-file>\n";
    std::cout << "Options:\n";
    std::cout << "  -o <file>          Output file (default: output.html)\n";
    std::cout << "  -d <dir>           Output directory (default: ./)\n";
    std::cout << "  --minify           Minify output\n";
    std::cout << "  --prettify         Prettify output (default)\n";
    std::cout << "  --source-map       Generate source map\n";
    std::cout << "  --target <version> JavaScript target version (ES5, ES6, etc.)\n";
    std::cout << "  --module <system>  Module system (ESM, CommonJS, AMD)\n";
    std::cout << "  --watch            Watch for file changes\n";
    std::cout << "  --strict           Enable strict mode\n";
    std::cout << "  --debug            Enable debug output\n";
    std::cout << "  -v, --version      Show version\n";
    std::cout << "  -h, --help         Show this help\n";
}

int main(int argc, char* argv[]) {
    using namespace CHTL;
    
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    // 解析命令行参数
    CompileOptions options;
    std::string inputFile;
    bool watch = false;
    bool debug = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-o" && i + 1 < argc) {
            options.outputFile = argv[++i];
        }
        else if (arg == "-d" && i + 1 < argc) {
            options.outputDir = argv[++i];
        }
        else if (arg == "--minify") {
            options.minify = true;
            options.prettify = false;
        }
        else if (arg == "--prettify") {
            options.prettify = true;
            options.minify = false;
        }
        else if (arg == "--source-map") {
            options.generateSourceMap = true;
        }
        else if (arg == "--target" && i + 1 < argc) {
            options.targetVersion = argv[++i];
        }
        else if (arg == "--module" && i + 1 < argc) {
            options.customConfig["moduleSystem"] = argv[++i];
        }
        else if (arg == "--watch") {
            watch = true;
        }
        else if (arg == "--strict") {
            options.customConfig["strict"] = "true";
        }
        else if (arg == "--debug") {
            debug = true;
            options.enableDebugInfo = true;
        }
        else if (arg == "-v" || arg == "--version") {
            std::cout << "CHTL Compiler v1.0.0\n";
            return 0;
        }
        else if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg[0] != '-') {
            inputFile = arg;
        }
        else {
            std::cerr << "Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }
    
    // 检查输入文件是否存在
    if (!FileSystem::exists(inputFile)) {
        std::cerr << "Error: Input file not found: " << inputFile << "\n";
        return 1;
    }
    
    // 设置默认输出文件
    if (options.outputFile.empty()) {
        options.outputFile = PathUtil::replaceExtension(
            PathUtil::filename(inputFile), ".html"
        );
    }
    
    // 创建输出目录
    if (!options.outputDir.empty()) {
        FileSystem::createDirectories(options.outputDir);
        options.outputFile = PathUtil::join(options.outputDir, options.outputFile);
    }
    
    try {
        // 创建编译器调度器
        auto& manager = CompilerManager::getInstance();
        auto dispatcher = manager.getDefaultDispatcher();
        dispatcher->setOptions(options);
        
        // 设置错误处理
        dispatcher->setErrorHandler([](const std::string& error) {
            std::cerr << "Error: " << error << "\n";
        });
        
        dispatcher->setWarningHandler([](const std::string& warning) {
            std::cerr << "Warning: " << warning << "\n";
        });
        
        if (debug) {
            dispatcher->setProgressCallback([](size_t current, size_t total) {
                std::cout << "Processing fragment " << current << "/" << total << "\n";
            });
        }
        
        // 编译文件
        std::cout << "Compiling " << inputFile << "...\n";
        auto result = dispatcher->compile(inputFile);
        
        if (result.success) {
            std::cout << "Compilation successful!\n";
            std::cout << "Output written to: " << result.outputPath << "\n";
            
            if (debug) {
                std::cout << "Processed " << result.processedFragments << " fragments\n";
                std::cout << "Compilation time: " << result.compilationTime << " ms\n";
            }
            
            // Watch模式
            if (watch) {
                std::cout << "Watching for changes... (Press Ctrl+C to stop)\n";
                
                FileWatcher watcher;
                watcher.addPath(inputFile);
                watcher.setCallback([&](const std::string& path, FileType type) {
                    if (type == FileType::Regular) {
                        std::cout << "File changed: " << path << "\n";
                        std::cout << "Recompiling...\n";
                        
                        auto watchResult = dispatcher->compile(inputFile);
                        if (watchResult.success) {
                            std::cout << "Recompilation successful!\n";
                        } else {
                            std::cerr << "Recompilation failed!\n";
                            for (const auto& error : watchResult.errors) {
                                std::cerr << "Error: " << error << "\n";
                            }
                        }
                    }
                });
                
                watcher.start();
                
                // 保持程序运行
                while (true) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
            
            return 0;
        } else {
            std::cerr << "Compilation failed!\n";
            for (const auto& error : result.errors) {
                std::cerr << "Error: " << error << "\n";
            }
            for (const auto& warning : result.warnings) {
                std::cerr << "Warning: " << warning << "\n";
            }
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
}