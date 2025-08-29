#include <iostream>
#include <string>
#include <vector>
#include "../CHTLJS/CJMODSystem/CJMODPackager.h"
#include "../Error/ErrorReport.h"

void printUsage(const char* program) {
    std::cout << "CJMOD Packager Tool\n";
    std::cout << "Usage: " << program << " <command> [options]\n";
    std::cout << "\nCommands:\n";
    std::cout << "  pack <dir> <output>    Pack a directory into a CJMOD file\n";
    std::cout << "  unpack <cjmod> <dir>   Unpack a CJMOD file to a directory\n";
    std::cout << "  info <cjmod>           Show information about a CJMOD file\n";
    std::cout << "  validate <dir>         Validate a CJMOD directory structure\n";
    std::cout << "  test <cjmod>           Test load extension from CJMOD\n";
    std::cout << "\nOptions:\n";
    std::cout << "  --sign                 Sign the package (requires key)\n";
    std::cout << "  --key <file>           Private key for signing\n";
    std::cout << "  --verify               Verify package signature\n";
    std::cout << "  --verbose              Show detailed output\n";
    std::cout << "  -h, --help             Show this help\n";
}

int main(int argc, char* argv[]) {
    using namespace CHTL;
    
    // 初始化错误报告系统
    auto& errorReport = ErrorReport::getInstance();
    errorReport.addReporter(std::make_shared<ConsoleErrorReporter>(true));
    
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string command = argv[1];
    bool sign = false;
    bool verify = false;
    bool verbose = false;
    std::string keyFile;
    
    // 解析选项
    std::vector<std::string> args;
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--sign") {
            sign = true;
        } else if (arg == "--key" && i + 1 < argc) {
            keyFile = argv[++i];
        } else if (arg == "--verify") {
            verify = true;
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg[0] != '-') {
            args.push_back(arg);
        } else {
            ErrorBuilder(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR)
                .withMessage("Unknown option: " + arg)
                .report();
            return 1;
        }
    }
    
    try {
        CJMODPackager packager;
        
        if (command == "pack") {
            if (args.size() < 2) {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR)
                    .withMessage("pack command requires <dir> and <output> arguments")
                    .report();
                return 1;
            }
            
            std::string dir = args[0];
            std::string output = args[1];
            
            if (verbose) {
                std::cout << "Packing directory: " << dir << "\n";
                std::cout << "Output file: " << output << "\n";
            }
            
            if (!packager.validateStructure(dir)) {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
                    .withMessage("Invalid CJMOD directory structure")
                    .atLocation(dir, 0, 0)
                    .report();
                return 1;
            }
            
            CJMODPackOptions options;
            options.sign = sign;
            options.privateKeyPath = keyFile;
            
            if (packager.packDirectory(dir, output, options)) {
                std::cout << "Successfully packed CJMOD: " << output << "\n";
                
                if (verbose) {
                    // 显示打包信息
                    CJMODInfo info;
                    if (packager.getModuleInfo(output, info)) {
                        std::cout << "Module: " << info.name << " v" << info.version << "\n";
                        std::cout << "Author: " << info.author << "\n";
                        std::cout << "Type: " << info.type << "\n";
                        if (info.hasExtension) {
                            std::cout << "Extension: Yes\n";
                        }
                        if (info.isSigned) {
                            std::cout << "Signed: Yes\n";
                        }
                    }
                }
                return 0;
            } else {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
                    .withMessage("Failed to pack CJMOD")
                    .report();
                return 1;
            }
            
        } else if (command == "unpack") {
            if (args.size() < 2) {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR)
                    .withMessage("unpack command requires <cjmod> and <dir> arguments")
                    .report();
                return 1;
            }
            
            std::string cjmodFile = args[0];
            std::string outputDir = args[1];
            
            if (verbose) {
                std::cout << "Unpacking CJMOD: " << cjmodFile << "\n";
                std::cout << "Output directory: " << outputDir << "\n";
            }
            
            CJMODUnpackOptions options;
            options.verifySignature = verify;
            
            if (packager.unpackToDirectory(cjmodFile, outputDir, options)) {
                std::cout << "Successfully unpacked CJMOD to: " << outputDir << "\n";
                return 0;
            } else {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
                    .withMessage("Failed to unpack CJMOD")
                    .report();
                return 1;
            }
            
        } else if (command == "info") {
            if (args.empty()) {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR)
                    .withMessage("info command requires <cjmod> argument")
                    .report();
                return 1;
            }
            
            std::string cjmodFile = args[0];
            CJMODInfo info;
            
            if (packager.getModuleInfo(cjmodFile, info)) {
                std::cout << "CJMOD Information:\n";
                std::cout << "  Name: " << info.name << "\n";
                std::cout << "  Version: " << info.version << "\n";
                std::cout << "  Description: " << info.description << "\n";
                std::cout << "  Author: " << info.author << "\n";
                std::cout << "  License: " << info.license << "\n";
                std::cout << "  Type: " << info.type << "\n";
                std::cout << "  API Version: " << info.apiVersion << "\n";
                std::cout << "  Has Extension: " << (info.hasExtension ? "Yes" : "No") << "\n";
                std::cout << "  Signed: " << (info.isSigned ? "Yes" : "No") << "\n";
                
                if (!info.dependencies.empty()) {
                    std::cout << "  Dependencies:\n";
                    for (const auto& [name, version] : info.dependencies) {
                        std::cout << "    - " << name << " >= " << version << "\n";
                    }
                }
                
                if (!info.syntaxDefinitions.empty()) {
                    std::cout << "  Syntax Definitions:\n";
                    for (const auto& [name, _] : info.syntaxDefinitions) {
                        std::cout << "    - " << name << "\n";
                    }
                }
                
                return 0;
            } else {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
                    .withMessage("Failed to read CJMOD info")
                    .report();
                return 1;
            }
            
        } else if (command == "validate") {
            if (args.empty()) {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR)
                    .withMessage("validate command requires <dir> argument")
                    .report();
                return 1;
            }
            
            std::string dir = args[0];
            
            if (verbose) {
                std::cout << "Validating directory: " << dir << "\n";
            }
            
            if (packager.validateStructure(dir)) {
                std::cout << "Directory structure is valid for CJMOD packaging\n";
                
                // 检查各个组件
                std::filesystem::path dirPath(dir);
                
                if (std::filesystem::exists(dirPath / "manifest.json")) {
                    std::cout << "  ✓ manifest.json found\n";
                }
                
                if (std::filesystem::exists(dirPath / "extension.cpp")) {
                    std::cout << "  ✓ extension.cpp found (C++ extension)\n";
                }
                
                if (std::filesystem::exists(dirPath / "syntax")) {
                    std::cout << "  ✓ syntax/ directory found\n";
                }
                
                if (std::filesystem::exists(dirPath / "docs")) {
                    std::cout << "  ✓ docs/ directory found\n";
                }
                
                return 0;
            } else {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
                    .withMessage("Invalid CJMOD directory structure")
                    .withDetail("Make sure the directory contains:\n"
                              "  - manifest.json (required)\n"
                              "  - extension.cpp (for C++ extensions)\n"
                              "  - syntax/ directory (for syntax definitions)\n"
                              "  - docs/ directory (optional)")
                    .report();
                return 1;
            }
            
        } else if (command == "test") {
            if (args.empty()) {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR)
                    .withMessage("test command requires <cjmod> argument")
                    .report();
                return 1;
            }
            
            std::string cjmodFile = args[0];
            
            if (verbose) {
                std::cout << "Testing CJMOD: " << cjmodFile << "\n";
            }
            
            // 创建加载器并测试加载
            CJMODLoader loader;
            if (loader.loadModule(cjmodFile)) {
                std::cout << "Successfully loaded CJMOD\n";
                
                CJMODInfo info;
                if (packager.getModuleInfo(cjmodFile, info)) {
                    auto module = loader.getModule(info.name);
                    if (module && module->extension) {
                        std::cout << "Extension loaded successfully\n";
                        
                        // 测试扩展
                        if (module->extension->test()) {
                            std::cout << "Extension test passed\n";
                            return 0;
                        } else {
                            std::cout << "Extension test failed\n";
                            return 1;
                        }
                    }
                }
            } else {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
                    .withMessage("Failed to load CJMOD")
                    .report();
                return 1;
            }
            
        } else {
            ErrorBuilder(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR)
                .withMessage("Unknown command: " + command)
                .report();
            printUsage(argv[0]);
            return 1;
        }
        
    } catch (const std::exception& e) {
        ErrorBuilder(ErrorLevel::FATAL, ErrorType::INTERNAL_ERROR)
            .withMessage("Unhandled exception")
            .withDetail(e.what())
            .report();
        return 1;
    }
}