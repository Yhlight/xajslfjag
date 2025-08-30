#include "ModuleGenerator.h"
#include "../../Error/ErrorReport.h"
#include <sstream>

namespace CHTLJS {

ModuleGenerator::ModuleGenerator(ModuleFormat format) : format_(format) {}

std::string ModuleGenerator::generate(const ModuleNode* node) {
    if (!node) return "";
    
    switch (format_) {
        case ModuleFormat::AMD:
            return generateAMD(node);
        case ModuleFormat::COMMONJS:
            return generateCommonJS(node);
        case ModuleFormat::ES6:
            return generateES6(node);
        default:
            return generateAMD(node);
    }
}

std::string ModuleGenerator::generateAMD(const ModuleNode* node) {
    std::stringstream output;
    const auto& deps = node->getLoadItems();
    
    // define(['dep1', 'dep2'], function(dep1, dep2) {
    output << "define([";
    
    // 依赖列表
    for (size_t i = 0; i < deps.size(); ++i) {
        output << "'" << deps[i] << "'";
        if (i < deps.size() - 1) output << ", ";
    }
    
    output << "], function(";
    
    // 参数列表
    for (size_t i = 0; i < deps.size(); ++i) {
        output << ModuleNode::toVariableName(deps[i]);
        if (i < deps.size() - 1) output << ", ";
    }
    
    output << ") {\n";
    
    // TODO: 生成模块内容
    output << "  // Module content\n";
    
    // 生成导出
    if (node->hasExports()) {
        output << "\n  return " << generateExports(node->getExports());
    }
    
    output << "});\n";
    
    return output.str();
}

std::string ModuleGenerator::generateCommonJS(const ModuleNode* node) {
    std::stringstream output;
    const auto& deps = node->getLoadItems();
    
    // 生成require语句
    for (const auto& dep : deps) {
        std::string varName = ModuleNode::toVariableName(dep);
        output << "const " << varName << " = require('" << dep << "');\n";
    }
    
    if (!deps.empty()) {
        output << "\n";
    }
    
    // TODO: 生成模块内容
    output << "// Module content\n\n";
    
    // 生成exports
    if (node->hasExports()) {
        for (const auto& [name, value] : node->getExports()) {
            output << "module.exports." << name << " = " << value << ";\n";
        }
    }
    
    return output.str();
}

std::string ModuleGenerator::generateES6(const ModuleNode* node) {
    std::stringstream output;
    const auto& deps = node->getLoadItems();
    
    // 生成import语句
    for (const auto& dep : deps) {
        if (ModuleNode::isRelativePath(dep)) {
            // 相对路径：import * as name from './path'
            std::string varName = ModuleNode::toVariableName(dep);
            output << "import * as " << varName << " from '" << dep << "';\n";
        } else {
            // 默认导入：import package from 'package'
            output << "import " << dep << " from '" << dep << "';\n";
        }
    }
    
    if (!deps.empty()) {
        output << "\n";
    }
    
    // TODO: 生成模块内容
    output << "// Module content\n\n";
    
    // 生成export语句
    if (node->hasExports()) {
        const auto& exports = node->getExports();
        if (exports.size() == 1 && exports.begin()->first == "default") {
            // 默认导出
            output << "export default " << exports.begin()->second << ";\n";
        } else {
            // 命名导出
            output << "export {\n";
            for (const auto& [name, value] : exports) {
                output << "  " << name;
                if (name != value) {
                    output << " as " << value;
                }
                output << ",\n";
            }
            output << "};\n";
        }
    }
    
    return output.str();
}

std::string ModuleGenerator::generateDependencyList(const std::vector<std::string>& deps) {
    std::stringstream output;
    output << "[";
    for (size_t i = 0; i < deps.size(); ++i) {
        output << "'" << deps[i] << "'";
        if (i < deps.size() - 1) output << ", ";
    }
    output << "]";
    return output.str();
}

std::string ModuleGenerator::generateExports(const std::map<std::string, std::string>& exports) {
    std::stringstream output;
    output << "{\n";
    for (const auto& [name, value] : exports) {
        output << "    " << name << ": " << value << ",\n";
    }
    output << "  }";
    return output.str();
}

} // namespace CHTLJS