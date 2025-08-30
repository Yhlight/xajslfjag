#pragma once

#include <string>
#include <vector>

namespace CJMOD {

// CJMOD生成结果
struct CJMODGenerationResult {
    bool success = false;
    std::string javascript;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

// CJMOD代码生成器
class CJMODGenerator {
public:
    CJMODGenerator();
    
    // 从语法生成JavaScript代码
    CJMODGenerationResult generateFromSyntax(const std::string& input);
    
    // 处理输入并返回结果
    std::string processInput(const std::string& input);
};

}  // namespace CJMOD