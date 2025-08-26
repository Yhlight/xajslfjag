#pragma once

#include "ModuleStructure.h"
#include <string>
#include <vector>

namespace CHTL {
namespace Core {

/**
 * ChthollyModuleTemplate - 珂朵莉模块模板
 * 严格按照语法文档实现珂朵莉混合模块结构
 * 珂朵莉模块采用CMOD + CJMOD的混合模块
 */
class ChthollyModuleTemplate {
public:
    /**
     * CMOD部分组件（严格按照语法文档）
     */
    enum class CMODComponent {
        ACCORDION,           // 手风琴
        FOUR_LEAF_GALLERY,   // 四叶窗相册
        MEMO,               // 备忘录
        WARM_NOTES,         // 暖色笔记
        CHERRY_RAIN,        // 樱花雨
        MOUSE_EFFECTS,      // 鼠标特效
        MOUSE_TRAIL,        // 鼠标拖尾
        PARALLAX_BG,        // 视差滚动背景
        CONTEXT_MENU,       // 右键菜单栏
        PROGRESS_BAR        // 进度条
    };
    
    /**
     * CJMOD部分功能（严格按照语法文档）
     */
    enum class CJMODFunction {
        PRINT_MY_LOVE,      // printMylove - 图片转字符像素块
        I_NEVER_AWAY        // iNeverAway - 标记函数系统
    };
    
    /**
     * 生成珂朵莉模块结构
     * @param output_directory 输出目录
     * @return 是否成功
     */
    static bool generateChthollyModule(const std::string& output_directory);
    
    /**
     * 生成CMOD部分结构
     * @param cmod_directory CMOD目录
     * @return 是否成功
     */
    static bool generateCMODStructure(const std::string& cmod_directory);
    
    /**
     * 生成CJMOD部分结构
     * @param cjmod_directory CJMOD目录
     * @return 是否成功
     */
    static bool generateCJMODStructure(const std::string& cjmod_directory);
    
    /**
     * 生成珂朵莉模块信息文件
     * @param info_file_path 信息文件路径
     * @return 是否成功
     */
    static bool generateChthollyInfo(const std::string& info_file_path);
    
    /**
     * 生成CMOD组件模板
     * @param component 组件类型
     * @param output_directory 输出目录
     * @return 是否成功
     */
    static bool generateCMODComponent(CMODComponent component, const std::string& output_directory);
    
    /**
     * 生成CJMOD功能模板
     * @param function 功能类型
     * @param output_directory 输出目录
     * @return 是否成功
     */
    static bool generateCJMODFunction(CJMODFunction function, const std::string& output_directory);

private:
    // CMOD组件模板
    static std::string generateAccordionTemplate();
    static std::string generateFourLeafGalleryTemplate();
    static std::string generateMemoTemplate();
    static std::string generateWarmNotesTemplate();
    static std::string generateCherryRainTemplate();
    static std::string generateMouseEffectsTemplate();
    static std::string generateMouseTrailTemplate();
    static std::string generateParallaxBgTemplate();
    static std::string generateContextMenuTemplate();
    static std::string generateProgressBarTemplate();
    
    // CJMOD功能模板
    static std::string generatePrintMyLoveHeader();
    static std::string generatePrintMyLoveSource();
    static std::string generateINeverAwayHeader();
    static std::string generateINeverAwaySource();
    
    // 信息模板
    static std::string generateChthollyInfoTemplate();
    static std::string generateComponentInfoTemplate(const std::string& component_name, const std::string& description);
    static std::string generateFunctionInfoTemplate(const std::string& function_name, const std::string& description);
    
    // 工具方法
    static std::string componentToString(CMODComponent component);
    static std::string functionToString(CJMODFunction function);
    static std::string getComponentDescription(CMODComponent component);
    static std::string getFunctionDescription(CJMODFunction function);
};

/**
 * YuigahamaModuleTemplate - 由比滨结衣模块模板
 * 严格按照语法文档，由比滨结衣模块使用CMOD
 */
class YuigahamaModuleTemplate {
public:
    /**
     * 由比滨结衣模块组件（严格按照语法文档）
     */
    enum class YuigahamaComponent {
        MUSIC_PLAYER,       // 音乐播放器
        ACCORDION,          // 手风琴
        FOUR_LEAF_GALLERY,  // 四叶窗相册
        MEMO,              // 备忘录
        WARM_NOTES,        // 暖色笔记
        CHERRY_RAIN,       // 樱花雨
        MOUSE_EFFECTS      // 鼠标特效
    };
    
