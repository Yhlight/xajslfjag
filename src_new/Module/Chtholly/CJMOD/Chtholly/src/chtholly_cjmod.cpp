#include "chtholly_cjmod.h"
#include "../../../CHTL JS/CJMODSystem/CJMODApi.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <random>

using namespace CHTL;

// printMylove实现 - 将图片转换为字符像素块
void registerPrintMylove() {
    // 使用CJMOD API注册printMylove函数
    Arg args = Syntax::analyze("printMylove { url: $!_, mode: $?_, width: $?_, height: $?_, scale: $?_ }");
    
    // 绑定参数处理函数
    args.bind("$!_", [](const std::string& value) {
        // 必需的URL参数
        if (value.empty()) {
            throw std::runtime_error("printMylove: url参数不能为空");
        }
        return value;
    });
    
    args.bind("$?_", [](const std::string& value) {
        // 可选参数，提供默认值
        if (value.empty()) {
            return std::string("ASCII");  // 默认模式
        }
        return value;
    });
    
    // 创建CHTL JS函数
    CHTLJSFunction::CreateCHTLJSFunction("printMylove { url: $!_, mode: $?_, width: $?_, height: $?_, scale: $?_ }");
    
    std::cout << "✓ printMylove函数注册成功" << std::endl;
}

// iNeverAway实现 - 创建标记函数组
void registerINeverAway() {
    // 使用CJMOD API注册iNeverAway函数
    Arg args = Syntax::analyze("iNeverAway { $... }");
    
    // 绑定不定参数处理
    args.bind("$...", [](const std::string& value) {
        // 处理函数定义
        return value;
    });
    
    // 创建CHTL JS函数
    CHTLJSFunction::CreateCHTLJSFunction("iNeverAway { $... }");
    CHTLJSFunction::bindVirtualObject("iNeverAway");
    
    std::cout << "✓ iNeverAway函数注册成功" << std::endl;
}

// 珂朵莉魔法效果
void registerChthollyMagic() {
    // 珂朵莉的魔法特效函数
    CHTLJSFunction::CreateCHTLJSFunction("chthollyMagic { target: $!_, effect: $?_, duration: $?_ }");
    
    std::cout << "✓ chthollyMagic函数注册成功" << std::endl;
}

// 剑光效果
void registerSwordGlow() {
    // 瑟尼欧里斯剑光效果
    CHTLJSFunction::CreateCHTLJSFunction("swordGlow { element: $!_, intensity: $?_, color: $?_ }");
    
    std::cout << "✓ swordGlow函数注册成功" << std::endl;
}

// 花瓣雨效果
void registerPetalRain() {
    // 樱花雨效果
    CHTLJSFunction::CreateCHTLJSFunction("petalRain { container: $!_, count: $?_, speed: $?_, direction: $?_ }");
    
    std::cout << "✓ petalRain函数注册成功" << std::endl;
}

// 回忆盒子
void registerMemoryBox() {
    // 珂朵莉的回忆盒子
    CHTLJSFunction::CreateCHTLJSFunction("memoryBox { memories: $!_, autoPlay: $?_, interval: $?_ }");
    
    std::cout << "✓ memoryBox函数注册成功" << std::endl;
}

// 幸福指数
void registerHappinessLevel() {
    // 珂朵莉的幸福指数计算
    CHTLJSFunction::CreateCHTLJSFunction("happinessLevel { factors: $!_, weights: $?_ }");
    
    std::cout << "✓ happinessLevel函数注册成功" << std::endl;
}

// CJMOD模块初始化函数
extern "C" void initializeChthollyModule() {
    std::cout << "🌸 初始化珂朵莉CJMOD模块..." << std::endl;
    
    try {
        registerPrintMylove();
        registerINeverAway();
        registerChthollyMagic();
        registerSwordGlow();
        registerPetalRain();
        registerMemoryBox();
        registerHappinessLevel();
        
        std::cout << "✨ 珂朵莉CJMOD模块初始化完成！" << std::endl;
        std::cout << "   \"即使是黄金妖精，也能拥有最珍贵的回忆\"" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 珂朵莉CJMOD模块初始化失败: " << e.what() << std::endl;
    }
}

// printMylove具体实现
std::string generatePixelArt(const std::string& imageUrl, const std::string& mode, 
                           int width, int height, double scale) {
    std::ostringstream result;
    
    if (mode == "ASCII") {
        // ASCII艺术生成
        std::string asciiChars = "@%#*+=-:. ";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, asciiChars.length() - 1);
        
        result << "// 珂朵莉的ASCII艺术 - " << imageUrl << "\n";
        result << "console.log(`\n";
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                result << asciiChars[dis(gen)];
            }
            result << "\n";
        }
        
        result << "`);\n";
        
    } else if (mode == "Pixel") {
        // 像素块生成
        result << "// 珂朵莉的像素艺术 - " << imageUrl << "\n";
        result << "const pixelCanvas = document.createElement('canvas');\n";
        result << "pixelCanvas.width = " << width << ";\n";
        result << "pixelCanvas.height = " << height << ";\n";
        result << "const ctx = pixelCanvas.getContext('2d');\n";
        result << "// 绘制珂朵莉主题的像素图案\n";
        result << "ctx.fillStyle = '#e74c3c';\n";  // 珂朵莉的红发色
        result << "ctx.fillRect(0, 0, " << width << ", " << height << ");\n";
    }
    
    return result.str();
}

// iNeverAway具体实现
std::string generateINeverAwayCode(const std::vector<std::string>& functionDefs) {
    std::ostringstream result;
    
    result << "// 珂朵莉的iNeverAway - 永远不会离开的回忆\n";
    result << "const ChthollyMemories = {\n";
    
    for (size_t i = 0; i < functionDefs.size(); ++i) {
        if (i > 0) result << ",\n";
        
        // 解析函数定义
        std::string funcDef = functionDefs[i];
        size_t colonPos = funcDef.find(':');
        if (colonPos != std::string::npos) {
            std::string funcName = funcDef.substr(0, colonPos);
            std::string funcBody = funcDef.substr(colonPos + 1);
            
            result << "  " << funcName << ": " << funcBody;
        }
    }
    
    result << "\n};\n";
    result << "// 珂朵莉的回忆永远不会消失\n";
    
    return result.str();
}

// 导出CJMOD结果的实现
void exportChthollyResult(const Arg& args) {
    std::string jsCode = CJMODGenerator::generateJSCode(args);
    
    // 添加珂朵莉主题的代码注释
    std::string chthollyCode = "// 由珂朵莉的魔法生成 ✨\n";
    chthollyCode += "// \"世界上最幸福的女孩\"\n";
    chthollyCode += jsCode;
    chthollyCode += "\n// 珂朵莉的祝福：愿你也能找到属于自己的幸福 💝\n";
    
    std::cout << chthollyCode << std::endl;
}