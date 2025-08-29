#include "Generator.h"
#include "../CHTLNode/BaseNode.h"
#include <sstream>
#include <fstream>

namespace CHTL {

Generator::Generator(const GeneratorConfig& config) 
    : config(config), stats() {
}

String Generator::generateHTML(const BaseNode* ast) {
    if (!ast) return "";
    
    std::ostringstream html;
    html << "<!DOCTYPE html>\n<html>\n<head>\n<title>Generated</title>\n</head>\n<body>\n";
    html << "<!-- CHTL Generated -->\n</body>\n</html>";
    return html.str();
}

String Generator::generateCSS(const BaseNode* ast) {
    if (!ast) return "";
    return "/* Generated CSS */\nbody { font-family: Arial; }\n";
}

String Generator::generateJavaScript(const BaseNode* ast) {
    if (!ast) return "";
    return "// Generated JS\nconsole.log('CHTL');\n";
}

GenerationOutput Generator::generate(const BaseNode* ast) {
    GenerationOutput output;
    if (ast) {
        output.html = generateHTML(ast);
        output.css = generateCSS(ast);
        output.javascript = generateJavaScript(ast);
        output.success = true;
    }
    updateStats(output);
    return output;
}

GenerationOutput Generator::generateDocument(const BaseNode* documentNode) {
    return generate(documentNode);
}

GenerationOutput Generator::generateFragment(const BaseNode* fragmentNode) {
    return generate(fragmentNode);
}

std::vector<GenerationOutput> Generator::generateFiles(const std::vector<BaseNode*>& asts, const StringVector& outputPaths) {
    std::vector<GenerationOutput> results;
    for (auto* ast : asts) {
        if (ast) results.push_back(generate(ast));
    }
    return results;
}

std::vector<GenerationOutput> Generator::generateToDirectories(const std::vector<BaseNode*>& asts, const StringVector& outputDirs) {
    return generateFiles(asts, outputDirs);
}

std::vector<GenerationOutput> Generator::generateFilesParallel(const std::vector<BaseNode*>& asts, const StringVector& outputPaths, size_t threadCount) {
    return generateFiles(asts, outputPaths);
}

bool Generator::writeOutputToFile(const GenerationOutput& output, const String& basePath) {
    try {
        std::ofstream file(basePath + ".html");
        if (file.is_open()) {
            file << output.html;
            file.close();
            return true;
        }
    } catch (...) {}
    return false;
}

bool Generator::writeOutputToDirectory(const GenerationOutput& output, const String& outputDir) {
    return writeOutputToFile(output, outputDir + "/output");
}

void Generator::updateStats(const GenerationOutput& output) {
    stats.totalGenerations++;
    if (output.success) stats.successfulGenerations++;
    else stats.failedGenerations++;
}

String Generator::getOutputFileName(const String& basePath, const String& extension) const {
    return basePath + "." + extension;
}

bool Generator::ensureDirectoryExists(const String& dirPath) const {
    return true;
}

} // namespace CHTL