#ifndef CHTL_H
#define CHTL_H

// Main CHTL library header - includes all necessary components

#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/CHTLNode.h"
#include "CHTLState/CHTLState.h"
#include "CHTLContext/CHTLContext.h"
#include "CHTLContext/GlobalMap.h"
#include "CHTLLoader/CHTLLoader.h"
#include "CHTLManage/CHTLManager.h"

namespace CHTL {

// Version information
constexpr const char* VERSION = "1.0.0";
constexpr int VERSION_MAJOR = 1;
constexpr int VERSION_MINOR = 0;
constexpr int VERSION_PATCH = 0;

// Simple compilation interface
struct CompileOptions {
    bool pretty_print = true;
    bool debug_mode = false;
    bool strict_mode = true;
    std::vector<std::string> include_paths;
};

// Simple API for quick compilation
std::string compileToHTML(const std::string& chtl_source, const CompileOptions& options = {});
std::string compileToCSS(const std::string& chtl_source, const CompileOptions& options = {});
std::string compileToJS(const std::string& chtl_source, const CompileOptions& options = {});

struct CompileResult {
    bool success;
    std::string html;
    std::string css;
    std::string js;
    std::vector<std::string> errors;
};

CompileResult compile(const std::string& chtl_source, const CompileOptions& options = {});
CompileResult compileFile(const std::string& filepath, const CompileOptions& options = {});

// Version functions
const char* getVersion();
int getVersionMajor();
int getVersionMinor();
int getVersionPatch();

} // namespace CHTL

#endif // CHTL_H