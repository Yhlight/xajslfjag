#pragma once
#include "CHTL基础节点.h"
#include "../CHTLLexer/CHTL全局映射表.h"

namespace CHTL {
    namespace 核心 {
        
        /**
         * CHTL元素节点
         * 表示HTML元素节点，严格按照CHTL语法文档定义
         * 支持所有HTML标签和属性
         */
        class CHTL元素节点 : public CHTL基础节点 {
        private:
            HTML标签类型 标签类型_;              // HTML标签类型
            bool 是自闭合标签_;                  // 是否为自闭合标签
            bool 是块级元素_;                    // 是否为块级元素
            std::unique_ptr<CHTL基础节点> 局部样式节点_;   // 局部样式块
            std::unique_ptr<CHTL基础节点> 局部脚本节点_;   // 局部脚本块
            
        public:
            // 构造函数
            explicit CHTL元素节点(const std::string& 标签名);
            CHTL元素节点(const std::string& 标签名, const std::string& 内容);
            
            // 访问器
            HTML标签类型 获取标签类型() const { return 标签类型_; }
            std::string 获取标签名() const;
            bool 是自闭合标签() const { return 是自闭合标签_; }
            bool 是块级元素() const { return 是块级元素_; }
            
            // 局部样式块管理
            void 设置局部样式(std::unique_ptr<CHTL基础节点> 样式节点);
            CHTL基础节点* 获取局部样式() const { return 局部样式节点_.get(); }
            bool 存在局部样式() const { return 局部样式节点_ != nullptr; }
            
            // 局部脚本块管理
            void 设置局部脚本(std::unique_ptr<CHTL基础节点> 脚本节点);
            CHTL基础节点* 获取局部脚本() const { return 局部脚本节点_.get(); }
            bool 存在局部脚本() const { return 局部脚本节点_ != nullptr; }
            
            // HTML属性管理（扩展基类的属性功能）
            void 设置ID(const std::string& id);
            std::string 获取ID() const;
            void 设置类名(const std::string& 类名);
            void 添加类名(const std::string& 类名);
            void 移除类名(const std::string& 类名);
            std::vector<std::string> 获取类名列表() const;
            bool 包含类名(const std::string& 类名) const;
            
            // HTML标准属性
            void 设置样式属性(const std::string& 样式);
            std::string 获取样式属性() const;
            void 设置标题(const std::string& 标题);
            std::string 获取标题() const;
            void 设置数据属性(const std::string& 键, const std::string& 值);
            std::string 获取数据属性(const std::string& 键) const;
            
            // 内容管理
            void 添加文本内容(const std::string& 文本);
            void 设置文本内容(const std::string& 文本);
            std::string 获取文本内容() const;
            void 清空内容();
            
            // 子元素管理
            void 添加子元素(std::unique_ptr<CHTL元素节点> 子元素);
            void 插入子元素(size_t 位置, std::unique_ptr<CHTL元素节点> 子元素);
            CHTL元素节点* 获取子元素(size_t 索引) const;
            std::vector<CHTL元素节点*> 获取所有子元素() const;
            
            // 元素查找
            CHTL元素节点* 根据ID查找元素(const std::string& id) const;
            std::vector<CHTL元素节点*> 根据类名查找元素(const std::string& 类名) const;
            std::vector<CHTL元素节点*> 根据标签名查找元素(const std::string& 标签名) const;
            
            // 节点验证（严格按照语法文档）
            bool 验证节点() const override;
            std::vector<std::string> 获取验证错误() const override;
            
            // 特殊验证方法
            bool 验证属性() const;
            bool 验证子节点() const;
            bool 验证局部样式() const;
            bool 验证局部脚本() const;
            
            // 访问者模式
            void 接受访问者(CHTL节点访问器& 访问者) override;
            
            // 克隆
            std::unique_ptr<CHTL基础节点> 克隆() const override;
            
            // 调试输出
            std::string 转为字符串() const override;
            void 打印节点树(int 缩进 = 0) const override;
            
        private:
            // 内部方法
            void 初始化标签信息(const std::string& 标签名);
            void 验证标签兼容性() const;
            std::string 生成自动类名() const;
            std::string 生成自动ID() const;
        };
        
        /**
         * CHTL文本节点
         * 表示text { }语法定义的文本节点
         */
        class CHTL文本节点 : public CHTL基础节点 {
        private:
            std::string 文本内容_;              // 文本内容
            bool 是无修饰字面量_;               // 是否为无修饰字面量
            bool 是字符串字面量_;               // 是否为字符串字面量
            
        public:
            // 构造函数
            explicit CHTL文本节点(const std::string& 内容);
            CHTL文本节点(const std::string& 内容, bool 是无修饰字面量);
            
            // 访问器
            const std::string& 获取文本内容() const { return 文本内容_; }
            void 设置文本内容(const std::string& 内容) { 文本内容_ = 内容; }
            bool 是无修饰字面量() const { return 是无修饰字面量_; }
            bool 是字符串字面量() const { return 是字符串字面量_; }
            
            // 文本处理
            void 追加文本(const std::string& 文本);
            void 前置文本(const std::string& 文本);
            size_t 获取文本长度() const;
            bool 是空文本() const;
            
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
            void 检测字面量类型(const std::string& 内容);
            std::string 转义文本内容() const;
        };
        
    } // namespace 核心
} // namespace CHTL