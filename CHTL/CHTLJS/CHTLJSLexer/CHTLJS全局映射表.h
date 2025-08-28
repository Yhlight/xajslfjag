#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace CHTLJS {
    namespace 核心 {
        
        /**
         * CSS选择器类型枚举
         * 严格按照CHTL语法文档中CHTL JS增强选择器部分定义
         * CHTL JS选择器功能：{{CSS选择器}}创建DOM对象
         */
        enum class CSS选择器类型 {
            // 基础选择器（语法文档明确支持）
            标签选择器,              // {{button}}
            类选择器,                // {{.box}}
            ID选择器,                // {{#box}}
            
            // 组合选择器（语法文档明确支持）
            后代选择器,              // {{.box button}}
            
            // 索引访问（语法文档明确支持）
            索引选择器,              // {{button[0]}}
            
            // 引用选择器（语法文档明确支持）
            引用选择器,              // {{&}}
            
            未知选择器
        };
        
        /**
         * JS事件类型枚举
         * 严格按照CHTL语法文档中CHTL JS支持的所有JS事件
         * 用于listen函数和delegate函数
         */
        enum class JS事件类型 {
            // 鼠标事件（语法文档明确支持）
            CLICK, MOUSEENTER, MOUSELEAVE, MOUSEMOVE, MOUSEDOWN, MOUSEUP,
            MOUSEOVER, MOUSEOUT, CONTEXTMENU, DBLCLICK,
            
            // 键盘事件（语法文档明确支持）
            KEYDOWN, KEYUP, KEYPRESS,
            
            // 表单事件（语法文档明确支持）
            SUBMIT, CHANGE, INPUT, FOCUS, BLUR, SELECT,
            
            // 窗口事件（语法文档明确支持）
            LOAD, UNLOAD, RESIZE, SCROLL,
            
            // 触摸事件（语法文档明确支持）
            TOUCHSTART, TOUCHEND, TOUCHMOVE, TOUCHCANCEL,
            
            // 拖拽事件（语法文档明确支持）
            DRAGSTART, DRAG, DRAGEND, DRAGENTER, DRAGOVER, DRAGLEAVE, DROP,
            
            未知事件
        };
        
        /**
         * 动画缓慢函数类型枚举
         * 严格按照CHTL语法文档中CHTL JS动画部分定义
         */
        enum class 动画缓慢函数类型 {
            // 基础缓慢函数（语法文档明确支持）
            LINEAR,                  // linear
            EASE,                    // ease
            EASE_IN,                 // ease-in
            EASE_OUT,                // ease-out
            EASE_IN_OUT,             // ease-in-out
            
            // 高级缓慢函数（语法文档明确支持）
            CUBIC_BEZIER,            // cubic-bezier
            STEPS,                   // steps
            
            未知缓慢函数
        };
        
        /**
         * CHTL JS全局符号映射表
         * 管理CSS选择器、JS事件、动画缓慢函数等CHTL JS专用符号
         * 完全独立于CHTL，不共享任何文件体系
         */
        class CHTLJS全局映射表 {
        private:
            // CSS选择器映射表
            static std::unordered_map<std::string, CSS选择器类型> CSS选择器映射表_;
            static std::unordered_map<CSS选择器类型, std::string> CSS选择器名称映射表_;
            
            // JS事件映射表
            static std::unordered_map<std::string, JS事件类型> JS事件映射表_;
            static std::unordered_map<JS事件类型, std::string> JS事件名称映射表_;
            
            // 动画缓慢函数映射表
            static std::unordered_map<std::string, 动画缓慢函数类型> 动画缓慢函数映射表_;
            static std::unordered_map<动画缓慢函数类型, std::string> 动画缓慢函数名称映射表_;
            
            // CHTL JS函数关键字集合（语法文档定义的函数）
            static std::unordered_set<std::string> CHTLJS函数关键字集合_;
            
            // 初始化标志
            static bool 已初始化_;
            
            // 初始化方法
            static void 初始化CSS选择器();
            static void 初始化JS事件();
            static void 初始化动画缓慢函数();
            static void 初始化CHTLJS函数关键字();
            
        public:
            // 确保映射表已初始化
            static void 确保已初始化();
            
            // CSS选择器相关方法
            static CSS选择器类型 获取CSS选择器类型(const std::string& 选择器);
            static std::string 获取CSS选择器名称(CSS选择器类型 类型);
            static bool 是有效CSS选择器(const std::string& 选择器);
            static bool 是索引选择器(const std::string& 选择器);
            static bool 是引用选择器(const std::string& 选择器);
            
            // JS事件相关方法
            static JS事件类型 获取JS事件类型(const std::string& 事件名);
            static std::string 获取JS事件名称(JS事件类型 类型);
            static bool 是有效JS事件(const std::string& 事件名);
            
            // 动画缓慢函数相关方法
            static 动画缓慢函数类型 获取动画缓慢函数类型(const std::string& 函数名);
            static std::string 获取动画缓慢函数名称(动画缓慢函数类型 类型);
            static bool 是有效动画缓慢函数(const std::string& 函数名);
            
            // CHTL JS函数验证
            static bool 是CHTLJS函数关键字(const std::string& 关键字);
            
            // 选择器解析辅助方法
            static std::string 提取选择器名称(const std::string& 完整选择器);
            static int 提取选择器索引(const std::string& 索引选择器);
            static bool 是类选择器格式(const std::string& 选择器);
            static bool 是ID选择器格式(const std::string& 选择器);
            
            // 获取所有有效列表
            static std::vector<std::string> 获取所有CSS选择器();
            static std::vector<std::string> 获取所有JS事件();
            static std::vector<std::string> 获取所有动画缓慢函数();
            static std::vector<std::string> 获取所有CHTLJS函数关键字();
            
            // 工具方法（CHTL JS专用）
            static std::string 转小写(const std::string& 字符串);
            static std::string 转驼峰格式(const std::string& 字符串);
            static bool 包含字符(const std::string& 字符串, char 字符);
        };
        
    } // namespace 核心
} // namespace CHTLJS