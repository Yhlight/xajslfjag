#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {
    namespace 核心 {
        
        // 前向声明
        class CHTL节点访问器;
        
        /**
         * CHTL节点类型枚举
         * 严格按照CHTL语法文档定义的所有节点类型
         */
        enum class CHTL节点类型 {
            // 基础节点类型（语法文档定义）
            元素节点,                  // HTML元素节点
            文本节点,                  // text { } 文本节点
            注释节点,                  // 注释节点（//、/**/、--）
            
            // 模板节点类型（语法文档[Template]定义）
            样式组模板节点,            // [Template] @Style
            元素模板节点,              // [Template] @Element  
            变量组模板节点,            // [Template] @Var
            
            // 自定义节点类型（语法文档[Custom]定义）
            自定义样式组节点,          // [Custom] @Style
            自定义元素节点,            // [Custom] @Element
            自定义变量组节点,          // [Custom] @Var
            
            // 原始嵌入节点（语法文档[Origin]定义）
            原始HTML节点,              // [Origin] @Html
            原始样式节点,              // [Origin] @Style  
            原始脚本节点,              // [Origin] @JavaScript
            自定义原始节点,            // [Origin] @自定义类型
            
            // 样式节点（语法文档局部样式块定义）
            局部样式节点,              // style { }
            内联样式节点,              // 元素内部的样式属性
            类选择器节点,              // .className
            ID选择器节点,              // #idName
            伪类选择器节点,            // &:hover
            伪元素选择器节点,          // &::before
            
            // 脚本节点（语法文档局部脚本定义）
            局部脚本节点,              // script { }
            
            // 导入节点（语法文档[Import]定义）
            导入HTML节点,              // [Import] @Html
            导入样式节点,              // [Import] @Style
            导入脚本节点,              // [Import] @JavaScript
            导入CHTL节点,              // [Import] @Chtl
            导入CJMOD节点,             // [Import] @CJmod
            导入配置节点,              // [Import] @Config
            导入模板节点,              // [Import] [Template] @XXX
            导入自定义节点,            // [Import] [Custom] @XXX
            导入原始节点,              // [Import] [Origin] @XXX
            
            // 配置节点（语法文档[Configuration]定义）
            配置组节点,                // [Configuration]
            名称配置节点,              // [Name]
            原始类型配置节点,          // [OriginType]
            
            // 命名空间节点（语法文档[Namespace]定义）
            命名空间节点,              // [Namespace]
            
            // 操作节点（语法文档操作定义）
            继承节点,                  // inherit
            删除节点,                  // delete
            插入节点,                  // insert
            使用节点,                  // use
            约束节点,                  // except
            
            // 特殊节点
            根节点,                    // 文档根节点
            属性节点,                  // HTML属性节点
            变量引用节点,              // 变量组引用
            模板引用节点,              // 模板引用
            自定义引用节点,            // 自定义引用
            命名空间引用节点           // 命名空间引用
        };
        
        /**
         * CHTL基础节点
         * 所有CHTL AST节点的基类
         * 严格按照CHTL语法文档的节点设计原则
         */
        class CHTL基础节点 {
        protected:
            CHTL节点类型 节点类型_;              // 节点类型
            std::string 节点名称_;               // 节点名称
            std::string 节点值_;                 // 节点值
            CHTL基础节点* 父节点_;               // 父节点指针
            std::vector<std::unique_ptr<CHTL基础节点>> 子节点列表_;  // 子节点列表
            std::unordered_map<std::string, std::string> 属性表_;    // 属性表
            
            // 位置信息
            size_t 起始行号_;
            size_t 起始列号_;
            size_t 结束行号_;
            size_t 结束列号_;
            
        public:
            // 构造函数
            CHTL基础节点(CHTL节点类型 类型, const std::string& 名称 = "", const std::string& 值 = "");
            
            // 虚析构函数
            virtual ~CHTL基础节点() = default;
            
            // 基础访问器
            CHTL节点类型 获取节点类型() const { return 节点类型_; }
            const std::string& 获取节点名称() const { return 节点名称_; }
            const std::string& 获取节点值() const { return 节点值_; }
            CHTL基础节点* 获取父节点() const { return 父节点_; }
            const std::vector<std::unique_ptr<CHTL基础节点>>& 获取子节点列表() const { return 子节点列表_; }
            
            // 基础修改器
            void 设置节点名称(const std::string& 名称) { 节点名称_ = 名称; }
            void 设置节点值(const std::string& 值) { 节点值_ = 值; }
            void 设置父节点(CHTL基础节点* 父节点) { 父节点_ = 父节点; }
            
            // 子节点管理
            void 添加子节点(std::unique_ptr<CHTL基础节点> 子节点);
            void 插入子节点(size_t 位置, std::unique_ptr<CHTL基础节点> 子节点);
            void 移除子节点(size_t 位置);
            void 移除子节点(CHTL基础节点* 节点);
            size_t 获取子节点数量() const { return 子节点列表_.size(); }
            CHTL基础节点* 获取子节点(size_t 索引) const;
            
            // 属性管理
            void 设置属性(const std::string& 键, const std::string& 值);
            std::string 获取属性(const std::string& 键) const;
            bool 存在属性(const std::string& 键) const;
            void 移除属性(const std::string& 键);
            const std::unordered_map<std::string, std::string>& 获取所有属性() const { return 属性表_; }
            
            // 位置信息管理
            void 设置位置信息(size_t 起始行, size_t 起始列, size_t 结束行, size_t 结束列);
            void 获取位置信息(size_t& 起始行, size_t& 起始列, size_t& 结束行, size_t& 结束列) const;
            
            // 节点查找
            std::vector<CHTL基础节点*> 查找子节点(CHTL节点类型 类型) const;
            std::vector<CHTL基础节点*> 查找子节点(const std::string& 名称) const;
            CHTL基础节点* 查找第一个子节点(CHTL节点类型 类型) const;
            CHTL基础节点* 查找第一个子节点(const std::string& 名称) const;
            
            // 节点验证（严格按照语法文档）
            virtual bool 验证节点() const;
            virtual std::vector<std::string> 获取验证错误() const;
            
            // 节点克隆
            virtual std::unique_ptr<CHTL基础节点> 克隆() const;
            
            // 访问者模式支持
            virtual void 接受访问者(CHTL节点访问器& 访问者) = 0;
            
            // 调试和诊断
            virtual std::string 转为字符串() const;
            virtual void 打印节点树(int 缩进 = 0) const;
            
            // 节点类型判断
            bool 是元素节点() const;
            bool 是文本节点() const;
            bool 是模板节点() const;
            bool 是自定义节点() const;
            bool 是原始节点() const;
            bool 是样式节点() const;
            bool 是脚本节点() const;
            bool 是导入节点() const;
            bool 是配置节点() const;
            bool 是命名空间节点() const;
            bool 是操作节点() const;
        };
        
        // 节点类型到字符串的转换
        std::string 节点类型转字符串(CHTL节点类型 类型);
        
        // 节点创建工厂方法
        std::unique_ptr<CHTL基础节点> 创建节点(CHTL节点类型 类型, const std::string& 名称 = "", const std::string& 值 = "");
        
    } // namespace 核心
} // namespace CHTL