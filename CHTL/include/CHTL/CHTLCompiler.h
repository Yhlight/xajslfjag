#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <variant>
#include <memory>

namespace CHTL {

    // Forward declarations
    class CHTLUnifiedScanner;
    struct ImportNode;
    struct CompilationResult; // Assuming this struct exists

    // Represents the filesystem context for a compilation unit
    using FileSystemContext = std::variant<std::string /* base path for disk */, 
                                           std::shared_ptr<std::map<std::string, std::string>> /* in-memory fs */>;

    struct CompilationContext {
        FileSystemContext FsContext;
        std::set<std::string> ProcessedFiles;
        CHTLUnifiedScanner* Scanner;
    };

    class CHTLCompiler {
    public:
        std::string Compile(const std::string& filePath);
        Config ParseAndGetNamedConfig(const std::string& source, const std::string& configName);

    private:
        CompilationResult CompileInternal(const std::string& entryPath, CompilationContext& context);
        // ... other private methods
    };

}