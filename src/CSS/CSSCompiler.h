#pragma once

#include <string>
#include <memory>
#include "Common/Error.h"

namespace CHTL {

class CodeSlice;

// CSS编译器包装类
class CSSCompiler {
public:
    CSSCompiler();
    ~CSSCompiler();
    
    // 编译CSS代码片段
    std::string Compile(const std::shared_ptr<CodeSlice>& slice, ErrorCollector& errorCollector);
    
    // 验证CSS语法
    bool Validate(const std::string& css, ErrorCollector& errorCollector);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace CHTL