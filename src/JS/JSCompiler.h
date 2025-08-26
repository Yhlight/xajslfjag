#pragma once

#include <string>
#include <memory>
#include "Common/Error.h"

namespace CHTL {

class CodeSlice;

// JavaScript编译器包装类
class JSCompiler {
public:
    JSCompiler();
    ~JSCompiler();
    
    // 编译JavaScript代码片段
    std::string Compile(const std::shared_ptr<CodeSlice>& slice, ErrorCollector& errorCollector);
    
    // 验证JavaScript语法
    bool Validate(const std::string& js, ErrorCollector& errorCollector);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace CHTL