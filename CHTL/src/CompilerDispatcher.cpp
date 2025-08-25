#include "CompilerDispatcher.h"
#include "CHTLCompiler.h"
#include "CHTL_JS/Compiler.h"
#include "CSSCompiler.h"
#include "JSCompiler.h"

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(const Config& config)
    : m_Config(config),
      m_ChtlCompiler(config),       // Pass config to CHTL compiler
      m_ChtlJsCompiler(config),    // Pass config to CHTL JS compiler
      m_CssCompiler(),
      m_JsCompiler() {}

FinalOutput CompilerDispatcher::Dispatch(const std::vector<CodeSlice>& slices) {
    FinalOutput output;

    for (const auto& slice : slices) {
        switch (slice.Type) {
            case SliceType::CHTL: {
                auto chtlResult = m_ChtlCompiler.Compile(slice.Content);
                output.Html += chtlResult.Html;
                output.Css += chtlResult.Css; // CSS from local style blocks
                if (chtlResult.EmitHtml5Doctype) output.EmitHtml5Doctype = true;
                break;
            }
            case SliceType::CHTL_JS: {
                output.Js += m_ChtlJsCompiler.Compile(slice.Content);
                break;
            }
            case SliceType::CSS: {
                output.Css += m_CssCompiler.Compile(slice.Content);
                break;
            }
            case SliceType::JS: {
                output.Js += m_JsCompiler.Compile(slice.Content);
                break;
            }
        }
    }
    return output;
}

}
