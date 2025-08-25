#pragma once

#include "UnifiedScanner/CHTLUnifiedScanner.h"
#include "Core/Ast.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

    // An intermediate structure to hold results before final processing and merging.
    struct IntermediateCompilationResult {
        std::string InitialHtml;
        std::string ChtlGeneratedCss;
        std::string ChtlGeneratedJs;
        std::string ChtlJsGeneratedJs;
        std::string RawCss;
        std::string RawJs;
        bool EmitHtml5Doctype = false;
        std::shared_ptr<ProgramNode> Ast;
    };

    // Dispatches code slices to the appropriate compiler.
    class CompilerDispatcher {
    public:
        CompilerDispatcher(const Config& config);

        // Takes all code slices and returns an intermediate result.
        IntermediateCompilationResult Dispatch(const std::vector<CodeSlice>& slices);
    };

}