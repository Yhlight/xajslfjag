#ifndef CHTL_CJMOD_GENERATOR_H
#define CHTL_CJMOD_GENERATOR_H

#include <string>
#include <sstream>
#include "Arg.h"

namespace CHTL {
namespace CJMOD {

// CJMOD生成器
class CJMODGenerator {
public:
    // 导出结果
    static void exportResult(const Arg& args);
    
    // 生成转换后的代码
    static std::string generate(const Arg& args, const std::string& templateCode);
    
    // 设置输出流
    static void setOutputStream(std::ostream* stream);
    
private:
    static std::ostream* outputStream_;
    
    // 替换模板中的占位符
    static std::string replacePlaceholders(const std::string& templateCode, 
                                         const Arg& args);
};

} // namespace CJMOD
} // namespace CHTL

#endif // CHTL_CJMOD_GENERATOR_H