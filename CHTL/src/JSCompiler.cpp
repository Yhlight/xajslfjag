#include "JSCompiler.h"
#include "antlr4-runtime.h"
#include "js/JavaScriptLexer.h"
#include "js/JavaScriptParser.h"
#include <iostream>

namespace CHTL {

std::string JSCompiler::Compile(const std::string& source) {
    antlr4::ANTLRInputStream input(source);
    JavaScriptLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    JavaScriptParser parser(&tokens);

    try {
        // Start parsing from the main entry rule
        parser.program();
        std::cout << "JavaScript compilation successful." << std::endl;
    } catch (const antlr4::ParseCancellationException& e) {
        std::cerr << "JavaScript Compilation Error: " << e.what() << std::endl;
        return ""; // Return empty string on error
    }

    // For now, we just validate and pass the source through.
    return source;
}

}
