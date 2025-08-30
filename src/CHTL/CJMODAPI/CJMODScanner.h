#ifndef CHTL_CJMOD_SCANNER_H
#define CHTL_CJMOD_SCANNER_H

#include <string>
#include "Arg.h"

namespace CHTL {
namespace CJMOD {

// CJMOD扫描器
class CJMODScanner {
public:
    // 扫描代码，提取匹配的值
    static Arg scan(const Arg& pattern, const std::string& code);
    
    // 扫描并验证
    static bool scanAndValidate(const Arg& pattern, const std::string& code);
    
private:
    // 匹配单个模式项
    static bool matchPatternItem(const std::string& patternItem, 
                                const std::string& code, 
                                size_t& pos,
                                std::string& matchedValue);
    
    // 提取匹配的值
    static std::string extractValue(const std::string& code, 
                                   size_t start, 
                                   size_t end);
};

} // namespace CJMOD
} // namespace CHTL

#endif // CHTL_CJMOD_SCANNER_H