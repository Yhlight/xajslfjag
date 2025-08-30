// 示例：animate语法的CJMOD扩展实现
#include "../../src/CHTLJS/CJMODSystem/Runtime/CJMODRuntime.h"
#include <sstream>
#include <regex>

using namespace CHTL;

class AnimateExtension : public CJMODExtension {
public:
    std::string getName() const override {
        return "animate-extension";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    std::string getDescription() const override {
        return "Provides enhanced animation syntax for CHTL JS";
    }
    
    bool initialize(CJMODRuntimeContext* context) override {
        context_ = context;
        context_->log("Animate extension initialized");
        return true;
    }
    
    ProcessResult process(
        const std::string& syntaxName,
        const std::string& matchedText,
        const std::map<std::string, std::string>& captures
    ) override {
        ProcessResult result;
        
        if (syntaxName == "animate") {
            result = processAnimateBlock(matchedText, captures);
        } else if (syntaxName == "animate-inline") {
            result = processInlineAnimate(matchedText, captures);
        } else {
            result.errorMessage = "Unknown syntax: " + syntaxName;
        }
        
        return result;
    }
    
private:
    CJMODRuntimeContext* context_ = nullptr;
    
    ProcessResult processAnimateBlock(
        const std::string& matchedText,
        const std::map<std::string, std::string>& captures
    ) {
        ProcessResult result;
        
        try {
            // 获取animate块的内容
            auto bodyIt = captures.find("body");
            if (bodyIt == captures.end()) {
                result.errorMessage = "Missing animate body";
                return result;
            }
            
            const std::string& body = bodyIt->second;
            
            // 解析动画定义
            std::stringstream js;
            js << "(function() {\n";
            js << "  const animations = [];\n";
            
            // 解析每个选择器的动画
            std::regex selectorRegex(R"(\{\{([^}]+)\}\}\s*->\s*\{([^}]+)\})");
            std::smatch match;
            std::string remaining = body;
            
            while (std::regex_search(remaining, match, selectorRegex)) {
                std::string selector = match[1];
                std::string animDef = match[2];
                
                js << "  animations.push({\n";
                js << "    selector: '" << escapeString(selector) << "',\n";
                js << "    animation: " << parseAnimationDef(animDef) << "\n";
                js << "  });\n";
                
                remaining = match.suffix();
            }
            
            // 生成执行代码
            js << "  \n";
            js << "  // Execute animations\n";
            js << "  animations.forEach(({selector, animation}) => {\n";
            js << "    const elements = document.querySelectorAll(selector);\n";
            js << "    elements.forEach((el, index) => {\n";
            js << "      const delay = typeof animation.delay === 'function' \n";
            js << "        ? animation.delay(index) \n";
            js << "        : animation.delay || 0;\n";
            js << "      \n";
            js << "      if (animation.trigger === 'onLoad') {\n";
            js << "        setTimeout(() => {\n";
            js << "          el.animate([\n";
            js << "            animation.from || {},\n";
            js << "            animation.to || {}\n";
            js << "          ], {\n";
            js << "            duration: animation.duration || 1000,\n";
            js << "            easing: animation.easing || 'ease',\n";
            js << "            fill: 'forwards'\n";
            js << "          });\n";
            js << "        }, delay);\n";
            js << "      } else if (animation.trigger === 'onScroll') {\n";
            js << "        // Intersection Observer for scroll animations\n";
            js << "        const observer = new IntersectionObserver((entries) => {\n";
            js << "          entries.forEach(entry => {\n";
            js << "            if (entry.isIntersecting) {\n";
            js << "              setTimeout(() => {\n";
            js << "                entry.target.animate([\n";
            js << "                  animation.from || {},\n";
            js << "                  animation.to || {}\n";
            js << "                ], {\n";
            js << "                  duration: animation.duration || 1000,\n";
            js << "                  easing: animation.easing || 'ease',\n";
            js << "                  fill: 'forwards'\n";
            js << "                });\n";
            js << "              }, delay);\n";
            js << "              observer.unobserve(entry.target);\n";
            js << "            }\n";
            js << "          });\n";
            js << "        }, {\n";
            js << "          threshold: animation.threshold || 0.5\n";
            js << "        });\n";
            js << "        observer.observe(el);\n";
            js << "      }\n";
            js << "    });\n";
            js << "  });\n";
            js << "})();\n";
            
            result.success = true;
            result.generatedCode = js.str();
            
            // 添加依赖（如果需要polyfill）
            if (context_->getConfig("targetBrowser") == "legacy") {
                result.dependencies.push_back("web-animations-polyfill");
            }
            
        } catch (const std::exception& e) {
            result.errorMessage = "Failed to process animate block: " + std::string(e.what());
        }
        
        return result;
    }
    
    ProcessResult processInlineAnimate(
        const std::string& matchedText,
        const std::map<std::string, std::string>& captures
    ) {
        ProcessResult result;
        
        // 处理内联动画语法
        // 例如: element &-> animate({ ... })
        
        try {
            auto elementIt = captures.find("element");
            auto animationIt = captures.find("animation");
            
            if (elementIt == captures.end() || animationIt == captures.end()) {
                result.errorMessage = "Invalid inline animation syntax";
                return result;
            }
            
            std::stringstream js;
            js << elementIt->second << ".animate(" << animationIt->second << ")";
            
            result.success = true;
            result.generatedCode = js.str();
            
        } catch (const std::exception& e) {
            result.errorMessage = "Failed to process inline animation: " + std::string(e.what());
        }
        
        return result;
    }
    
    std::string parseAnimationDef(const std::string& def) {
        // 简化的动画定义解析
        // 实际实现需要更复杂的解析器
        return "{" + def + "}";
    }
    
    std::string escapeString(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '\'': result += "\\'"; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c;
            }
        }
        return result;
    }
};

// 导出函数
CJMOD_EXPORT CJMODExtension* cjmod_create_extension() {
    return new AnimateExtension();
}

CJMOD_EXPORT void cjmod_destroy_extension(CJMODExtension* ext) {
    delete ext;
}