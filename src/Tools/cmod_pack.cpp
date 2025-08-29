#include <iostream>
#include <string>
#include <vector>
#include "../CHTL/CMODSystem/CMODPackager.h"
#include "../Error/ErrorReport.h"

void printUsage(const char* program) {
    std::cout << "CMOD Packager Tool\n";
    std::cout << "Usage: " << program << " <command> [options]\n";
    std::cout << "\nCommands:\n";
    std::cout << "  pack <dir> <output>    Pack a directory into a CMOD file\n";
    std::cout << "  unpack <cmod> <dir>    Unpack a CMOD file to a directory\n";
    std::cout << "  info <cmod>            Show information about a CMOD file\n";
    std::cout << "  validate <dir>         Validate a CMOD directory structure\n";
    std::cout << "\nOptions:\n";
    std::cout << "  --compress             Enable compression (default)\n";
    std::cout << "  --no-compress          Disable compression\n";
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
    bool compress = true;
    bool verbose = false;
    
    // 解析选项
    std::vector<std::string> args;
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--compress") {
            compress = true;
        } else if (arg == "--no-compress") {
            compress = false;
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
        CMODPackager packager;
        packager.setCompressionEnabled(compress);
        
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
                    .withMessage("Invalid CMOD directory structure")
                    .atLocation(dir, 0, 0)
                    .report();
                return 1;
            }
            
            if (packager.packDirectory(dir, output)) {
                std::cout << "Successfully packed CMOD: " << output << "\n";
                if (verbose) {
                    // 显示打包信息
                    CMODInfo info;
                    if (packager.getModuleInfo(output, info)) {
                        std::cout << "Module: " << info.moduleName << " v" << info.version << "\n";
                        std::cout << "Author: " << info.author << "\n";
                        std::cout << "Files: " << info.fileList.size() << "\n";
                    }
                }
                return 0;
            } else {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
                    .withMessage("Failed to pack CMOD")
                    .report();
                return 1;
            }
            
        } else if (command == "unpack") {
            if (args.size() < 2) {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR)
                    .withMessage("unpack command requires <cmod> and <dir> arguments")
                    .report();
                return 1;
            }
            
            std::string cmodFile = args[0];
            std::string outputDir = args[1];
            
            if (verbose) {
                std::cout << "Unpacking CMOD: " << cmodFile << "\n";
                std::cout << "Output directory: " << outputDir << "\n";
            }
            
            if (packager.unpackToDirectory(cmodFile, outputDir)) {
                std::cout << "Successfully unpacked CMOD to: " << outputDir << "\n";
                return 0;
            } else {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
                    .withMessage("Failed to unpack CMOD")
                    .report();
                return 1;
            }
            
        } else if (command == "info") {
            if (args.empty()) {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR)
                    .withMessage("info command requires <cmod> argument")
                    .report();
                return 1;
            }
            
            std::string cmodFile = args[0];
            CMODInfo info;
            
            if (packager.getModuleInfo(cmodFile, info)) {
                std::cout << "CMOD Information:\n";
                std::cout << "  Module Name: " << info.moduleName << "\n";
                std::cout << "  Version: " << info.version << "\n";
                std::cout << "  Description: " << info.description << "\n";
                std::cout << "  Author: " << info.author << "\n";
                std::cout << "  License: " << info.license << "\n";
                std::cout << "  Created: " << info.timestamp << "\n";
                std::cout << "  Compressed: " << (info.isCompressed ? "Yes" : "No") << "\n";
                std::cout << "  File Count: " << info.fileList.size() << "\n";
                
                if (!info.dependencies.empty()) {
                    std::cout << "  Dependencies:\n";
                    for (const auto& dep : info.dependencies) {
                        std::cout << "    - " << dep << "\n";
                    }
                }
                
                if (verbose) {
                    std::cout << "  Files:\n";
                    for (const auto& file : info.fileList) {
                        std::cout << "    - " << file << "\n";
                    }
                }
                
                return 0;
            } else {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
                    .withMessage("Failed to read CMOD info")
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
                std::cout << "Directory structure is valid for CMOD packaging\n";
                return 0;
            } else {
                ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
                    .withMessage("Invalid CMOD directory structure")
                    .withDetail("Make sure the directory contains:\n"
                              "  - cmod.json (manifest file)\n"
                              "  - src/ directory with CHTL source files\n"
                              "  - Optionally: info/, submodules/ directories")
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