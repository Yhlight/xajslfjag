#include "CJMODGenerator.h"
#include <iostream>

namespace CHTL {
namespace CJMOD {

std::ostream* CJMODGenerator::outputStream_ = &std::cout;

void CJMODGenerator::exportResult(const Arg& args) {
    // 导出转换后的结果
    if (outputStream_) {
        for (const auto& item : args.getItems()) {
            *outputStream_ << item.value;
        }
        *outputStream_ << std::endl;
    }
}

std::string CJMODGenerator::generate(const Arg& args, const std::string& templateCode) {
    return replacePlaceholders(templateCode, args);
}

void CJMODGenerator::setOutputStream(std::ostream* stream) {
    outputStream_ = stream;
}

std::string CJMODGenerator::replacePlaceholders(const std::string& templateCode, 
                                               const Arg& args) {
    std::string result = templateCode;
    
    // 替换所有的arg[n].value占位符
    for (size_t i = 0; i < args.size(); ++i) {
        std::string placeholder = "arg[" + std::to_string(i) + "].value";
        std::string value = args[i].value;
        
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }
    
    return result;
}

} // namespace CJMOD
} // namespace CHTL