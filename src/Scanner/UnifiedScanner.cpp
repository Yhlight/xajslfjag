#include "UnifiedScanner.h"
#include <algorithm>
#include <iostream>
#include <sstream>

namespace CHTL {

UnifiedScanner::UnifiedScanner(const ScanConfig& config) 
    : config_(config), currentFile_(nullptr), errorCollector_(nullptr) {
    InitializeMarkers();
}

void UnifiedScanner::InitializeMarkers() {
    // 初始化CHTL标记
    chtlMarkers_ = {
        // 模板和自定义标记
        {"[Template]", nullptr, ScannerState::BlockType::TEMPLATE, SliceType::CHTL},
        {"[Custom]", nullptr, ScannerState::BlockType::CUSTOM, SliceType::CHTL},
        {"[Configuration]", nullptr, ScannerState::BlockType::CONFIGURATION, SliceType::CHTL},
        {"[Origin]", nullptr, ScannerState::BlockType::ORIGIN, SliceType::CHTL},
        {"[Import]", nullptr, ScannerState::BlockType::IMPORT, SliceType::CHTL},
        {"[Namespace]", nullptr, ScannerState::BlockType::NAMESPACE, SliceType::CHTL},
        
        // 关键字
        {"text", nullptr, ScannerState::BlockType::TEXT, SliceType::CHTL},
        {"style", nullptr, ScannerState::BlockType::STYLE, SliceType::CHTL},
        {"script", nullptr, ScannerState::BlockType::SCRIPT, SliceType::CHTL},
        
        // 特殊符号
        {"@Style", nullptr, ScannerState::BlockType::NONE, SliceType::CHTL},
        {"@Element", nullptr, ScannerState::BlockType::NONE, SliceType::CHTL},
        {"@Var", nullptr, ScannerState::BlockType::NONE, SliceType::CHTL},
        {"@Html", nullptr, ScannerState::BlockType::NONE, SliceType::CHTL},
        {"@JavaScript", nullptr, ScannerState::BlockType::NONE, SliceType::CHTL},
        {"@Chtl", nullptr, ScannerState::BlockType::NONE, SliceType::CHTL},
        {"@CJmod", nullptr, ScannerState::BlockType::NONE, SliceType::CHTL},
        {"@Config", nullptr, ScannerState::BlockType::NONE, SliceType::CHTL}
    };
    
    // 初始化CHTL JS标记
    chtljsMarkers_ = {
        {"{{", SliceType::CHTL_JS, true},      // 增强选择器
        {"->", SliceType::CHTL_JS, true},      // CHTL JS方法调用
        {"vir", SliceType::CHTL_JS, true},     // 虚对象
        {"listen", SliceType::CHTL_JS, true},   // 事件监听
        {"delegate", SliceType::CHTL_JS, true}, // 事件委托
        {"animate", SliceType::CHTL_JS, true}   // 动画
    };
}

std::vector<CodeSlicePtr> UnifiedScanner::Scan(const SourceFilePtr& sourceFile, ErrorCollector& errorCollector) {
    currentFile_ = sourceFile;
    errorCollector_ = &errorCollector;
    state_.Reset();
    
    std::vector<CodeSlicePtr> slices;
    const std::string& content = sourceFile->GetContent();
    size_t offset = 0;
    size_t sliceStart = 0;
    
    while (offset < content.length()) {
        // 处理注释
        if (HandleComment(content, offset, slices)) {
            sliceStart = offset;
            continue;
        }
        
        // 处理字符串
        if (state_.IsInString()) {
            if (content[offset] == state_.GetStringDelimiter() && 
                (offset == 0 || content[offset - 1] != '\\')) {
                state_.ExitString();
            }
            offset++;
            continue;
        } else if (content[offset] == '"' || content[offset] == '\'') {
            state_.EnterString(content[offset]);
            offset++;
            continue;
        }
        
        // 检测CHTL语法
        size_t syntaxEnd = offset;
        if (DetectCHTLSyntax(content, offset, syntaxEnd)) {
            // 保存之前的切片
            if (offset > sliceStart) {
                auto slice = ProcessSlice(content.substr(sliceStart, offset - sliceStart), 
                                        sliceStart, offset);
                if (slice) slices.push_back(slice);
            }
            
            // 创建CHTL切片
            auto chtlSlice = std::make_shared<CodeSlice>(
                SliceType::CHTL,
                content.substr(offset, syntaxEnd - offset),
                CreateLocation(offset),
                CreateLocation(syntaxEnd),
                state_.GetCurrentContext()
            );
            slices.push_back(chtlSlice);
            
            offset = syntaxEnd;
            sliceStart = offset;
            continue;
        }
        
        // 检测CHTL JS语法
        if (state_.IsInLocalScript() || state_.IsInGlobalScript()) {
            if (DetectCHTLJSSyntax(content, offset, syntaxEnd)) {
                // 保存之前的切片
                if (offset > sliceStart) {
                    auto slice = ProcessSlice(content.substr(sliceStart, offset - sliceStart), 
                                            sliceStart, offset);
                    if (slice) slices.push_back(slice);
                }
                
                // 创建CHTL JS切片
                auto chtljsSlice = std::make_shared<CodeSlice>(
                    SliceType::CHTL_JS,
                    content.substr(offset, syntaxEnd - offset),
                    CreateLocation(offset),
                    CreateLocation(syntaxEnd),
                    state_.GetCurrentContext()
                );
                slices.push_back(chtljsSlice);
                
                offset = syntaxEnd;
                sliceStart = offset;
                continue;
            }
        }
        
        // 检测块的开始和结束
        std::string blockName;
        ScannerState::BlockType blockType;
        if (DetectBlockStart(content, offset, blockName, blockType)) {
            // 保存之前的切片
            if (offset > sliceStart) {
                auto slice = ProcessSlice(content.substr(sliceStart, offset - sliceStart), 
                                        sliceStart, offset);
                if (slice) slices.push_back(slice);
            }
            
            // 更新状态
            SliceContext newContext = state_.GetCurrentContext();
            if (blockType == ScannerState::BlockType::STYLE) {
                newContext = state_.IsInBlock(ScannerState::BlockType::ELEMENT) 
                    ? SliceContext::LOCAL_STYLE : SliceContext::GLOBAL_STYLE;
            } else if (blockType == ScannerState::BlockType::SCRIPT) {
                newContext = state_.IsInBlock(ScannerState::BlockType::ELEMENT) 
                    ? SliceContext::LOCAL_SCRIPT : SliceContext::GLOBAL_SCRIPT;
            }
            
            state_.PushBlock(blockType, blockName, newContext);
            sliceStart = offset;
        }
        
        if (content[offset] == '{') {
            state_.IncreaseBraceDepth();
        } else if (content[offset] == '}') {
            state_.DecreaseBraceDepth();
            if (state_.GetBraceDepth() == 0 && state_.GetCurrentBlock()) {
                // 创建块结束前的切片
                if (offset + 1 > sliceStart) {
                    auto slice = ProcessSlice(content.substr(sliceStart, offset + 1 - sliceStart), 
                                            sliceStart, offset + 1);
                    if (slice) slices.push_back(slice);
                }
                
                state_.PopBlock();
                sliceStart = offset + 1;
            }
        }
        
        offset++;
        state_.SetCurrentOffset(offset);
        
        // 检查是否需要创建新切片
        if (offset - sliceStart >= config_.initialSliceSize) {
            auto slice = ProcessSlice(content.substr(sliceStart, offset - sliceStart), 
                                    sliceStart, offset);
            if (slice) slices.push_back(slice);
            sliceStart = offset;
        }
    }
    
    // 处理最后的切片
    if (sliceStart < content.length()) {
        auto slice = ProcessSlice(content.substr(sliceStart), sliceStart, content.length());
        if (slice) slices.push_back(slice);
    }
    
    // 合并相邻的相同类型切片
    if (config_.mergeAdjacentSlices) {
        std::vector<CodeSlicePtr> mergedSlices;
        for (const auto& slice : slices) {
            if (!mergedSlices.empty() && mergedSlices.back()->CanMergeWith(*slice)) {
                mergedSlices.back()->MergeWith(*slice);
            } else {
                mergedSlices.push_back(slice);
            }
        }
        slices = std::move(mergedSlices);
    }
    
    return slices;
}

CodeSlicePtr UnifiedScanner::ProcessSlice(const std::string& content, size_t startOffset, size_t endOffset) {
    if (content.empty()) return nullptr;
    
    SliceType type = DetermineSliceType(content, startOffset);
    Location startLoc = CreateLocation(startOffset);
    Location endLoc = CreateLocation(endOffset);
    
    return std::make_shared<CodeSlice>(type, content, startLoc, endLoc, state_.GetCurrentContext());
}

bool UnifiedScanner::DetectCHTLSyntax(const std::string& content, size_t offset, size_t& syntaxEnd) {
    // 跳过空白
    offset = SkipWhitespace(content, offset);
    
    // 检查所有CHTL标记
    for (const auto& marker : chtlMarkers_) {
        if (LookAhead(content, offset, marker.pattern)) {
            syntaxEnd = offset + marker.pattern.length();
            
            // 如果有验证器，执行验证
            if (marker.validator && !marker.validator(content, syntaxEnd)) {
                continue;
            }
            
            // 对于某些标记，需要读取完整的语法单元
            if (marker.pattern[0] == '[') {
                // 读取到']'
                while (syntaxEnd < content.length() && content[syntaxEnd] != ']') {
                    syntaxEnd++;
                }
                if (syntaxEnd < content.length()) syntaxEnd++;
            } else if (marker.pattern[0] == '@') {
                // 读取完整的标识符
                while (syntaxEnd < content.length() && IsIdentifierChar(content[syntaxEnd])) {
                    syntaxEnd++;
                }
            }
            
            return true;
        }
    }
    
    // 检查是否是HTML元素
    if (IsIdentifierStart(content[offset])) {
        size_t identEnd = offset;
        while (identEnd < content.length() && IsIdentifierChar(content[identEnd])) {
            identEnd++;
        }
        
        // 跳过空白
        size_t nextNonWhite = SkipWhitespace(content, identEnd);
        
        // 如果后面跟着 { 或 : 或 ; 则是CHTL语法
        if (nextNonWhite < content.length() && 
            (content[nextNonWhite] == '{' || content[nextNonWhite] == ':' || content[nextNonWhite] == ';')) {
            syntaxEnd = identEnd;
            return true;
        }
    }
    
    return false;
}

bool UnifiedScanner::DetectCHTLJSSyntax(const std::string& content, size_t offset, size_t& syntaxEnd) {
    for (const auto& marker : chtljsMarkers_) {
        if (LookAhead(content, offset, marker.pattern)) {
            syntaxEnd = offset + marker.pattern.length();
            
            // 特殊处理增强选择器
            if (marker.pattern == "{{") {
                // 找到对应的 }}
                int braceCount = 2;
                syntaxEnd = offset + 2;
                while (syntaxEnd < content.length() - 1 && braceCount > 0) {
                    if (content[syntaxEnd] == '}' && content[syntaxEnd + 1] == '}') {
                        braceCount -= 2;
                        syntaxEnd += 2;
                        if (braceCount == 0) break;
                    } else if (content[syntaxEnd] == '{' && content[syntaxEnd + 1] == '{') {
                        braceCount += 2;
                        syntaxEnd += 2;
                    } else {
                        syntaxEnd++;
                    }
                }
            }
            
            return true;
        }
    }
    
    return false;
}

bool UnifiedScanner::DetectBlockStart(const std::string& content, size_t offset, 
                                     std::string& blockName, ScannerState::BlockType& blockType) {
    offset = SkipWhitespace(content, offset);
    
    // 检查是否是已知的块类型
    for (const auto& marker : chtlMarkers_) {
        if (marker.blockType != ScannerState::BlockType::NONE && 
            LookAhead(content, offset, marker.pattern)) {
            blockName = marker.pattern;
            blockType = marker.blockType;
            return true;
        }
    }
    
    // 检查是否是HTML元素
    if (IsIdentifierStart(content[offset])) {
        size_t nameStart = offset;
        blockName = ReadIdentifier(content, offset);
        
        // 跳过空白
        offset = SkipWhitespace(content, offset);
        
        // 检查是否后面跟着 {
        if (offset < content.length() && content[offset] == '{') {
            blockType = ScannerState::BlockType::ELEMENT;
            return true;
        }
        
        // 恢复offset
        offset = nameStart;
    }
    
    return false;
}

SliceType UnifiedScanner::DetermineSliceType(const std::string& content, size_t offset) {
    // 根据当前上下文判断
    SliceContext context = state_.GetCurrentContext();
    
    switch (context) {
        case SliceContext::LOCAL_STYLE:
        case SliceContext::GLOBAL_STYLE:
            return SliceType::CSS;
            
        case SliceContext::LOCAL_SCRIPT:
        case SliceContext::GLOBAL_SCRIPT:
            return SliceType::JAVASCRIPT;
            
        case SliceContext::ORIGIN_BLOCK:
            // 原始嵌入块根据块类型决定
            if (state_.GetCurrentBlock()) {
                const std::string& blockName = state_.GetCurrentBlock()->name;
                if (blockName.find("@Html") != std::string::npos) {
                    return SliceType::HTML;
                } else if (blockName.find("@Style") != std::string::npos) {
                    return SliceType::CSS;
                } else if (blockName.find("@JavaScript") != std::string::npos) {
                    return SliceType::JAVASCRIPT;
                }
            }
            return SliceType::TEXT;
            
        default:
            // 尝试检测是否是CHTL语法
            size_t syntaxEnd;
            if (DetectCHTLSyntax(content, offset, syntaxEnd)) {
                return SliceType::CHTL;
            }
            return SliceType::TEXT;
    }
}

bool UnifiedScanner::HandleComment(const std::string& content, size_t& offset, std::vector<CodeSlicePtr>& slices) {
    // 单行注释 //
    if (offset + 1 < content.length() && content[offset] == '/' && content[offset + 1] == '/') {
        size_t commentStart = offset;
        offset += 2;
        
        // 读取到行尾
        while (offset < content.length() && content[offset] != '\n') {
            offset++;
        }
        if (offset < content.length()) offset++; // 包含换行符
        
        auto commentSlice = std::make_shared<CodeSlice>(
            SliceType::CHTL,
            content.substr(commentStart, offset - commentStart),
            CreateLocation(commentStart),
            CreateLocation(offset),
            state_.GetCurrentContext()
        );
        slices.push_back(commentSlice);
        return true;
    }
    
    // 多行注释 /* */
    if (offset + 1 < content.length() && content[offset] == '/' && content[offset + 1] == '*') {
        size_t commentStart = offset;
        offset += 2;
        
        // 查找 */
        while (offset + 1 < content.length()) {
            if (content[offset] == '*' && content[offset + 1] == '/') {
                offset += 2;
                break;
            }
            offset++;
        }
        
        auto commentSlice = std::make_shared<CodeSlice>(
            SliceType::CHTL,
            content.substr(commentStart, offset - commentStart),
            CreateLocation(commentStart),
            CreateLocation(offset),
            state_.GetCurrentContext()
        );
        slices.push_back(commentSlice);
        return true;
    }
    
    // 生成器注释 --
    if (offset + 1 < content.length() && content[offset] == '-' && content[offset + 1] == '-') {
        size_t commentStart = offset;
        offset += 2;
        
        // 读取到行尾或遇到另一个--
        while (offset < content.length()) {
            if (content[offset] == '\n') {
                offset++;
                break;
            }
            if (offset + 1 < content.length() && content[offset] == '-' && content[offset + 1] == '-') {
                offset += 2;
                break;
            }
            offset++;
        }
        
        auto commentSlice = std::make_shared<CodeSlice>(
            SliceType::CHTL,
            content.substr(commentStart, offset - commentStart),
            CreateLocation(commentStart),
            CreateLocation(offset),
            state_.GetCurrentContext()
        );
        slices.push_back(commentSlice);
        return true;
    }
    
    return false;
}

bool UnifiedScanner::LookAhead(const std::string& content, size_t offset, const std::string& pattern) const {
    if (offset + pattern.length() > content.length()) {
        return false;
    }
    return content.substr(offset, pattern.length()) == pattern;
}

size_t UnifiedScanner::SkipWhitespace(const std::string& content, size_t offset) const {
    while (offset < content.length() && IsWhitespace(content[offset])) {
        offset++;
    }
    return offset;
}

std::string UnifiedScanner::ReadIdentifier(const std::string& content, size_t& offset) const {
    size_t start = offset;
    if (offset < content.length() && IsIdentifierStart(content[offset])) {
        offset++;
        while (offset < content.length() && IsIdentifierChar(content[offset])) {
            offset++;
        }
    }
    return content.substr(start, offset - start);
}

Location UnifiedScanner::CreateLocation(size_t offset) const {
    if (!currentFile_) {
        return Location(1, 0, offset);
    }
    
    size_t line, column;
    currentFile_->GetLineColumn(offset, line, column);
    return Location(line, column, offset, currentFile_->GetFilename());
}

void UnifiedScanner::DebugLog(const std::string& message) const {
    if (config_.debugMode) {
        std::cout << "[Scanner Debug] " << message << std::endl;
    }
}

} // namespace CHTL