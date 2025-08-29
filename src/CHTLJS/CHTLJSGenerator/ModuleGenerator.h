#ifndef CHTLJS_MODULE_GENERATOR_H
#define CHTLJS_MODULE_GENERATOR_H

#include <string>
#include <memory>
#include <map>
#include "../CHTLJSNode/ModuleNode.h"

namespace CHTLJS {

// 模块生成器 - 负责生成AMD/CommonJS/ES6模块
class ModuleGenerator {
public:
    enum class ModuleFormat {
        AMD,        // define(['dep1', 'dep2'], function(dep1, dep2) { ... })
        COMMONJS,   // require/exports
        ES6         // import/export
    };
    
    ModuleGenerator(ModuleFormat format = ModuleFormat::AMD);
    ~ModuleGenerator() = default;
    
    // 生成模块代码
    std::string generate(const ModuleNode* node);
    
    // 设置模块格式
    void setFormat(ModuleFormat format) { format_ = format; }
    ModuleFormat getFormat() const { return format_; }
    
private:
    ModuleFormat format_;
    
    // 生成不同格式的模块
    std::string generateAMD(const ModuleNode* node);
    std::string generateCommonJS(const ModuleNode* node);
    std::string generateES6(const ModuleNode* node);
    
    // 辅助方法
    std::string generateDependencyList(const std::vector<std::string>& deps);
    std::string generateExports(const std::map<std::string, std::string>& exports);
};

} // namespace CHTLJS

#endif // CHTLJS_MODULE_GENERATOR_H