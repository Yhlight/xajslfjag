#include "Scanner/UnifiedScanner.h"
#include "Core/Logger.h"
#include <algorithm>
#include <cctype>

namespace CHTL {

// TokenIdentifier 实现
TokenIdentifier::TokenIdentifier() {
    InitializeKeywords();
}

void TokenIdentifier::InitializeKeywords() {
    // CHTL 关键字（基于语法文档）
    chtlKeywords_ = {
        "text", "style", "script",
        "inherit", "delete", "insert",
        "after", "before", "replace", "at",
        "from", "as", "except",
        "use", "html5"
    };
    
    // CHTL 块标记
    chtlBlockMarkers_ = {
        "[Template]", "[Custom]", "[Origin]", "[Import]",
        "[Namespace]", "[Configuration]", "[Info]", "[Export]"
    };
    
    // CHTL JS 关键字
    chtljsKeywords_ = {
        "vir", "listen", "delegate", "animate",
        "printMylove", "iNeverAway"
    };
}

bool TokenIdentifier::IsCHTLKeyword(const String& token) const {
    return chtlKeywords_.find(token) != chtlKeywords_.end();
}

bool TokenIdentifier::IsCHTLJSKeyword(const String& token) const {
    return chtljsKeywords_.find(token) != chtljsKeywords_.end();
}

bool TokenIdentifier::IsCHTLBlockMarker(const String& token) const {
    return chtlBlockMarkers_.find(token) != chtlBlockMarkers_.end();
}

Optional<String> TokenIdentifier::GetTokenType(const String& token) const {
    if (IsCHTLKeyword(token)) return "CHTL_KEYWORD";
    if (IsCHTLJSKeyword(token)) return "CHTLJS_KEYWORD";
    if (IsCHTLBlockMarker(token)) return "CHTL_BLOCK_MARKER";
    return std::nullopt;
}

// SliceManager 实现
void SliceManager::AddSlice(const ScannerSlice& slice) {
    size_t index = slices_.size();
    slices_.push_back(slice);
    categoryIndex_[slice.category].push_back(index);
}

Vector<ScannerSlice> SliceManager::GetSlicesByCategory(SliceCategory category) const {
    Vector<ScannerSlice> result;
    auto it = categoryIndex_.find(category);
    if (it != categoryIndex_.end()) {
        for (size_t index : it->second) {
            result.push_back(slices_[index]);
        }
    }
    return result;
}

void SliceManager::MergeSlices(const std::function<bool(const ScannerSlice&, const ScannerSlice&)>& canMerge) {
    if (slices_.size() < 2) return;
    
    Vector<ScannerSlice> merged;
    merged.push_back(slices_[0]);
    
    for (size_t i = 1; i < slices_.size(); ++i) {
        if (canMerge(merged.back(), slices_[i])) {
            // 合并切片
            merged.back().content += slices_[i].content;
            merged.back().end = slices_[i].end;
        } else {
            merged.push_back(slices_[i]);
        }
    }
    
    slices_ = std::move(merged);
    
    // 重建索引
    categoryIndex_.clear();
    for (size_t i = 0; i < slices_.size(); ++i) {
        categoryIndex_[slices_[i].category].push_back(i);
    }
}

void SliceManager::OptimizeSlices() {
    // 合并相邻的相同类型切片
    MergeSlices([](const ScannerSlice& a, const ScannerSlice& b) {
        return a.category == b.category && 
               a.mode == b.mode &&
               a.end.line == b.start.line &&
               a.end.column == b.start.column;
    });
}

void SliceManager::Clear() {
    slices_.clear();
    categoryIndex_.clear();
}

// UnifiedScanner 实现
UnifiedScanner::UnifiedScanner() 
    : sliceManager_(std::make_unique<SliceManager>()),
      tokenIdentifier_(std::make_unique<TokenIdentifier>()) {
}

UnifiedScanner::UnifiedScanner(const ScannerConfig& config)
    : config_(config),
      sliceManager_(std::make_unique<SliceManager>()),
      tokenIdentifier_(std::make_unique<TokenIdentifier>()) {
}

UnifiedScanner::~UnifiedScanner() = default;

Result<Vector<ScannerSlice>> UnifiedScanner::ScanCode(const String& code, const String& filename) {
    Reset();
    sourceCode_ = code;
    filename_ = filename;
    
    LOG_DEBUG("开始扫描文件: " + filename);
    
    auto result = PerformScan();
    if (result.IsError()) {
        return Err<Vector<ScannerSlice>>(result.Error());
    }
    
    // 合并相邻切片
    if (config_.mergeAdjacentSlices) {
        MergeAdjacentSlices();
    }
    
    // 优化切片
    sliceManager_->OptimizeSlices();
    
    LOG_DEBUG("扫描完成，生成 " + std::to_string(slices_.size()) + " 个切片");
    
    return Ok(slices_);
}

void UnifiedScanner::Reset() {
    sourceCode_.clear();
    filename_.clear();
    currentPos_ = 0;
    currentPosition_ = Position();
    currentMode_ = ScanMode::Normal;
    while (!modeStack_.empty()) {
        modeStack_.pop();
    }
    slices_.clear();
    sliceManager_->Clear();
    doublePointer_ = DoublePointer();
}

Result<void> UnifiedScanner::PerformScan() {
    while (currentPos_ < sourceCode_.length()) {
        // 读取可变长度切片
        auto sliceResult = ReadVariableSlice();
        if (sliceResult.IsError()) {
            return Err<void>(sliceResult.Error());
        }
        
        String slice = sliceResult.Value();
        Position startPos = currentPosition_;
        
        // 识别并处理切片
        SliceCategory category = IdentifySliceCategory(slice);
        
        // 根据类别处理切片
        switch (category) {
            case SliceCategory::CHTLElement:
            case SliceCategory::CHTLAttribute:
            case SliceCategory::CHTLText:
            case SliceCategory::CHTLStyle:
            case SliceCategory::CHTLScript:
            case SliceCategory::CHTLTemplate:
            case SliceCategory::CHTLCustom:
            case SliceCategory::CHTLOrigin:
            case SliceCategory::CHTLImport:
            case SliceCategory::CHTLNamespace:
            case SliceCategory::CHTLConfiguration:
                ProcessCHTLSlice(slice, startPos);
                break;
                
            case SliceCategory::CHTLJSSelector:
            case SliceCategory::CHTLJSArrow:
            case SliceCategory::CHTLJSFunction:
            case SliceCategory::CHTLJSVirtual:
                ProcessCHTLJSSlice(slice, startPos);
                break;
                
            case SliceCategory::PlainCSS:
                ProcessCSSSlice(slice, startPos);
                break;
                
            case SliceCategory::PlainJavaScript:
                ProcessJavaScriptSlice(slice, startPos);
                break;
                
            default:
                // 直接添加切片
                AddSlice(category, slice, startPos, currentPosition_);
                break;
        }
    }
    
    return Ok();
}

Result<String> UnifiedScanner::ReadVariableSlice() {
    if (currentPos_ >= sourceCode_.length()) {
        return Ok(String());
    }
    
    size_t sliceSize = config_.initialSliceSize;
    size_t startPos = currentPos_;
    
    // 读取初始切片
    size_t endPos = std::min(startPos + sliceSize, sourceCode_.length());
    String slice = sourceCode_.substr(startPos, endPos - startPos);
    
    // 检查边界是否合理
    while (endPos < sourceCode_.length() && !CheckSliceBoundary(slice, endPos)) {
        // 扩展切片
        sliceSize = std::min(sliceSize * 2, config_.maxSliceSize);
        endPos = std::min(startPos + sliceSize, sourceCode_.length());
        slice = sourceCode_.substr(startPos, endPos - startPos);
    }
    
    // 更新位置
    for (size_t i = startPos; i < endPos; ++i) {
        UpdatePosition(sourceCode_[i]);
        currentPos_++;
    }
    
    return Ok(slice);
}

bool UnifiedScanner::CheckSliceBoundary(const String& slice, size_t nextCharPos) {
    // 检查是否在字符串或注释内部
    int stringCount = 0;
    int commentDepth = 0;
    bool inLineComment = false;
    bool inGeneratorComment = false;
    
    for (size_t i = 0; i < slice.length(); ++i) {
        char ch = slice[i];
        
        // 检查字符串
        if (ch == '"' || ch == '\'') {
            if (i == 0 || slice[i-1] != '\\') {
                stringCount++;
            }
        }
        
        // 检查注释
        if (i + 1 < slice.length()) {
            if (slice[i] == '/' && slice[i+1] == '/') {
                inLineComment = true;
            } else if (slice[i] == '/' && slice[i+1] == '*') {
                commentDepth++;
                i++; // 跳过*
            } else if (slice[i] == '*' && slice[i+1] == '/') {
                commentDepth--;
                i++; // 跳过/
            } else if (slice[i] == '-' && slice[i+1] == '-') {
                inGeneratorComment = true;
            }
        }
        
        if (ch == '\n') {
            inLineComment = false;
            inGeneratorComment = false;
        }
    }
    
    // 如果在字符串或注释内部，边界不合理
    if (stringCount % 2 != 0 || commentDepth != 0 || inLineComment || inGeneratorComment) {
        return false;
    }
    
    // 检查是否在CHTL块内部
    int braceDepth = 0;
    int bracketDepth = 0;
    
    for (char ch : slice) {
        if (ch == '{') braceDepth++;
        else if (ch == '}') braceDepth--;
        else if (ch == '[') bracketDepth++;
        else if (ch == ']') bracketDepth--;
    }
    
    // 如果括号不匹配，边界不合理
    if (braceDepth != 0 || bracketDepth != 0) {
        return false;
    }
    
    // 检查下一个字符是否会破坏当前token
    if (nextCharPos < sourceCode_.length()) {
        char nextChar = sourceCode_[nextCharPos];
        
        // 如果最后一个字符是标识符的一部分，检查下一个字符
        if (!slice.empty() && IsIdentifierPart(slice.back()) && IsIdentifierPart(nextChar)) {
            return false;
        }
        
        // 检查CHTL JS特殊语法
        if (slice.length() >= 2) {
            String lastTwo = slice.substr(slice.length() - 2);
            if (lastTwo == "{{" && nextChar != '}') {
                return false;
            }
            if (lastTwo.back() == '-' && nextChar == '>') {
                return false;
            }
        }
    }
    
    return true;
}

SliceCategory UnifiedScanner::IdentifySliceCategory(const String& slice) {
    // 跳过空白
    size_t pos = 0;
    while (pos < slice.length() && IsWhitespace(slice[pos])) {
        pos++;
    }
    
    if (pos >= slice.length()) {
        return SliceCategory::Whitespace;
    }
    
    // 检查注释
    if (pos + 1 < slice.length()) {
        if (slice[pos] == '/' && slice[pos+1] == '/') {
            return SliceCategory::Comment;
        }
        if (slice[pos] == '/' && slice[pos+1] == '*') {
            return SliceCategory::Comment;
        }
        if (slice[pos] == '-' && slice[pos+1] == '-') {
            return SliceCategory::GeneratorComment;
        }
    }
    
    // 检查CHTL块标记
    if (slice[pos] == '[') {
        size_t endPos = slice.find(']', pos);
        if (endPos != String::npos) {
            String marker = slice.substr(pos, endPos - pos + 1);
            if (tokenIdentifier_->IsCHTLBlockMarker(marker)) {
                if (marker == "[Template]") return SliceCategory::CHTLTemplate;
                if (marker == "[Custom]") return SliceCategory::CHTLCustom;
                if (marker == "[Origin]") return SliceCategory::CHTLOrigin;
                if (marker == "[Import]") return SliceCategory::CHTLImport;
                if (marker == "[Namespace]") return SliceCategory::CHTLNamespace;
                if (marker == "[Configuration]") return SliceCategory::CHTLConfiguration;
            }
        }
    }
    
    // 检查CHTL JS选择器
    if (pos + 1 < slice.length() && slice[pos] == '{' && slice[pos+1] == '{') {
        return SliceCategory::CHTLJSSelector;
    }
    
    // 检查CHTL JS箭头
    if (pos + 1 < slice.length() && slice[pos] == '-' && slice[pos+1] == '>') {
        return SliceCategory::CHTLJSArrow;
    }
    
    // 提取第一个标识符
    if (IsIdentifierStart(slice[pos])) {
        size_t tokenEnd = pos + 1;
        while (tokenEnd < slice.length() && IsIdentifierPart(slice[tokenEnd])) {
            tokenEnd++;
        }
        
        String token = slice.substr(pos, tokenEnd - pos);
        
        // 检查CHTL关键字
        if (tokenIdentifier_->IsCHTLKeyword(token)) {
            if (token == "text") return SliceCategory::CHTLText;
            if (token == "style") {
                // 判断是局部还是全局style
                if (currentMode_ == ScanMode::CHTL) {
                    return SliceCategory::CHTLStyle;
                } else {
                    return SliceCategory::PlainCSS;
                }
            }
            if (token == "script") {
                // 判断是局部还是全局script
                if (currentMode_ == ScanMode::CHTL) {
                    return SliceCategory::CHTLScript;
                } else {
                    return SliceCategory::PlainJavaScript;
                }
            }
            return SliceCategory::CHTLElement;
        }
        
        // 检查CHTL JS关键字
        if (tokenIdentifier_->IsCHTLJSKeyword(token)) {
            if (token == "vir") return SliceCategory::CHTLJSVirtual;
            return SliceCategory::CHTLJSFunction;
        }
        
        // 检查是否是HTML元素
        // 这里简化处理，实际应该有完整的HTML元素列表
        return SliceCategory::CHTLElement;
    }
    
    // 根据当前模式判断
    switch (currentMode_) {
        case ScanMode::CSS:
            return SliceCategory::PlainCSS;
        case ScanMode::JavaScript:
        case ScanMode::CHTLJS:
            return SliceCategory::PlainJavaScript;
        default:
            return SliceCategory::Unknown;
    }
}

Result<void> UnifiedScanner::ProcessCHTLSlice(const String& slice, const Position& startPos) {
    // 进行二次切割
    Vector<ScannerSlice> subSlices = SecondarySlice(slice, SliceCategory::CHTLElement, startPos);
    
    for (const auto& subSlice : subSlices) {
        AddSlice(subSlice);
    }
    
    return Ok();
}

Result<void> UnifiedScanner::ProcessCHTLJSSlice(const String& slice, const Position& startPos) {
    // 如果启用双指针扫描
    if (config_.enableDoublePointerScan) {
        auto result = DoublePointerScan(slice, startPos);
        if (result.IsOk()) {
            for (const auto& subSlice : result.Value()) {
                AddSlice(subSlice);
            }
            return Ok();
        }
    }
    
    // 进行二次切割
    Vector<ScannerSlice> subSlices = SecondarySlice(slice, SliceCategory::CHTLJSFunction, startPos);
    
    for (const auto& subSlice : subSlices) {
        AddSlice(subSlice);
    }
    
    return Ok();
}

Result<void> UnifiedScanner::ProcessCSSSlice(const String& slice, const Position& startPos) {
    AddSlice(SliceCategory::PlainCSS, slice, startPos, currentPosition_);
    return Ok();
}

Result<void> UnifiedScanner::ProcessJavaScriptSlice(const String& slice, const Position& startPos) {
    AddSlice(SliceCategory::PlainJavaScript, slice, startPos, currentPosition_);
    return Ok();
}

Vector<ScannerSlice> UnifiedScanner::SecondarySlice(const String& content, 
                                                   SliceCategory category, 
                                                   const Position& startPos) {
    Vector<ScannerSlice> result;
    
    // 这里实现精确的二次切割逻辑
    // 根据CHTL/CHTL JS的最小单元进行切割
    
    size_t pos = 0;
    Position currentPos = startPos;
    
    while (pos < content.length()) {
        // 跳过空白
        size_t nonWhitePos = pos;
        while (nonWhitePos < content.length() && IsWhitespace(content[nonWhitePos])) {
            if (IsNewline(content[nonWhitePos])) {
                currentPos.line++;
                currentPos.column = 1;
            } else {
                currentPos.column++;
            }
            nonWhitePos++;
        }
        
        if (nonWhitePos > pos && config_.preserveWhitespace) {
            ScannerSlice whiteSlice(SliceCategory::Whitespace, 
                                   content.substr(pos, nonWhitePos - pos),
                                   currentPos, currentPos, currentMode_);
            result.push_back(whiteSlice);
        }
        
        pos = nonWhitePos;
        if (pos >= content.length()) break;
        
        // 识别下一个token
        size_t tokenStart = pos;
        Position tokenStartPos = currentPos;
        
        // 处理特殊符号
        if (content[pos] == '{' || content[pos] == '}' || 
            content[pos] == '[' || content[pos] == ']' ||
            content[pos] == '(' || content[pos] == ')' ||
            content[pos] == ';' || content[pos] == ':' ||
            content[pos] == '=' || content[pos] == ',') {
            
            ScannerSlice symbolSlice(category, String(1, content[pos]),
                                   tokenStartPos, currentPos, currentMode_);
            result.push_back(symbolSlice);
            pos++;
            currentPos.column++;
        }
        // 处理标识符
        else if (IsIdentifierStart(content[pos])) {
            size_t tokenEnd = pos + 1;
            while (tokenEnd < content.length() && IsIdentifierPart(content[tokenEnd])) {
                tokenEnd++;
            }
            
            String token = content.substr(pos, tokenEnd - pos);
            SliceCategory tokenCategory = category;
            
            // 细化分类
            if (tokenIdentifier_->IsCHTLKeyword(token)) {
                tokenCategory = SliceCategory::CHTLElement;
            } else if (tokenIdentifier_->IsCHTLJSKeyword(token)) {
                tokenCategory = SliceCategory::CHTLJSFunction;
            }
            
            Position tokenEndPos = currentPos;
            tokenEndPos.column += tokenEnd - pos;
            
            ScannerSlice tokenSlice(tokenCategory, token,
                                   tokenStartPos, tokenEndPos, currentMode_);
            result.push_back(tokenSlice);
            
            pos = tokenEnd;
            currentPos = tokenEndPos;
        }
        // 处理字符串
        else if (content[pos] == '"' || content[pos] == '\'') {
            char quote = content[pos];
            size_t stringEnd = pos + 1;
            
            while (stringEnd < content.length()) {
                if (content[stringEnd] == quote && 
                    (stringEnd == pos + 1 || content[stringEnd - 1] != '\\')) {
                    stringEnd++;
                    break;
                }
                stringEnd++;
            }
            
            String str = content.substr(pos, stringEnd - pos);
            Position stringEndPos = currentPos;
            stringEndPos.column += stringEnd - pos;
            
            ScannerSlice stringSlice(category, str,
                                    tokenStartPos, stringEndPos, currentMode_);
            result.push_back(stringSlice);
            
            pos = stringEnd;
            currentPos = stringEndPos;
        }
        else {
            // 其他字符
            pos++;
            currentPos.column++;
        }
    }
    
    return result;
}

Result<Vector<ScannerSlice>> UnifiedScanner::DoublePointerScan(const String& content, 
                                                              const Position& startPos) {
    Vector<ScannerSlice> result;
    
    // 实现双指针扫描逻辑
    doublePointer_.back = 0;
    doublePointer_.front = 0;
    doublePointer_.collecting = false;
    
    while (doublePointer_.front < content.length()) {
        // 预扫描，查找关键字
        size_t keywordPos = String::npos;
        String foundKeyword;
        
        // 查找CHTL JS特殊语法
        size_t nextSelector = content.find("{{", doublePointer_.front);
        size_t nextArrow = content.find("->", doublePointer_.front);
        
        if (nextSelector != String::npos && 
            (nextArrow == String::npos || nextSelector < nextArrow)) {
            keywordPos = nextSelector;
            foundKeyword = "{{";
        } else if (nextArrow != String::npos) {
            keywordPos = nextArrow;
            foundKeyword = "->";
        }
        
        if (keywordPos != String::npos) {
            // 收集关键字前的内容
            if (keywordPos > doublePointer_.back) {
                String beforeContent = content.substr(doublePointer_.back, 
                                                    keywordPos - doublePointer_.back);
                if (!beforeContent.empty()) {
                    ScannerSlice beforeSlice(SliceCategory::PlainJavaScript, 
                                           beforeContent, startPos, startPos, 
                                           ScanMode::JavaScript);
                    result.push_back(beforeSlice);
                }
            }
            
            // 处理关键字
            if (foundKeyword == "{{") {
                // 查找匹配的}}
                size_t endPos = content.find("}}", keywordPos + 2);
                if (endPos != String::npos) {
                    String selector = content.substr(keywordPos, endPos + 2 - keywordPos);
                    ScannerSlice selectorSlice(SliceCategory::CHTLJSSelector, 
                                             selector, startPos, startPos, 
                                             ScanMode::CHTLJS);
                    result.push_back(selectorSlice);
                    doublePointer_.front = endPos + 2;
                    doublePointer_.back = doublePointer_.front;
                } else {
                    doublePointer_.front = content.length();
                }
            } else if (foundKeyword == "->") {
                // 处理箭头
                ScannerSlice arrowSlice(SliceCategory::CHTLJSArrow, 
                                       "->", startPos, startPos, 
                                       ScanMode::CHTLJS);
                result.push_back(arrowSlice);
                doublePointer_.front = keywordPos + 2;
                doublePointer_.back = doublePointer_.front;
            }
        } else {
            // 没有更多关键字，收集剩余内容
            if (doublePointer_.back < content.length()) {
                String remainingContent = content.substr(doublePointer_.back);
                if (!remainingContent.empty()) {
                    ScannerSlice remainingSlice(SliceCategory::PlainJavaScript, 
                                              remainingContent, startPos, startPos, 
                                              ScanMode::JavaScript);
                    result.push_back(remainingSlice);
                }
            }
            doublePointer_.front = content.length();
        }
    }
    
    return Ok(result);
}

void UnifiedScanner::UpdatePosition(char ch) {
    currentPosition_.offset++;
    if (ch == '\n') {
        currentPosition_.line++;
        currentPosition_.column = 1;
    } else if (ch == '\t') {
        currentPosition_.column += 4; // 假设tab为4个空格
    } else {
        currentPosition_.column++;
    }
}

bool UnifiedScanner::IsWhitespace(char ch) const {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

bool UnifiedScanner::IsNewline(char ch) const {
    return ch == '\n' || ch == '\r';
}

bool UnifiedScanner::IsIdentifierStart(char ch) const {
    return std::isalpha(ch) || ch == '_' || ch == '$' || ch == '@';
}

bool UnifiedScanner::IsIdentifierPart(char ch) const {
    return std::isalnum(ch) || ch == '_' || ch == '$' || ch == '-';
}

void UnifiedScanner::AddSlice(const ScannerSlice& slice) {
    slices_.push_back(slice);
    sliceManager_->AddSlice(slice);
}

void UnifiedScanner::AddSlice(SliceCategory category, const String& content, 
                            const Position& start, const Position& end) {
    ScannerSlice slice(category, content, start, end, currentMode_);
    AddSlice(slice);
}

void UnifiedScanner::MergeAdjacentSlices() {
    sliceManager_->MergeSlices([](const ScannerSlice& a, const ScannerSlice& b) {
        // 合并条件：
        // 1. 相同类别
        // 2. 相同模式
        // 3. 位置相邻
        // 4. 不是特殊类别（如CHTL关键字等）
        if (a.category != b.category || a.mode != b.mode) {
            return false;
        }
        
        if (a.end.offset != b.start.offset) {
            return false;
        }
        
        // 某些类别不应该合并
        switch (a.category) {
            case SliceCategory::CHTLElement:
            case SliceCategory::CHTLAttribute:
            case SliceCategory::CHTLJSSelector:
            case SliceCategory::CHTLJSArrow:
            case SliceCategory::CHTLJSFunction:
                return false;
            default:
                return true;
        }
    });
    
    // 更新主切片列表
    slices_ = sliceManager_->GetAllSlices();
}

void UnifiedScanner::PushMode(ScanMode mode) {
    modeStack_.push(currentMode_);
    currentMode_ = mode;
}

void UnifiedScanner::PopMode() {
    if (!modeStack_.empty()) {
        currentMode_ = modeStack_.top();
        modeStack_.pop();
    }
}

} // namespace CHTL