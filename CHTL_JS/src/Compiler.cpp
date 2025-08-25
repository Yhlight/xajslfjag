#include "CHTL_JS/Compiler.h"
#include "CHTL_JS/Parser/Parser.h"
#include "CHTL_JS/Generator/Generator.h"
#include "CHTL_JS/Lexer/Lexer.h"

namespace CHTL_JS {

Compiler::Compiler(const CHTL::Config& config) : m_Config(config) {}

std::string Compiler::Compile(const std::string& source) {
    // This is for compiling a pure CHTL_JS slice
    Lexer lexer(source, m_Config);
    auto tokens = lexer.ScanTokens();
    Parser parser(tokens, m_Config);
    auto ast = parser.Parse();
    Generator generator(ast);
    return generator.Generate();
}

std::string Compiler::ProcessJsString(const std::string& js) {
    // This is for pre-processing a JS slice that might contain CHTL_JS syntax
    // The logic is essentially the same as a full compile for now.
    return Compile(js);
}

}
