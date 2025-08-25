#include "CHTL_JS/BuiltinFunctions/AnimateHandler.h"
#include <sstream>
#include <iomanip>

namespace CHTL_JS {

std::string AnimateHandler::Generate(const AnimateNode& node) {
    std::stringstream js;
    
    // Generate JavaScript code from CHTL JS animate() function
    // This is the COMPILATION result, not the source
    
    // Generate unique animation ID
    static int animationCounter = 0;
    std::string animId = "chtl_anim_" + std::to_string(++animationCounter);
    
    js << "(function() {\n";
    js << "  const targets = " << GenerateTargetArray(node) << ";\n";
    js << "  const duration = " << node.Duration << ";\n";
    js << "  const easing = '" << node.Easing << "';\n";
    js << "  const loop = " << node.LoopCount << ";\n";
    js << "  const direction = '" << node.Direction << "';\n";
    js << "  const delay = " << node.Delay << ";\n";
    
    // Generate keyframes
    js << "  const keyframes = " << GenerateKeyframes(node) << ";\n";
    
    // Create animation using Web Animations API
    js << "  const animations = [];\n";
    js << "  targets.forEach(target => {\n";
    js << "    if (target) {\n";
    js << "      const anim = target.animate(keyframes, {\n";
    js << "        duration: duration,\n";
    js << "        easing: easing,\n";
    js << "        iterations: loop === -1 ? Infinity : loop,\n";
    js << "        direction: direction,\n";
    js << "        delay: delay,\n";
    js << "        fill: 'forwards'\n";
    js << "      });\n";
    js << "      animations.push(anim);\n";
    js << "    }\n";
    js << "  });\n";
    
    // Handle callback
    if (!node.Callback.empty()) {
        js << "  Promise.all(animations.map(a => a.finished)).then(() => {\n";
        js << "    " << node.Callback << "\n";
        js << "  });\n";
    }
    
    // Return animation control object
    js << "  return {\n";
    js << "    play: () => animations.forEach(a => a.play()),\n";
    js << "    pause: () => animations.forEach(a => a.pause()),\n";
    js << "    cancel: () => animations.forEach(a => a.cancel()),\n";
    js << "    finish: () => animations.forEach(a => a.finish()),\n";
    js << "    reverse: () => animations.forEach(a => a.reverse())\n";
    js << "  };\n";
    js << "})()";
    
    return js.str();
}

std::string AnimateHandler::GenerateTargetArray(const AnimateNode& node) {
    std::stringstream js;
    
    if (std::holds_alternative<std::shared_ptr<AstNode>>(node.Targets)) {
        // Single target - will be compiled from CHTL JS selector to JS
        js << "["; // TODO: Generate selector/element reference from CHTL JS AST
        js << "]";
    } else {
        // Multiple targets - will be compiled from CHTL JS selectors to JS
        js << "["; // TODO: Generate array of selectors/element references from CHTL JS AST
        js << "]";
    }
    
    return js.str();
}

std::string AnimateHandler::GenerateCSSProperties(const std::map<std::string, std::string>& props) {
    std::stringstream js;
    js << "{";
    
    bool first = true;
    for (const auto& [prop, value] : props) {
        if (!first) js << ", ";
        js << "'" << prop << "': '" << value << "'";
        first = false;
    }
    
    js << "}";
    return js.str();
}

std::string AnimateHandler::GenerateKeyframes(const AnimateNode& node) {
    std::stringstream js;
    js << "[\n";
    
    // Begin state
    if (!node.BeginState.empty()) {
        js << "    " << GenerateCSSProperties(node.BeginState);
    } else {
        js << "    {}";
    }
    
    // Intermediate keyframes
    for (const auto& [at, props] : node.KeyFrames) {
        js << ",\n    ";
        auto keyframe = props;
        keyframe["offset"] = std::to_string(at);
        js << GenerateCSSProperties(keyframe);
    }
    
    // End state
    if (!node.EndState.empty()) {
        js << ",\n    " << GenerateCSSProperties(node.EndState);
    }
    
    js << "\n  ]";
    return js.str();
}

}