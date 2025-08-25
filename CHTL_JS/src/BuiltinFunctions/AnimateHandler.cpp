#include "CHTL_JS/BuiltinFunctions/AnimateHandler.h"
#include "CHTL_JS/Core/Runtime.h"
#include <sstream>
#include <iomanip>

namespace CHTL_JS {

std::string AnimateHandler::Generate(const AnimateNode& node) {
    std::stringstream chtljs;
    
    // Generate CHTL JS code, NOT JavaScript!
    // CHTL JS has its own syntax and runtime
    
    // Generate unique animation ID in CHTL JS style
    static int animationCounter = 0;
    std::string animId = "chtljs_animation_" + std::to_string(++animationCounter);
    
    // CHTL JS animation declaration syntax
    chtljs << "animation " << animId << " {\n";
    
    // Target specification in CHTL JS
    chtljs << "  targets: " << GenerateTargetArray(node) << ";\n";
    
    // Animation properties in CHTL JS syntax
    chtljs << "  duration: " << node.Duration << "ms;\n";
    chtljs << "  easing: " << node.Easing << ";\n";
    chtljs << "  iterations: " << node.LoopCount << ";\n";
    chtljs << "  direction: " << node.Direction << ";\n";
    chtljs << "  delay: " << node.Delay << "ms;\n";
    
    // Keyframes in CHTL JS syntax
    chtljs << "  keyframes {\n";
    
    // Begin state
    if (!node.BeginState.empty()) {
        chtljs << "    0% {\n";
        for (const auto& [prop, value] : node.BeginState) {
            chtljs << "      " << prop << ": " << value << ";\n";
        }
        chtljs << "    }\n";
    }
    
    // Intermediate keyframes
    for (const auto& [at, props] : node.KeyFrames) {
        chtljs << "    " << (at * 100) << "% {\n";
        for (const auto& [prop, value] : props) {
            chtljs << "      " << prop << ": " << value << ";\n";
        }
        chtljs << "    }\n";
    }
    
    // End state
    if (!node.EndState.empty()) {
        chtljs << "    100% {\n";
        for (const auto& [prop, value] : node.EndState) {
            chtljs << "      " << prop << ": " << value << ";\n";
        }
        chtljs << "    }\n";
    }
    
    chtljs << "  }\n";
    
    // Callback in CHTL JS syntax
    if (!node.Callback.empty()) {
        chtljs << "  on_complete: " << node.Callback << ";\n";
    }
    
    chtljs << "}\n";
    
    // Return CHTL JS animation invocation
    chtljs << "invoke_animation(" << animId << ")";
    
    return chtljs.str();
}

std::string AnimateHandler::GenerateTargetArray(const AnimateNode& node) {
    std::stringstream chtljs;
    
    // Generate CHTL JS array syntax, not JavaScript
    if (std::holds_alternative<std::shared_ptr<AstNode>>(node.Targets)) {
        // Single target in CHTL JS syntax
        chtljs << "[single_target]"; // Placeholder - would generate actual CHTL JS selector
    } else {
        // Multiple targets in CHTL JS syntax
        chtljs << "[target_array]"; // Placeholder - would generate actual CHTL JS array
    }
    
    return chtljs.str();
}

std::string AnimateHandler::GenerateCSSProperties(const std::map<std::string, std::string>& props) {
    // This method is no longer needed as we generate CHTL JS syntax directly
    return "";
}

std::string AnimateHandler::GenerateKeyframes(const AnimateNode& node) {
    // This method is no longer needed as we generate CHTL JS syntax directly
    return "";
}

}