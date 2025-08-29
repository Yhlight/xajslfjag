#ifndef FRAGMENT_COLLECTOR_H
#define FRAGMENT_COLLECTOR_H

#include "CHTLUnifiedScanner.h"
#include <map>
#include <memory>

namespace CHTL {

// 片段收集器 - 负责将同类型片段组合成完整代码
class FragmentCollector {
public:
    // 片段流
    class FragmentStream {
    public:
        FragmentStream(FragmentType type) : type_(type) {}
        
        // 添加片段
        void addFragment(const CodeFragment& fragment) {
            fragments_.push_back(fragment);
            if (!content_.empty() && needsNewline(fragment)) {
                content_ += "\n";
            }
            content_ += fragment.getContent();
        }
        
        // 获取完整内容
        std::string getCompleteContent() const { return content_; }
        
        // 获取片段类型
        FragmentType getType() const { return type_; }
        
        // 是否有内容
        bool hasContent() const { return !content_.empty(); }
        
        // 清空
        void clear() {
            fragments_.clear();
            content_.clear();
        }
        
    private:
        FragmentType type_;
        std::vector<CodeFragment> fragments_;
        std::string content_;
        
        // 判断是否需要添加换行
        bool needsNewline(const CodeFragment& fragment) {
            if (fragments_.empty()) return false;
            
            // CSS和JS片段之间需要换行以保持结构
            if (type_ == FragmentType::CSS || type_ == FragmentType::JavaScript) {
                return true;
            }
            
            return false;
        }
    };
    
    FragmentCollector() {
        // 初始化各类型的流
        streams_[FragmentType::CHTL] = std::make_unique<FragmentStream>(FragmentType::CHTL);
        streams_[FragmentType::CHTLJS] = std::make_unique<FragmentStream>(FragmentType::CHTLJS);
        streams_[FragmentType::CSS] = std::make_unique<FragmentStream>(FragmentType::CSS);
        streams_[FragmentType::JavaScript] = std::make_unique<FragmentStream>(FragmentType::JavaScript);
    }
    
    // 处理片段
    void processFragment(const CodeFragment& fragment) {
        auto it = streams_.find(fragment.getType());
        if (it != streams_.end()) {
            it->second->addFragment(fragment);
        }
    }
    
    // 处理片段列表
    void processFragments(const std::vector<CodeFragment>& fragments) {
        for (const auto& fragment : fragments) {
            processFragment(fragment);
        }
    }
    
    // 获取特定类型的完整代码
    std::string getCompleteCode(FragmentType type) const {
        auto it = streams_.find(type);
        return it != streams_.end() ? it->second->getCompleteContent() : "";
    }
    
    // 获取CSS完整代码
    std::string getCompleteCSS() const {
        return getCompleteCode(FragmentType::CSS);
    }
    
    // 获取JavaScript完整代码
    std::string getCompleteJavaScript() const {
        return getCompleteCode(FragmentType::JavaScript);
    }
    
    // 检查是否有特定类型的内容
    bool hasContent(FragmentType type) const {
        auto it = streams_.find(type);
        return it != streams_.end() && it->second->hasContent();
    }
    
    // 清空所有流
    void clear() {
        for (auto& [type, stream] : streams_) {
            stream->clear();
        }
    }
    
private:
    std::map<FragmentType, std::unique_ptr<FragmentStream>> streams_;
};

} // namespace CHTL

#endif // FRAGMENT_COLLECTOR_H