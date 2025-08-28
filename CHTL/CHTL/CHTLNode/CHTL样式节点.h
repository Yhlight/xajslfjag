#pragma once
#include "CHTL基础节点.h"
#include <unordered_set>

namespace CHTL {
    namespace 核心 {
        
        /**
         * CSS选择器类型枚举
         * 严格按照CHTL语法文档局部样式块支持的选择器类型
         */
        enum class CSS选择器类型 {
            内联样式,                  // 直接在style{}内部的属性
            类选择器,                  // .className
            ID选择器,                  // #idName  
            伪类选择器,                // &:hover
            伪元素选择器,              // &::before
            引用选择器,                // & (上下文推导)
            复合选择器,                // .class1.class2
            后代选择器,                // .parent .child
            子选择器,                  // .parent > .child
            相邻选择器,                // .element + .sibling
            兄弟选择器,                // .element ~ .sibling
            未知选择器
        };
        
        /**
         * CHTL局部样式节点
         * 严格按照CHTL语法文档的局部样式块定义
         * 支持内联样式、类/ID选择器、伪类/伪元素选择器
         */
        class CHTL局部样式节点 : public CHTL基础节点 {
        private:
            // 内联样式属性（直接在style{}内部的属性）
            std::unordered_map<std::string, std::string> 内联样式属性表_;
            
            // 选择器样式映射
            struct 选择器样式 {
                CSS选择器类型 选择器类型;
                std::string 选择器名称;
                std::unordered_map<std::string, std::string> 样式属性;
                bool 自动生成;              // 是否为自动生成的选择器
                
                选择器样式() : 选择器类型(CSS选择器类型::未知选择器), 自动生成(false) {}
            };
            
            std::vector<选择器样式> 选择器样式列表_;
            
            // 自动化配置
            bool 自动添加类选择器_ = true;
            bool 自动添加ID选择器_ = true;
            
        public:
            // 构造函数
            CHTL局部样式节点();
            explicit CHTL局部样式节点(const std::unordered_map<std::string, std::string>& 内联样式);
            
            // 内联样式管理
            void 添加内联样式(const std::string& 属性名, const std::string& 属性值);
            void 移除内联样式(const std::string& 属性名);
            std::string 获取内联样式(const std::string& 属性名) const;
            bool 存在内联样式(const std::string& 属性名) const;
            const std::unordered_map<std::string, std::string>& 获取所有内联样式() const { return 内联样式属性表_; }
            void 设置内联样式表(const std::unordered_map<std::string, std::string>& 样式表);
            void 清空内联样式();
            
            // 选择器样式管理
            void 添加类选择器(const std::string& 类名, const std::unordered_map<std::string, std::string>& 样式);
            void 添加ID选择器(const std::string& ID名, const std::unordered_map<std::string, std::string>& 样式);
            void 添加伪类选择器(const std::string& 伪类名, const std::unordered_map<std::string, std::string>& 样式);
            void 添加伪元素选择器(const std::string& 伪元素名, const std::unordered_map<std::string, std::string>& 样式);
            void 添加引用选择器(const std::unordered_map<std::string, std::string>& 样式);
            void 添加自定义选择器(CSS选择器类型 类型, const std::string& 选择器名, const std::unordered_map<std::string, std::string>& 样式);
            
            // 选择器查询
            std::vector<选择器样式*> 获取类选择器列表();
            std::vector<选择器样式*> 获取ID选择器列表();
            选择器样式* 查找选择器(const std::string& 选择器名);
            选择器样式* 查找选择器(CSS选择器类型 类型, const std::string& 选择器名);
            const std::vector<选择器样式>& 获取所有选择器样式() const { return 选择器样式列表_; }
            
            // 自动化管理（语法文档自动化类名/ID功能）
            void 启用自动类选择器(bool 启用) { 自动添加类选择器_ = 启用; }
            void 启用自动ID选择器(bool 启用) { 自动添加ID选择器_ = 启用; }
            bool 是否自动添加类选择器() const { return 自动添加类选择器_; }
            bool 是否自动添加ID选择器() const { return 自动添加ID选择器_; }
            
            // 自动化操作
            std::string 自动生成类名() const;
            std::string 自动生成ID名() const;
            void 应用自动化类选择器(const std::string& 类名 = "");
            void 应用自动化ID选择器(const std::string& ID名 = "");
            
            // 上下文推导（语法文档&引用功能）
            void 设置引用上下文(const std::string& 类名, const std::string& ID名);
            std::string 解析引用选择器() const;  // 将&解析为具体的类名或ID
            
            // 样式合并和继承
            void 合并样式(const CHTL局部样式节点& 其他样式);
            void 应用模板样式(const std::string& 模板名);
            void 应用自定义样式(const std::string& 自定义名);
            
            // 全局样式块生成（语法文档：类/ID/伪类选择器自动添加至全局样式块）
            std::vector<std::string> 生成全局CSS规则() const;
            std::string 生成内联样式字符串() const;
            
