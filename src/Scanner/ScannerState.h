#pragma once

#include <stack>
#include <string>
#include "CodeSlice.h"

namespace CHTL {

// 扫描器状态
class ScannerState {
public:
    // 块类型
    enum class BlockType {
        NONE,
        ELEMENT,              // 元素块 {}
        STYLE,               // style块
        SCRIPT,              // script块
        TEMPLATE,            // 模板定义
        CUSTOM,              // 自定义定义
        CONFIGURATION,       // 配置块
        ORIGIN,              // 原始嵌入块
        NAMESPACE,           // 命名空间块
        IMPORT,              // 导入语句
        TEXT,                // text块
        COMMENT              // 注释块
    };
    
    // 块信息
    struct BlockInfo {
        BlockType type;
        std::string name;      // 块名称（如元素名、模板名等）
        size_t startOffset;    // 块开始位置
        int braceDepth;        // 花括号深度
        SliceContext context;  // 块的上下文
        
        BlockInfo(BlockType t, const std::string& n, size_t offset, SliceContext ctx)
            : type(t), name(n), startOffset(offset), braceDepth(0), context(ctx) {}
    };
    
    ScannerState() : currentOffset_(0), inString_(false), stringDelimiter_('\0') {}
    
    // 进入新块
    void PushBlock(BlockType type, const std::string& name, SliceContext context) {
        blockStack_.push(BlockInfo(type, name, currentOffset_, context));
    }
    
    // 退出当前块
    void PopBlock() {
        if (!blockStack_.empty()) {
            blockStack_.pop();
        }
    }
    
    // 获取当前块信息
    BlockInfo* GetCurrentBlock() {
        return blockStack_.empty() ? nullptr : &blockStack_.top();
    }
    
    // 获取当前上下文
    SliceContext GetCurrentContext() const {
        if (blockStack_.empty()) {
            return SliceContext::GLOBAL;
        }
        return blockStack_.top().context;
    }
    
    // 检查是否在特定类型的块中
    bool IsInBlock(BlockType type) const {
        std::stack<BlockInfo> temp = blockStack_;
        while (!temp.empty()) {
            if (temp.top().type == type) {
                return true;
            }
            temp.pop();
        }
        return false;
    }
    
    // 检查是否在局部样式块中
    bool IsInLocalStyle() const {
        return GetCurrentContext() == SliceContext::LOCAL_STYLE;
    }
    
    // 检查是否在局部脚本块中
    bool IsInLocalScript() const {
        return GetCurrentContext() == SliceContext::LOCAL_SCRIPT;
    }
    
    // 检查是否在全局样式块中
    bool IsInGlobalStyle() const {
        return GetCurrentContext() == SliceContext::GLOBAL_STYLE;
    }
    
    // 检查是否在全局脚本块中
    bool IsInGlobalScript() const {
        return GetCurrentContext() == SliceContext::GLOBAL_SCRIPT;
    }
    
    // 字符串状态管理
    void EnterString(char delimiter) {
        inString_ = true;
        stringDelimiter_ = delimiter;
    }
    
    void ExitString() {
        inString_ = false;
        stringDelimiter_ = '\0';
    }
    
    bool IsInString() const { return inString_; }
    char GetStringDelimiter() const { return stringDelimiter_; }
    
    // 更新花括号深度
    void IncreaseBraceDepth() {
        if (!blockStack_.empty()) {
            blockStack_.top().braceDepth++;
        }
    }
    
    void DecreaseBraceDepth() {
        if (!blockStack_.empty() && blockStack_.top().braceDepth > 0) {
            blockStack_.top().braceDepth--;
        }
    }
    
    int GetBraceDepth() const {
        return blockStack_.empty() ? 0 : blockStack_.top().braceDepth;
    }
    
    // 偏移量管理
    size_t GetCurrentOffset() const { return currentOffset_; }
    void SetCurrentOffset(size_t offset) { currentOffset_ = offset; }
    void AdvanceOffset(size_t count) { currentOffset_ += count; }
    
    // 重置状态
    void Reset() {
        while (!blockStack_.empty()) {
            blockStack_.pop();
        }
        currentOffset_ = 0;
        inString_ = false;
        stringDelimiter_ = '\0';
    }
    
private:
    std::stack<BlockInfo> blockStack_;
    size_t currentOffset_;
    bool inString_;
    char stringDelimiter_;
};

} // namespace CHTL