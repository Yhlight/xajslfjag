#pragma once
#include <string>
#include <vector>
#include <memory>

namespace CHTL {
    namespace 核心 {
        
        /**
         * CHTL词法单元类型枚举
         * 严格按照CHTL语法文档定义的所有语法元素
         * 绝对不私自扩展，严格遵循语法文档规范
         */
        enum class CHTL词法单元类型 {
            // 基础词法单元
            标识符,                    // 标识符
            字符串字面量,              // 字符串字面量 ("", '')  
            无修饰字面量,              // 无修饰字面量（无引号字符串）
            数字,                      // 数字
            
            // 分隔符和操作符
            冒号,                      // :
            分号,                      // ;
            逗号,                      // ,
            点号,                      // .
            等号,                      // = (CE对等式，与冒号完全等价)
            
            // 括号
            左大括号,                  // {
            右大括号,                  // }
            左中括号,                  // [
            右中括号,                  // ]
            左小括号,                  // (
            右小括号,                  // )
            
            // 注释类型（严格按照语法文档）
            双斜杠注释,                // //
            块注释,                    // /* */
            生成器注释,                // -- （会被生成器识别的注释）
            
            // 关键字（严格按照语法文档定义）
            关键字_文本,               // text
            关键字_样式,               // style
            关键字_脚本,               // script
            关键字_继承,               // inherit
            关键字_删除,               // delete
            关键字_插入,               // insert
            关键字_之后,               // after
            关键字_之前,               // before
            关键字_替换,               // replace
            关键字_顶部,               // at top
            关键字_底部,               // at bottom
            关键字_来自,               // from
            关键字_作为,               // as
            关键字_除了,               // except
            关键字_使用,               // use
            关键字_HTML5,              // html5
            
            // 块类型标识符（语法文档规定的块类型）
            模板块,                    // [Template]
            自定义块,                  // [Custom]
            原始块,                    // [Origin]
            导入块,                    // [Import]
            命名空间块,                // [Namespace]
            配置块,                    // [Configuration]
            信息块,                    // [Info] (用于CMOD)
            导出块,                    // [Export] (用于CMOD)
            名称块,                    // [Name] (配置组的子块)
            原始类型块,                // [OriginType] (配置组的子块)
            
            // 类型标识符（语法文档规定的@类型）
            类型_样式,                 // @Style
            类型_元素,                 // @Element
            类型_变量,                 // @Var
            类型_HTML,                 // @Html
            类型_JavaScript,           // @JavaScript
            类型_CHTL,                 // @Chtl
            类型_CJMOD,                // @CJmod
            类型_配置,                 // @Config
            类型_Vue,                  // @Vue (自定义原始嵌入类型示例)
            
            // CHTL JS特殊语法（语法文档定义的增强选择器等）
            选择器开始,                // {{
            选择器结束,                // }}
            引用符号,                  // &
            箭头,                      // ->
            事件绑定,                  // &->
            
            // HTML标签和属性
            HTML标签,                  // HTML元素标签
            属性名,                    // 属性名
            
            // 特殊字符
            换行符,                    // 换行符
            空白字符,                  // 空白字符
            文件结束,                  // 文件结束标记
            未知字符                   // 未知字符
        };
        
        /**
         * CHTL词法单元位置信息
         * 用于错误报告和调试
         */
        struct CHTL词法单元位置 {
            size_t 行号;               // 行号（从1开始）
            size_t 列号;               // 列号（从1开始）
            size_t 偏移量;             // 在文件中的字节偏移量
            
            CHTL词法单元位置(size_t 行 = 1, size_t 列 = 1, size_t 偏移 = 0) 
                : 行号(行), 列号(列), 偏移量(偏移) {}
        };
        
        /**
         * CHTL词法单元
         * 表示从CHTL源代码中解析出的一个语法元素
         * 严格按照CHTL语法文档的词法规则定义
         */
        class CHTL词法单元 {
        private:
            CHTL词法单元类型 类型_;           // 词法单元类型
            std::string 值_;                  // 词法单元值
            CHTL词法单元位置 位置_;           // 位置信息
            
        public:
            CHTL词法单元(CHTL词法单元类型 类型, const std::string& 值, 
                        const CHTL词法单元位置& 位置);
            
            // 访问器方法
            CHTL词法单元类型 获取类型() const { return 类型_; }
            const std::string& 获取值() const { return 值_; }
            const CHTL词法单元位置& 获取位置() const { return 位置_; }
            
            // 类型判断方法（严格按照语法文档分类）
            bool 是关键字() const;
            bool 是类型标识符() const;
            bool 是块标识符() const;
            bool 是字面量() const;
            bool 是操作符() const;
            bool 是注释() const;
            bool 是HTML元素() const;
            bool 是CHTL_JS语法() const;
            
            // 字符串表示方法
            std::string 转为字符串() const;
            
            // 比较操作
            bool operator==(const CHTL词法单元& 其他) const;
            bool operator!=(const CHTL词法单元& 其他) const;
        };
        
        // 工具函数
        std::string 词法单元类型转字符串(CHTL词法单元类型 类型);
        bool 是关键字字符串(const std::string& 字符串);
        CHTL词法单元类型 获取关键字类型(const std::string& 字符串);
        
    } // namespace 核心
} // namespace CHTL