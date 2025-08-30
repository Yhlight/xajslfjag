#pragma once

#include <string>
#include <vector>

// 珂朵莉CJMOD模块头文件
// 提供printMylove, iNeverAway等CHTL JS扩展功能

namespace CHTL {
    class Arg;
}

// 主要导出函数
extern "C" {
    // 模块初始化
    void initializeChthollyModule();
    
    // printMylove相关
    void registerPrintMylove();
    std::string generatePixelArt(const std::string& imageUrl, const std::string& mode, 
                               int width = 80, int height = 40, double scale = 1.0);
    
    // iNeverAway相关
    void registerINeverAway();
    std::string generateINeverAwayCode(const std::vector<std::string>& functionDefs);
    
    // 珂朵莉特色功能
    void registerChthollyMagic();
    void registerSwordGlow();
    void registerPetalRain();
    void registerMemoryBox();
    void registerHappinessLevel();
    
    // 导出结果
    void exportChthollyResult(const CHTL::Arg& args);
}

// 珂朵莉魔法效果常量
namespace ChthollyMagic {
    constexpr const char* PETAL_COLORS[] = {
        "#ffb6c1", "#ffc0cb", "#ffe4e1", "#fff0f5", "#ffefd5"
    };
    
    constexpr const char* SWORD_EFFECTS[] = {
        "glow", "sparkle", "slash", "pierce", "divine"
    };
    
    constexpr const char* MEMORY_TYPES[] = {
        "happy", "sad", "nostalgic", "hopeful", "peaceful"
    };
}

// 珂朵莉主题配置
struct ChthollyConfig {
    bool enablePetalRain = true;
    bool enableSwordEffects = true;
    bool enableMemorySystem = true;
    double happinessMultiplier = 1.0;
    int maxMemories = 100;
    
    // 动画配置
    int petalCount = 50;
    double petalSpeed = 1.0;
    int swordGlowDuration = 1000;
    int memoryFadeDuration = 800;
};

// 全局珂朵莉配置实例
extern ChthollyConfig g_chthollyConfig;