#include "Dispatcher/CompilerDispatcher.h"
#include "Core/Logger.h"
#include "Scanner/UnifiedScanner.h"

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() {
    InitializeCompilers();
}

CompilerDispatcher::~CompilerDispatcher() = default;

void CompilerDispatcher::InitializeCompilers() {
    scanner_ = std::make_unique<UnifiedScanner>();
    // TODO: 初始化其他编译器
    LOG_DEBUG("编译器调度器初始化完成");
}

void CompilerDispatcher::AddModulePath(const String& path) {
    modulePaths_.push_back(Path(path));
    LOG_DEBUG("添加模块路径: " + path);
}

void CompilerDispatcher::SetConfigFile(const String& configFile) {
    configFile_ = Path(configFile);
    LOG_DEBUG("设置配置文件: " + configFile);
}

void CompilerDispatcher::SetOption(const String& name, bool value) {
    if (name == "debug") options_.debug = value;
    else if (name == "enableTimer") options_.enableTimer = value;
    else if (name == "enableOptimization") options_.enableOptimization = value;
    else if (name == "enableMinification") options_.enableMinification = value;
    else if (name == "enableSourceMap") options_.enableSourceMap = value;
}

void CompilerDispatcher::SetOption(const String& name, const String& value) {
    if (name == "outputEncoding") options_.outputEncoding = value;
}

Result<String> CompilerDispatcher::CompileFile(const String& inputFile, const String& outputFile) {
    CompileContext context;
    context.currentFile = Path(inputFile);
    context.outputFile = Path(outputFile);
    
    // TODO: 实现完整的编译流程
    LOG_INFO("开始编译文件: " + inputFile);
    
    // 读取文件内容
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        return Err<String>("无法打开文件: " + inputFile);
    }
    
    String content((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());
    file.close();
    
    // 使用统一扫描器扫描代码
    auto scanResult = scanner_->ScanCode(content, inputFile);
    if (scanResult.IsError()) {
        return Err<String>("扫描失败: " + scanResult.Error());
    }
    
    auto slices = scanResult.Value();
    LOG_INFO("扫描完成，生成 " + std::to_string(slices.size()) + " 个切片");
    
    // TODO: 分发切片到各编译器
    // TODO: 合并结果
    // TODO: 写入输出文件
    
    return Ok(String("编译成功"));
}

Result<String> CompilerDispatcher::CompileString(const String& content, const String& filename) {
    CompileContext context;
    context.currentFile = Path(filename);
    
    // TODO: 实现字符串编译
    return Ok(String(""));
}

Result<void> CompilerDispatcher::PackCMOD(const String& directory, const String& outputFile) {
    // TODO: 实现CMOD打包
    return Ok();
}

Result<void> CompilerDispatcher::PackCJMOD(const String& directory, const String& outputFile) {
    // TODO: 实现CJMOD打包
    return Ok();
}

Result<void> CompilerDispatcher::UnpackModule(const String& moduleFile, const String& outputDir) {
    // TODO: 实现模块解包
    return Ok();
}

} // namespace CHTL