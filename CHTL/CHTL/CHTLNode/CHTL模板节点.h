#pragma once
#include "CHTL基础节点.h"
#include <unordered_set>

namespace CHTL {
    namespace 核心 {
        
        /**
         * CHTL模板节点基类
         * 严格按照CHTL语法文档[Template]定义
         * 支持样式组模板、元素模板、变量组模板
         */
        class CHTL模板节点 : public CHTL基础节点 {
        protected:
            std::string 模板名称_;               // 模板名称
            std::vector<std::string> 继承列表_;  // 继承的模板列表
            std::unordered_set<std::string> 依赖集合_;  // 依赖的其他模板
            
        public:
            // 构造函数
            CHTL模板节点(CHTL节点类型 类型, const std::string& 模板名称);
            
            // 访问器
            const std::string& 获取模板名称() const { return 模板名称_; }
            void 设置模板名称(const std::string& 名称) { 模板名称_ = 名称; }
            
            // 继承管理
            void 添加继承(const std::string& 模板名);
            void 移除继承(const std::string& 模板名);
            const std::vector<std::string>& 获取继承列表() const { return 继承列表_; }
            bool 继承自(const std::string& 模板名) const;
            
            // 依赖管理
            void 添加依赖(const std::string& 依赖模板);
            const std::unordered_set<std::string>& 获取依赖集合() const { return 依赖集合_; }
            bool 依赖于(const std::string& 模板名) const;
            
            // 模板验证
            virtual bool 验证模板() const = 0;
            virtual bool 检查循环依赖() const;
            bool 验证节点() const override;
            
            // 访问者模式
            void 接受访问者(CHTL节点访问器& 访问者) override;
        };
        
        /**
         * CHTL样式组模板节点
         * 严格按照语法文档[Template] @Style定义
         * 用于创建可重用的样式组
         */
        class CHTL样式组模板节点 : public CHTL模板节点 {
        private:
            std::unordered_map<std::string, std::string> 样式属性表_;  // CSS属性键值对
            
        public:
            // 构造函数
            explicit CHTL样式组模板节点(const std::string& 模板名称);
            
            // 样式属性管理
            void 添加样式属性(const std::string& 属性名, const std::string& 属性值);
            void 移除样式属性(const std::string& 属性名);
            std::string 获取样式属性(const std::string& 属性名) const;
            bool 存在样式属性(const std::string& 属性名) const;
            const std::unordered_map<std::string, std::string>& 获取所有样式属性() const { return 样式属性表_; }
            
            // 样式属性批量操作
            void 设置样式属性表(const std::unordered_map<std::string, std::string>& 属性表);
            void 合并样式属性(const std::unordered_map<std::string, std::string>& 其他属性);
            void 清空样式属性();
            
            // 继承处理（语法文档继承规则）
            void 应用继承样式(const CHTL样式组模板节点& 父模板);
            std::unordered_map<std::string, std::string> 解析最终样式() const;
            
            // 模板验证
            bool 验证模板() const override;
            std::vector<std::string> 获取验证错误() const override;
            
            // 克隆
            std::unique_ptr<CHTL基础节点> 克隆() const override;
            
            // 调试输出
            std::string 转为字符串() const override;
            
        private:
            // 内部方法
            bool 验证CSS属性名(const std::string& 属性名) const;
            bool 验证CSS属性值(const std::string& 属性名, const std::string& 属性值) const;
        };
        
        /**
         * CHTL元素模板节点
         * 严格按照语法文档[Template] @Element定义
         * 用于创建可重用的元素组合
         */
        class CHTL元素模板节点 : public CHTL模板节点 {
        public:
            // 构造函数
            explicit CHTL元素模板节点(const std::string& 模板名称);
            
            // 元素管理（使用基类的子节点管理功能）
            void 添加模板元素(std::unique_ptr<CHTL基础节点> 元素);
            void 插入模板元素(size_t 位置, std::unique_ptr<CHTL基础节点> 元素);
            std::vector<CHTL基础节点*> 获取模板元素列表() const;
            
            // 模板实例化
            std::vector<std::unique_ptr<CHTL基础节点>> 实例化模板() const;
            std::unique_ptr<CHTL基础节点> 实例化元素(size_t 索引) const;
            
            // 模板验证
            bool 验证模板() const override;
            std::vector<std::string> 获取验证错误() const override;
            
            // 克隆
            std::unique_ptr<CHTL基础节点> 克隆() const override;
            
            // 调试输出
            std::string 转为字符串() const override;
            
        private:
            // 内部方法
            bool 验证元素节点(const CHTL基础节点* 节点) const;
        };
        
        /**
         * CHTL变量组模板节点
         * 严格按照语法文档[Template] @Var定义
         * 用于创建可重用的变量集合
         */
        class CHTL变量组模板节点 : public CHTL模板节点 {
        private:
            std::unordered_map<std::string, std::string> 变量表_;  // 变量名到值的映射
            
        public:
            // 构造函数
            explicit CHTL变量组模板节点(const std::string& 模板名称);
            
            // 变量管理
            void 添加变量(const std::string& 变量名, const std::string& 变量值);
            void 移除变量(const std::string& 变量名);
            std::string 获取变量值(const std::string& 变量名) const;
            bool 存在变量(const std::string& 变量名) const;
            const std::unordered_map<std::string, std::string>& 获取所有变量() const { return 变量表_; }
            
            // 变量批量操作
            void 设置变量表(const std::unordered_map<std::string, std::string>& 变量表);
            void 合并变量(const std::unordered_map<std::string, std::string>& 其他变量);
            void 清空变量();
            
            // 变量引用解析（语法文档变量组语法：ThemeColor(tableColor)）
            std::string 解析变量引用(const std::string& 变量引用) const;
            std::vector<std::string> 获取变量引用列表(const std::string& 表达式) const;
            
            // 继承处理
            void 应用继承变量(const CHTL变量组模板节点& 父模板);
            std::unordered_map<std::string, std::string> 解析最终变量() const;
            
            // 模板验证
            bool 验证模板() const override;
            std::vector<std::string> 获取验证错误() const override;
            
            // 克隆
            std::unique_ptr<CHTL基础节点> 克隆() const override;
            
            // 调试输出
            std::string 转为字符串() const override;
            
        private:
            // 内部方法
            bool 验证变量名(const std::string& 变量名) const;
            bool 验证变量值(const std::string& 变量值) const;
            bool 验证变量引用格式(const std::string& 引用) const;
        };
        
    } // namespace 核心
} // namespace CHTL