            // 节点验证
            bool 验证节点() const override;
            std::vector<std::string> 获取验证错误() const override;
            
            // 访问者模式
            void 接受访问者(CHTL节点访问器& 访问者) override;
            
            // 克隆
            std::unique_ptr<CHTL基础节点> 克隆() const override;
            
            // 调试输出
            std::string 转为字符串() const override;
            
        private:
            // 内部方法
            bool 验证CSS属性名(const std::string& 属性名) const;
            bool 验证CSS属性值(const std::string& 属性名, const std::string& 属性值) const;
            bool 验证选择器名称(CSS选择器类型 类型, const std::string& 名称) const;
            std::string 生成选择器字符串(const 选择器样式& 样式) const;
            CSS选择器类型 解析选择器类型(const std::string& 选择器) const;
            void 自动分配元素属性();  // 自动为元素添加class/id属性
        };
        
        /**
         * CHTL局部脚本节点
         * 严格按照CHTL语法文档的局部脚本块定义
         * 支持增强选择器、引用功能、事件绑定
         */
        class CHTL局部脚本节点 : public CHTL基础节点 {
        private:
            std::string 脚本内容_;               // JavaScript代码内容
            std::vector<std::string> 增强选择器列表_;  // {{.box}}等增强选择器
            std::vector<std::string> 引用列表_;       // {{&}}引用
            std::vector<std::string> 事件绑定列表_;   // &->语法
            
            // 自动化配置（与局部样式的自动化协调）
            bool 自动添加类选择器_ = true;
            bool 自动添加ID选择器_ = true;
            
        public:
            // 构造函数
            CHTL局部脚本节点();
            explicit CHTL局部脚本节点(const std::string& 脚本内容);
            
            // 脚本内容管理
            void 设置脚本内容(const std::string& 内容) { 脚本内容_ = 内容; }
            const std::string& 获取脚本内容() const { return 脚本内容_; }
            void 追加脚本内容(const std::string& 内容);
            void 清空脚本内容() { 脚本内容_.clear(); }
            
            // 增强选择器管理（语法文档{{CSS选择器}}功能）
            void 添加增强选择器(const std::string& 选择器);
            void 移除增强选择器(const std::string& 选择器);
            const std::vector<std::string>& 获取增强选择器列表() const { return 增强选择器列表_; }
            bool 包含增强选择器(const std::string& 选择器) const;
            
            // 引用管理（语法文档{{&}}功能）
            void 添加引用(const std::string& 引用);
            const std::vector<std::string>& 获取引用列表() const { return 引用列表_; }
            
            // 事件绑定管理（语法文档&->功能）
            void 添加事件绑定(const std::string& 绑定);
            const std::vector<std::string>& 获取事件绑定列表() const { return 事件绑定列表_; }
            
            // 自动化管理（语法文档选择器自动化与引用规则）
            void 启用自动类选择器(bool 启用) { 自动添加类选择器_ = 启用; }
            void 启用自动ID选择器(bool 启用) { 自动添加ID选择器_ = 启用; }
            bool 是否自动添加类选择器() const { return 自动添加类选择器_; }
            bool 是否自动添加ID选择器() const { return 自动添加ID选择器_; }
            
            // 选择器自动化应用
            void 应用自动化选择器();
            std::string 获取第一个类选择器() const;
            std::string 获取第一个ID选择器() const;
            
            // 上下文推导（语法文档引用规则）
            void 设置引用上下文(const std::string& 类名, const std::string& ID名);
            std::string 解析引用选择器() const;  // 将{{&}}解析为具体的选择器
            
            // 脚本解析和处理
            std::string 解析增强选择器(const std::string& 原始脚本) const;
            std::string 生成最终JavaScript代码() const;
            std::vector<std::string> 提取选择器引用(const std::string& 脚本) const;
            
            // 节点验证
            bool 验证节点() const override;
            std::vector<std::string> 获取验证错误() const override;
            
            // 访问者模式
            void 接受访问者(CHTL节点访问器& 访问者) override;
            
            // 克隆
            std::unique_ptr<CHTL基础节点> 克隆() const override;
            
            // 调试输出
            std::string 转为字符串() const override;
            
        private:
            // 内部方法
            bool 验证JavaScript语法(const std::string& 代码) const;
            bool 验证增强选择器格式(const std::string& 选择器) const;
            bool 验证事件绑定格式(const std::string& 绑定) const;
            std::vector<std::string> 扫描脚本中的选择器() const;
            std::string 处理选择器替换(const std::string& 原始代码) const;
            void 自动分配元素属性();  // 与局部样式协调的自动化
        };
        
        // 工具函数
        std::string CSS选择器类型转字符串(CSS选择器类型 类型);
        CSS选择器类型 字符串转CSS选择器类型(const std::string& 字符串);
        bool 是有效CSS选择器(const std::string& 选择器);
        bool 是增强选择器格式(const std::string& 选择器);
        
    } // namespace 核心
} // namespace CHTL