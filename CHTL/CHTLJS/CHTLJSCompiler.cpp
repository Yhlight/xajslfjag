#include "CHTLJSCompiler.h"
#include "CHTLJSLexer/Lexer.h"
#include "CHTLJSLexer/GlobalMap.h"
#include "CHTLJSParser.h"
#include "CHTLJSGenerator.h"
#include <iostream>
#include <sstream>

namespace CHTLJS {

class CHTLJSCompiler::Impl {
public:
    std::shared_ptr<GlobalMap> globalMap;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    bool debugMode;
    
    Impl() : debugMode(false) {
        globalMap = std::make_shared<GlobalMap>();
    }
};

CHTLJSCompiler::CHTLJSCompiler() : pImpl(std::make_unique<Impl>()) {
}

CHTLJSCompiler::~CHTLJSCompiler() = default;

std::string CHTLJSCompiler::Compile(const std::string& source) {
    pImpl->errors.clear();
    pImpl->warnings.clear();
    
    try {
        // 创建词法分析器
        auto lexer = std::make_shared<Lexer>(source, pImpl->globalMap);
        lexer->SetDebugMode(pImpl->debugMode);
        
        // 创建解析器
        CHTLJSParser parser(lexer, pImpl->globalMap);
        parser.SetDebugMode(pImpl->debugMode);
        
        // 解析
        auto ast = parser.Parse();
        
        // 检查解析错误
        if (parser.HasErrors()) {
            pImpl->errors = parser.GetErrors();
            return "";
        }
        
        // 生成JavaScript代码
        CHTLJSGenerator generator(pImpl->globalMap);
        std::string result = generator.Generate(ast);
        
        if (pImpl->debugMode) {
            std::cout << "CHTL JS编译完成，生成 " << generator.GetLineCount() 
                     << " 行代码" << std::endl;
        }
        
        return result;
        
    } catch (const std::exception& e) {
        pImpl->errors.push_back("CHTL JS编译错误: " + std::string(e.what()));
        return "";
    }
}

void CHTLJSCompiler::SetDebugMode(bool debug) {
    pImpl->debugMode = debug;
}

bool CHTLJSCompiler::HasErrors() const {
    return !pImpl->errors.empty();
}

std::vector<std::string> CHTLJSCompiler::GetErrors() const {
    return pImpl->errors;
}

std::vector<std::string> CHTLJSCompiler::GetWarnings() const {
    return pImpl->warnings;
}

void CHTLJSCompiler::Reset() {
    pImpl->globalMap->Clear();
    pImpl->errors.clear();
    pImpl->warnings.clear();
}

} // namespace CHTLJS