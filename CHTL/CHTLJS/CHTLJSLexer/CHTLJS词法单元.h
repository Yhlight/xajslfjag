#pragma once
#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {
    namespace 核心 {
        
        /**
         * CHTL JS词法单元类型枚举
         * 严格按照CHTL语法文档中的CHTL JS部分定义
         * CHTL JS是独立的编程语言，与JS毫无关系，不能包含JS的任何内容
         * 完全独立于CHTL，不共享任何文件体系
         */
        enum class CHTLJS词法单元类型 {
            // 基础词法单元
            标识符,                    // 标识符
            字符串字面量,              // 字符串字面量 ("", '')  
            无修饰字面量,              // 无修饰字面量（CHTL JS支持）
            数字,                      // 数字
            
            // 分隔符和操作符
            冒号,                      // :
            分号,                      // ;
            逗号,                      // ,
            点号,                      // .
            等号,                      // =
            
            // 括号
            左大括号,                  // {
            右大括号,                  // }
            左中括号,                  // [
            右中括号,                  // ]
            左小括号,                  // (
            右小括号,                  // )
            
            // 注释类型（CHTL JS支持的注释）
            双斜杠注释,                // //
            块注释,                    // /* */
            生成器注释,                // -- （CHTL JS也支持生成器注释）
            
            // CHTL JS特殊关键字
            关键字_模块,               // module (模块导入语法)
            关键字_加载,               // load (模块加载)
            关键字_监听,               // listen (增强监听器)
            关键字_委托,               // delegate (事件委托)
            关键字_目标,               // target (委托目标)
            关键字_动画,               // animate (动画)
            关键字_持续时间,           // duration
            关键字_缓慢函数,           // easing
            关键字_开始,               // begin
            关键字_何时,               // when
            关键字_结束,               // end
            关键字_循环,               // loop
            关键字_方向,               // direction
            关键字_延迟,               // delay
            关键字_回调,               // callback
            关键字_虚对象,             // vir (虚对象)
            关键字_在,                 // at (动画时间点)
            
            // CHTL JS增强选择器语法
            选择器开始,                // {{ 
            选择器结束,                // }}
            引用符号,                  // & (引用选择器)
            箭头,                      // -> (明确使用CHTL JS语法)
            事件绑定,                  // &-> (事件绑定操作符)
            
            // CHTL JS函数语法
            函数开始,                  // function关键字后的 {
            函数结束,                  // }
            
            // 特殊字符
            换行符,                    // 换行符
            空白字符,                  // 空白字符
            文件结束,                  // 文件结束标记
            未知字符                   // 未知字符
        };
        
        /**
         * CHTL JS词法单元位置信息
         * 用于错误报告和调试
         */
        struct CHTLJS词法单元位置 {
            size_t 行号;               // 行号（从1开始）
            size_t 列号;               // 列号（从1开始）
            size_t 偏移量;             // 在文件中的字节偏移量
            
            CHTLJS词法单元位置(size_t 行 = 1, size_t 列 = 1, size_t 偏移 = 0) 
                : 行号(行), 列号(列), 偏移量(偏移) {}
        };
        
        /**
         * CHTL JS词法单元
         * 表示从CHTL JS源代码中解析出的一个语法元素
         * CHTL JS完全独立于JS，是一门独立的编程语言
         * 与CHTL完全分离，不共享任何文件体系
         */
        class CHTLJS词法单元 {
        private:
            CHTLJS词法单元类型 类型_;           // 词法单元类型
            std::string 值_;                    // 词法单元值
            CHTLJS词法单元位置 位置_;           // 位置信息
            
        public:
            CHTLJS词法单元(CHTLJS词法单元类型 类型, const std::string& 值, 
                          const CHTLJS词法单元位置& 位置);
            
            // 访问器方法
            CHTLJS词法单元类型 获取类型() const { return 类型_; }
            const std::string& 获取值() const { return 值_; }
            const CHTLJS词法单元位置& 获取位置() const { return 位置_; }
            
            // 类型判断方法（严格按照CHTL JS语法文档分类）
            bool 是CHTLJS关键字() const;
            bool 是字面量() const;
            bool 是操作符() const;
            bool 是注释() const;
            bool 是选择器语法() const;
            bool 是函数语法() const;
            bool 是模块语法() const;
            bool 是动画语法() const;
            bool 是虚对象语法() const;
            
            // 字符串表示方法
            std::string 转为字符串() const;
            
            // 比较操作
            bool operator==(const CHTLJS词法单元& 其他) const;
            bool operator!=(const CHTLJS词法单元& 其他) const;
        };
        
        // CHTL JS工具函数（完全独立的实现）
        std::string CHTLJS词法单元类型转字符串(CHTLJS词法单元类型 类型);
        bool 是CHTLJS关键字字符串(const std::string& 字符串);
        CHTLJS词法单元类型 获取CHTLJS关键字类型(const std::string& 字符串);
        
    } // namespace 核心
} // namespace CHTLJS