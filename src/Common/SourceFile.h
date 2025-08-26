#pragma once

#include <string>
#include <memory>
#include <vector>

namespace CHTL {

// 源文件管理
class SourceFile {
public:
    SourceFile(const std::string& filename, const std::string& content)
        : filename_(filename), content_(content) {
        BuildLineMap();
    }
    
    const std::string& GetFilename() const { return filename_; }
    const std::string& GetContent() const { return content_; }
    
    // 获取指定行的内容
    std::string GetLine(size_t lineNumber) const {
        if (lineNumber == 0 || lineNumber > lineOffsets_.size()) {
            return "";
        }
        
        size_t start = lineOffsets_[lineNumber - 1];
        size_t end = (lineNumber < lineOffsets_.size()) 
            ? lineOffsets_[lineNumber] - 1 
            : content_.length();
        
        // 去除换行符
        if (end > start && content_[end - 1] == '\n') {
            end--;
        }
        if (end > start && content_[end - 1] == '\r') {
            end--;
        }
        
        return content_.substr(start, end - start);
    }
    
    // 获取行数
    size_t GetLineCount() const { return lineOffsets_.size(); }
    
    // 从偏移量获取行列信息
    void GetLineColumn(size_t offset, size_t& line, size_t& column) const {
        line = 1;
        column = 0;
        
        for (size_t i = 1; i < lineOffsets_.size(); ++i) {
            if (offset < lineOffsets_[i]) {
                line = i;
                column = offset - lineOffsets_[i - 1];
                return;
            }
        }
        
        // 最后一行
        if (!lineOffsets_.empty()) {
            line = lineOffsets_.size();
            column = offset - lineOffsets_.back();
        }
    }
    
private:
    void BuildLineMap() {
        lineOffsets_.push_back(0);  // 第一行从0开始
        
        for (size_t i = 0; i < content_.length(); ++i) {
            if (content_[i] == '\n') {
                lineOffsets_.push_back(i + 1);
            }
        }
    }
    
    std::string filename_;
    std::string content_;
    std::vector<size_t> lineOffsets_;  // 每行的起始偏移量
};

using SourceFilePtr = std::shared_ptr<SourceFile>;

} // namespace CHTL