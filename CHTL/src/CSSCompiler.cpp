#include "CSSCompiler.h"
#include "antlr4-runtime.h"
#include "css/css3Lexer.h"
#include "css/css3Parser.h"
#include <iostream>

namespace CHTL {

std::string CSSCompiler::Compile(const std::string& source) {
    antlr4::ANTLRInputStream input(source);
    css3Lexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    css3Parser parser(&tokens);

    // Optional: Add error listeners for better diagnostics
    // parser.removeErrorListeners();
    // parser.addErrorListener(...);

    try {
        // Start parsing from the main entry rule
        parser.stylesheet();
        // If no exception is thrown, the CSS is syntactically valid.
        std::cout << "CSS compilation successful." << std::endl;
    } catch (const antlr4::ParseCancellationException& e) {
        std::cerr << "CSS Compilation Error: " << e.what() << std::endl;
        return ""; // Return empty string on error
    }

    // For now, we just validate and pass the source through.
    return source;
}

}
