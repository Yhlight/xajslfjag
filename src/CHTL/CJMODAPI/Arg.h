#ifndef CHTL_CJMOD_ARG_H
#define CHTL_CJMOD_ARG_H

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace CHTL {
namespace CJMOD {

// 参数项
struct ArgItem {
    std::string pattern;    // 原始模式（如 "$", "**"）
    std::string value;      // 实际值
    size_t position;        // 在原始模式中的位置
    
    ArgItem(const std::string& p, size_t pos) 
        : pattern(p), position(pos) {}
};

// 参数列表类
class Arg {
public:
    Arg() = default;
    
    // 添加参数项
    void addItem(const ArgItem& item);
    
    // 绑定值获取函数
    void bind(const std::string& pattern, 
              std::function<std::string(const std::string&)> getter);
    
    // 填充值
    void fillValue(const Arg& scannedResult);
    
    // 转换为目标代码
    std::string transform(const std::string& templateCode);
    
    // 打印参数列表
    void print() const;
    
    // 获取参数项
    const std::vector<ArgItem>& getItems() const { return items_; }
    ArgItem& operator[](size_t index) { return items_[index]; }
    const ArgItem& operator[](size_t index) const { return items_[index]; }
    
    // 获取大小
    size_t size() const { return items_.size(); }
    
private:
    std::vector<ArgItem> items_;
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> binders_;
};

} // namespace CJMOD
} // namespace CHTL

#endif // CHTL_CJMOD_ARG_H