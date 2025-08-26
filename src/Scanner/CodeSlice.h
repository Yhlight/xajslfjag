#pragma once

#include <string>
#include <memory>
#include "Common/Location.h"

namespace CHTL {

// 代码片段类型
enum class SliceType {
    UNKNOWN,
    CHTL,         // CHTL代码片段
    CHTL_JS,      // CHTL JS代码片段
    CSS,          // CSS代码片段
    JAVASCRIPT,   // JavaScript代码片段
    HTML,         // 原始HTML片段
    TEXT          // 纯文本片段
};

// 代码片段上下文
enum class SliceContext {
    GLOBAL,              // 全局上下文
    ELEMENT_BODY,        // 元素体内
    LOCAL_STYLE,         // 局部style块内
    LOCAL_SCRIPT,        // 局部script块内
    GLOBAL_STYLE,        // 全局style块内
    GLOBAL_SCRIPT,       // 全局script块内
    TEMPLATE_DEFINITION, // 模板定义内
    CUSTOM_DEFINITION,   // 自定义定义内
    CONFIGURATION,       // 配置块内
    ORIGIN_BLOCK         // 原始嵌入块内
};

// 代码切片
class CodeSlice {
public:
    CodeSlice(SliceType type, const std::string& content, 
              const Location& startLoc, const Location& endLoc,
              SliceContext context = SliceContext::GLOBAL)
        : type_(type), content_(content), 
          startLocation_(startLoc), endLocation_(endLoc),
          context_(context) {}
    
    SliceType GetType() const { return type_; }
    const std::string& GetContent() const { return content_; }
    const Location& GetStartLocation() const { return startLocation_; }
    const Location& GetEndLocation() const { return endLocation_; }
    SliceContext GetContext() const { return context_; }
    
    void SetType(SliceType type) { type_ = type; }
    void SetContext(SliceContext context) { context_ = context; }
    
    // 获取切片大小
    size_t GetSize() const { return content_.size(); }
    
    // 检查是否为特定类型
    bool IsCHTL() const { return type_ == SliceType::CHTL; }
    bool IsCHTLJS() const { return type_ == SliceType::CHTL_JS; }
    bool IsCSS() const { return type_ == SliceType::CSS; }
    bool IsJavaScript() const { return type_ == SliceType::JAVASCRIPT; }
    
    // 合并相邻的切片（如果类型相同）
    bool CanMergeWith(const CodeSlice& other) const {
        return type_ == other.type_ && 
               context_ == other.context_ &&
               endLocation_.GetOffset() == other.startLocation_.GetOffset();
    }
    
    void MergeWith(const CodeSlice& other) {
        if (CanMergeWith(other)) {
            content_ += other.content_;
            endLocation_ = other.endLocation_;
        }
    }
    
    std::string ToString() const;
    
private:
    SliceType type_;
    std::string content_;
    Location startLocation_;
    Location endLocation_;
    SliceContext context_;
};

using CodeSlicePtr = std::shared_ptr<CodeSlice>;

} // namespace CHTL