#pragma once

#include <string>
#include <vector>

namespace CHTL {

class CSSCompiler {
public:
    CSSCompiler();
    ~CSSCompiler();
    
    // 编译CSS代码
    std::string Compile(const std::string& source);
    
    // 错误处理
    bool HasErrors() const { return !errors.empty(); }
    std::vector<std::string> GetErrors() const { return errors; }
    void ClearErrors() { errors.clear(); }
    
private:
    std::vector<std::string> errors;
};

} // namespace CHTL