    /**
     * 生成由比滨结衣模块结构
     * @param output_directory 输出目录
     * @return 是否成功
     */
    static bool generateYuigahamaModule(const std::string& output_directory);
    
    /**
     * 生成由比滨结衣模块信息文件
     * @param info_file_path 信息文件路径
     * @return 是否成功
     */
    static bool generateYuigahamaInfo(const std::string& info_file_path);
    
    /**
     * 生成由比滨结衣组件模板
     * @param component 组件类型
     * @param output_directory 输出目录
     * @return 是否成功
     */
    static bool generateYuigahamaComponent(YuigahamaComponent component, const std::string& output_directory);

private:
    // 组件模板
    static std::string generateMusicPlayerTemplate();
    static std::string generateYuigahamaAccordionTemplate();
    static std::string generateYuigahamaFourLeafGalleryTemplate();
    static std::string generateYuigahamaMemoTemplate();
    static std::string generateYuigahamaWarmNotesTemplate();
    static std::string generateYuigahamaCherryRainTemplate();
    static std::string generateYuigahamaMouseEffectsTemplate();
    
    // 信息模板
    static std::string generateYuigahamaInfoTemplate();
    
    // 工具方法
    static std::string yuigahamaComponentToString(YuigahamaComponent component);
    static std::string getYuigahamaComponentDescription(YuigahamaComponent component);
};

/**
 * ModuleTemplateFactory - 模块模板工厂
 * 统一创建各种官方模块模板
 */
class ModuleTemplateFactory {
public:
    /**
     * 官方模块类型
     */
    enum class OfficialModuleType {
        CHTHOLLY,           // 珂朵莉模块 (CMOD + CJMOD混合)
        YUIGAHAMA,          // 由比滨结衣模块 (纯CMOD)
        CUSTOM_CMOD,        // 自定义CMOD模块
        CUSTOM_CJMOD,       // 自定义CJMOD模块
        CUSTOM_MIXED        // 自定义混合模块
    };
    
    /**
     * 创建官方模块模板
     * @param module_type 模块类型
     * @param module_name 模块名称
     * @param output_directory 输出目录
     * @return 是否成功
     */
    static bool createOfficialModule(OfficialModuleType module_type, 
                                   const std::string& module_name,
                                   const std::string& output_directory);
    
    /**
     * 创建自定义模块模板
     * @param structure_type 结构类型
     * @param module_name 模块名称
     * @param output_directory 输出目录
     * @param author 作者
     * @param description 描述
     * @return 是否成功
     */
    static bool createCustomModule(ModuleStructureType structure_type,
                                 const std::string& module_name,
                                 const std::string& output_directory,
                                 const std::string& author = "CHTL Developer",
                                 const std::string& description = "Custom CHTL Module");
    
    /**
     * 验证模块模板
     * @param module_directory 模块目录
     * @return 验证结果
     */
    static ModuleStructureValidator::ValidationResult validateModuleTemplate(const std::string& module_directory);

private:
    static bool createCMODTemplate(const std::string& module_name, const std::string& output_directory, 
                                  const std::string& author, const std::string& description);
    static bool createCJMODTemplate(const std::string& module_name, const std::string& output_directory,
                                   const std::string& author, const std::string& description);
    static bool createMixedTemplate(const std::string& module_name, const std::string& output_directory,
                                   const std::string& author, const std::string& description);
    
    static std::string generateBasicInfoTemplate(const std::string& module_name, const std::string& author, 
                                                const std::string& description, const std::string& category);
    static std::string generateBasicChtlTemplate(const std::string& module_name);
    static std::string generateBasicCppTemplate(const std::string& module_name);
    static std::string generateBasicHeaderTemplate(const std::string& module_name);
};

} // namespace Core
} // namespace CHTL