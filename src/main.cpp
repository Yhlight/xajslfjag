/**
 * CHTL项目主程序入口
 * 
 * 这是CHTL项目的核心入口点，负责初始化各个组件并启动编译流程
 * 
 * @author CHTL Team
 * @version 1.0.0
 * @license MIT
 */

#include <iostream>
#include <string>
#include <memory>

// 核心组件头文件
#include "Scanner/CHTLUnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"
#include "CHTL/CHTLManage/CHTLManage.h"
#include "CHTL JS/CHTLJSManage/CHTLJSManage.h"
#include "CSS/CSSCompiler.h"
#include "JS/JSCompiler.h"
#include "Util/Logger.h"

using namespace std;

/**
 * 显示帮助信息
 */
void ShowHelp() {
    cout << "CHTL编译器 v1.0.0" << endl;
    cout << "用法: chtl_compiler [选项] <输入文件>" << endl;
    cout << endl;
    cout << "选项:" << endl;
    cout << "  -h, --help     显示此帮助信息" << endl;
    cout << "  -v, --version  显示版本信息" << endl;
    cout << "  -o <输出文件>  指定输出文件" << endl;
    cout << "  -d <输出目录>  指定输出目录" << endl;
    cout << "  --verbose      详细输出模式" << endl;
    cout << endl;
    cout << "示例:" << endl;
    cout << "  chtl_compiler input.chtl -o output.html" << endl;
    cout << "  chtl_compiler input.chtl -d ./output" << endl;
}

/**
 * 显示版本信息
 */
void ShowVersion() {
    cout << "CHTL编译器 v1.0.0" << endl;
    cout << "基于C++17实现的超文本语言编译器" << endl;
    cout << "使用MIT开源协议" << endl;
    cout << endl;
    cout << "架构特性:" << endl;
    cout << "  - 完全模块化设计" << endl;
    cout << "  - CHTL和CHTL JS完全分离" << endl;
    cout << "  - 统一扫描器架构" << endl;
    cout << "  - 支持CMOD和CJMOD模块系统" << endl;
}

/**
 * 主函数
 */
int main(int argc, char* argv[]) {
    // 初始化日志系统
    Logger::Initialize();
    Logger::Info("CHTL编译器启动");
    
    try {
        // 解析命令行参数
        string inputFile;
        string outputFile;
        string outputDir;
        bool verbose = false;
        
        for (int i = 1; i < argc; i++) {
            string arg = argv[i];
            
            if (arg == "-h" || arg == "--help") {
                ShowHelp();
                return 0;
            }
            else if (arg == "-v" || arg == "--version") {
                ShowVersion();
                return 0;
            }
            else if (arg == "-o" && i + 1 < argc) {
                outputFile = argv[++i];
            }
            else if (arg == "-d" && i + 1 < argc) {
                outputDir = argv[++i];
            }
            else if (arg == "--verbose") {
                verbose = true;
            }
            else if (arg[0] != '-') {
                inputFile = arg;
            }
        }
        
        // 检查输入文件
        if (inputFile.empty()) {
            cerr << "错误: 未指定输入文件" << endl;
            ShowHelp();
            return 1;
        }
        
        Logger::Info("输入文件: " + inputFile);
        if (!outputFile.empty()) {
            Logger::Info("输出文件: " + outputFile);
        }
        if (!outputDir.empty()) {
            Logger::Info("输出目录: " + outputDir);
        }
        
        // 初始化统一扫描器
        auto scanner = make_unique<CHTLUnifiedScanner>();
        Logger::Info("统一扫描器初始化完成");
        
        // 初始化编译器调度器
        auto dispatcher = make_unique<CompilerDispatcher>();
        Logger::Info("编译器调度器初始化完成");
        
        // 初始化各个编译器
        auto chtlManager = make_unique<CHTLManage>();
        auto chtljsManager = make_unique<CHTLJSManage>();
        auto cssCompiler = make_unique<CSSCompiler>();
        auto jsCompiler = make_unique<JSCompiler>();
        
        Logger::Info("所有编译器初始化完成");
        
        // 执行编译流程
        Logger::Info("开始编译流程");
        
        // TODO: 实现完整的编译流程
        // 1. 使用统一扫描器切割代码
        // 2. 通过调度器分发到各个编译器
        // 3. 收集编译结果并合并
        // 4. 生成最终输出
        
        Logger::Info("编译流程完成");
        
    } catch (const exception& e) {
        Logger::Error("编译过程中发生错误: " + string(e.what()));
        cerr << "错误: " << e.what() << endl;
        return 1;
    } catch (...) {
        Logger::Error("编译过程中发生未知错误");
        cerr << "错误: 发生未知错误" << endl;
        return 1;
    }
    
    Logger::Info("CHTL编译器正常退出");
    return 0;
}