#include "Arg.h"
#include <iostream>

namespace CHTL {
namespace CJMOD {

void Arg::addItem(const ArgItem& item) {
    items_.push_back(item);
}

void Arg::bind(const std::string& pattern, 
               std::function<std::string(const std::string&)> getter) {
    binders_[pattern] = getter;
}

void Arg::fillValue(const Arg& scannedResult) {
    if (scannedResult.size() != items_.size()) {
        std::cerr << "Warning: Scanned result size doesn't match pattern size" << std::endl;
        return;
    }
    
    for (size_t i = 0; i < items_.size() && i < scannedResult.size(); ++i) {
        items_[i].value = scannedResult[i].value;
        
        // 如果有绑定的getter函数，调用它
        auto it = binders_.find(items_[i].pattern);
        if (it != binders_.end()) {
            items_[i].value = it->second(items_[i].value);
        }
    }
}

std::string Arg::transform(const std::string& templateCode) {
    std::string result = templateCode;
    
    // 替换占位符
    for (const auto& item : items_) {
        std::string placeholder = "arg[" + std::to_string(item.position) + "].value";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), item.value);
            pos += item.value.length();
        }
    }
    
    return result;
}

void Arg::print() const {
    std::cout << "[";
    for (size_t i = 0; i < items_.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << items_[i].pattern << "\"";
        if (!items_[i].value.empty()) {
            std::cout << " => \"" << items_[i].value << "\"";
        }
    }
    std::cout << "]" << std::endl;
}

} // namespace CJMOD
} // namespace CHTL