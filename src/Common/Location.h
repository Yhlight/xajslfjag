#pragma once

#include <string>

namespace CHTL {

// 源代码位置信息
class Location {
public:
    Location() : line_(0), column_(0), offset_(0) {}
    
    Location(size_t line, size_t column, size_t offset, const std::string& filename = "")
        : line_(line), column_(column), offset_(offset), filename_(filename) {}
    
    size_t GetLine() const { return line_; }
    size_t GetColumn() const { return column_; }
    size_t GetOffset() const { return offset_; }
    const std::string& GetFilename() const { return filename_; }
    
    void SetLine(size_t line) { line_ = line; }
    void SetColumn(size_t column) { column_ = column; }
    void SetOffset(size_t offset) { offset_ = offset; }
    void SetFilename(const std::string& filename) { filename_ = filename; }
    
    // 前进一个字符
    void Advance(char ch) {
        if (ch == '\n') {
            line_++;
            column_ = 0;
        } else {
            column_++;
        }
        offset_++;
    }
    
    std::string ToString() const;
    
private:
    size_t line_;      // 行号（从1开始）
    size_t column_;    // 列号（从0开始）
    size_t offset_;    // 文件偏移量
    std::string filename_;
};

// 范围位置（起始和结束）
class Range {
public:
    Range() = default;
    Range(const Location& start, const Location& end)
        : start_(start), end_(end) {}
    
    const Location& GetStart() const { return start_; }
    const Location& GetEnd() const { return end_; }
    
    void SetStart(const Location& start) { start_ = start; }
    void SetEnd(const Location& end) { end_ = end; }
    
    std::string ToString() const;
    
private:
    Location start_;
    Location end_;
};

} // namespace CHTL