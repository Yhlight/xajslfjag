#include <iostream>
#include <fstream>
#include <sstream>
#include "CHTL/Core/Config.h"
#include "UnifiedScanner/CHTLUnifiedScanner.h"
#include "CHTL/CompilerDispatcher.h"

// Forward declarations for bootstrap functions
CHTL::Config BootstrapConfig(int argc, char* argv[]);
std::string ReadSourceFile(const char* path);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: chtl <filename> [--config <config_path>]" << std::endl;
        return 1;
    }

    // 1. Bootstrap Phase: Determine configuration from arguments or `use` directives.
    // This is a placeholder for a more robust argument/`use` parsing logic.
    CHTL::Config config = BootstrapConfig(argc, argv);
    std::string source = ReadSourceFile(argv[1]);

    // 2. Scanning Phase: Cut the source code into slices of different languages.
    CHTL::CHTLUnifiedScanner scanner(source, config);
    std::vector<CHTL::CodeSlice> slices = scanner.Scan();

    // 3. Dispatching Phase: Send slices to their respective compilers.
    CHTL::CompilerDispatcher dispatcher(config);
    CHTL::FinalOutput result = dispatcher.Dispatch(slices);

    // 4. Assembly Phase: Combine the results into a final output.
    if (result.EmitHtml5Doctype) {
        std::cout << "<!DOCTYPE html>" << std::endl;
    }
    std::cout << result.Html;
    if (!result.Css.empty()) {
        std::cout << "<style>" << result.Css << "</style>";
    }
    if (!result.Js.empty()) {
        std::cout << "<script>" << result.Js << "</script>";
    }
    std::cout << std::endl;

    return 0;
}

// --- Placeholder Implementations ---
std::string ReadSourceFile(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) { /* ... error handling ... */ return ""; }
    std::stringstream buffer; buffer << file.rdbuf();
    return buffer.str();
}

CHTL::Config BootstrapConfig(int argc, char* argv[]) {
    // This function would parse argv for --config flags
    // or do a pre-scan of the source for `use @Config`.
    return CHTL::Config(); // Return default for now.
}
