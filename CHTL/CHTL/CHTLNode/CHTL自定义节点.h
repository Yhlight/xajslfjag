#pragma once
#include "CHTL模板节点.h"
#include <unordered_set>

namespace CHTL {
    namespace 核心 {
        
        /**
         * 特例化操作类型枚举
         * 严格按照CHTL语法文档中自定义的特例化操作定义
         */
        enum class 特例化操作类型 {
            删除属性,                  // delete property
            删除继承,                  // delete inherit
            删除元素,                  // delete element
            插入元素,                  // insert element
            替换元素,                  // replace element
            修改属性,                  // modify property
            添加样式,                  // add style
            无操作                     // no operation
        };
        
        /**
         * 插入位置枚举
         * 严格按照语法文档insert语法定义
         */
        enum class 插入位置类型 {
            之后,                      // after
            之前,                      // before
            替换,                      // replace
            顶部,                      // at top
            底部                       // at bottom
        };
        
        /**
         * CHTL自定义节点基类
         * 严格按照CHTL语法文档[Custom]定义
         * 支持特例化操作，提供极高的灵活性
         */
        class CHTL自定义节点 : public CHTL基础节点 {
        protected:
            std::string 自定义名称_;               // 自定义名称
            std::vector<std::string> 继承列表_;    // 继承的模板或自定义列表
            std::unordered_set<std::string> 依赖集合_;  // 依赖的其他自定义或模板
            
            // 特例化操作列表
            struct 特例化操作 {
                特例化操作类型 操作类型;
                std::string 目标名称;          // 操作目标
                std::string 操作值;            // 操作值或内容
                插入位置类型 插入位置;         // 插入操作的位置
                std::string 选择器;            // 元素选择器（如div[1]）
                
                特例化操作() : 操作类型(特例化操作类型::无操作), 插入位置(插入位置类型::之后) {}
            };
            
            std::vector<特例化操作> 特例化操作列表_;
            
        public:
            // 构造函数
            CHTL自定义节点(CHTL节点类型 类型, const std::string& 自定义名称);
            
            // 访问器
            const std::string& 获取自定义名称() const { return 自定义名称_; }
            void 设置自定义名称(const std::string& 名称) { 自定义名称_ = 名称; }
            
            // 继承管理
            void 添加继承(const std::string& 继承名);
            void 移除继承(const std::string& 继承名);
            const std::vector<std::string>& 获取继承列表() const { return 继承列表_; }
            bool 继承自(const std::string& 继承名) const;
            
            // 依赖管理
            void 添加依赖(const std::string& 依赖名);
            const std::unordered_set<std::string>& 获取依赖集合() const { return 依赖集合_; }
            bool 依赖于(const std::string& 依赖名) const;
            
            // 特例化操作管理
            void 添加删除属性操作(const std::string& 属性名);
            void 添加删除继承操作(const std::string& 继承名);
            void 添加删除元素操作(const std::string& 元素选择器);
            void 添加插入元素操作(插入位置类型 位置, const std::string& 选择器, const std::string& 内容);
            void 添加修改属性操作(const std::string& 属性名, const std::string& 新值);
            const std::vector<特例化操作>& 获取特例化操作列表() const { return 特例化操作列表_; }
            
            // 特例化验证
            virtual bool 验证特例化操作() const;
            virtual bool 检查循环依赖() const;
            bool 验证节点() const override;
            
            // 访问者模式
            void 接受访问者(CHTL节点访问器& 访问者) override;
            
        protected:
            // 内部方法
            bool 验证选择器格式(const std::string& 选择器) const;
            int 解析元素索引(const std::string& 选择器) const;
            std::string 提取元素名称(const std::string& 选择器) const;
        };
        
        /**
         * CHTL自定义样式组节点
         * 严格按照语法文档[Custom] @Style定义
         * 支持无值样式组和样式组特例化
         */
        class CHTL自定义样式组节点 : public CHTL自定义节点 {
        private:
            std::unordered_map<std::string, std::string> 样式属性表_;      // 有值样式属性
            std::unordered_set<std::string> 无值样式属性集合_;             // 无值样式属性
            std::unordered_map<std::string, std::string> 特例化值表_;      // 特例化时提供的值
            
        public:
            // 构造函数
            explicit CHTL自定义样式组节点(const std::string& 自定义名称);
            
            // 有值样式属性管理
            void 添加样式属性(const std::string& 属性名, const std::string& 属性值);
            void 移除样式属性(const std::string& 属性名);
            std::string 获取样式属性(const std::string& 属性名) const;
            bool 存在样式属性(const std::string& 属性名) const;
            
            // 无值样式属性管理（语法文档无值样式组功能）
            void 添加无值样式属性(const std::string& 属性名);
            void 移除无值样式属性(const std::string& 属性名);
            bool 是无值样式属性(const std::string& 属性名) const;
            const std::unordered_set<std::string>& 获取无值样式属性集合() const { return 无值样式属性集合_; }
            
            // 特例化值管理（使用时填入的值）
            void 设置特例化值(const std::string& 属性名, const std::string& 值);
            std::string 获取特例化值(const std::string& 属性名) const;
            bool 存在特例化值(const std::string& 属性名) const;
            void 设置特例化值表(const std::unordered_map<std::string, std::string>& 值表);
            
            // 样式解析（处理继承和特例化）
            std::unordered_map<std::string, std::string> 解析最终样式() const;
            bool 验证特例化值完整性() const;  // 检查所有无值属性是否都有特例化值
            
            // 删除操作（语法文档delete功能）
            void 应用删除属性操作(const std::string& 属性名);
            void 应用删除继承操作(const std::string& 继承名);
            
            // 自定义验证
            bool 验证特例化操作() const override;
            std::vector<std::string> 获取验证错误() const override;
            
            // 克隆
            std::unique_ptr<CHTL基础节点> 克隆() const override;
            
            // 调试输出
            std::string 转为字符串() const override;
            
        private:
            // 内部方法
            bool 验证无值属性名(const std::string& 属性名) const;
            void 应用继承样式(const std::string& 继承名);
        };
        
        /**
         * CHTL自定义元素节点
         * 严格按照语法文档[Custom] @Element定义
         * 支持索引访问、插入元素、删除元素等特例化操作
         */
        class CHTL自定义元素节点 : public CHTL自定义节点 {
        public:
            // 构造函数
            explicit CHTL自定义元素节点(const std::string& 自定义名称);
            
            // 元素管理（使用基类的子节点管理功能）
            void 添加自定义元素(std::unique_ptr<CHTL基础节点> 元素);
            void 插入自定义元素(size_t 位置, std::unique_ptr<CHTL基础节点> 元素);
            std::vector<CHTL基础节点*> 获取自定义元素列表() const;
            
            // 索引访问（语法文档div[1]功能）
            CHTL基础节点* 获取元素(const std::string& 选择器) const;
            CHTL基础节点* 获取元素(size_t 索引) const;
            std::vector<CHTL基础节点*> 获取同类型元素(const std::string& 元素类型) const;
            
            // 特例化操作应用
            void 应用删除元素操作(const std::string& 选择器);
            void 应用插入元素操作(插入位置类型 位置, const std::string& 选择器, std::unique_ptr<CHTL基础节点> 元素);
            void 应用替换元素操作(const std::string& 选择器, std::unique_ptr<CHTL基础节点> 新元素);
            void 应用增加样式操作(const std::string& 选择器, std::unique_ptr<CHTL基础节点> 样式节点);
            
            // 元素实例化
            std::vector<std::unique_ptr<CHTL基础节点>> 实例化自定义元素() const;
            
            // 自定义验证
            bool 验证特例化操作() const override;
            std::vector<std::string> 获取验证错误() const override;
            
            // 克隆
            std::unique_ptr<CHTL基础节点> 克隆() const override;
            
            // 调试输出
            std::string 转为字符串() const override;
            
        private:
            // 内部方法
            size_t 解析索引选择器(const std::string& 选择器) const;
            std::string 解析元素类型选择器(const std::string& 选择器) const;
            bool 验证插入位置(插入位置类型 位置, const std::string& 选择器) const;
        };
        
        /**
         * CHTL自定义变量组节点
         * 严格按照语法文档[Custom] @Var定义
         * 支持变量组特例化
         */
        class CHTL自定义变量组节点 : public CHTL自定义节点 {
        private:
            std::unordered_map<std::string, std::string> 变量表_;          // 变量名到值的映射
            std::unordered_map<std::string, std::string> 特例化值表_;      // 特例化时提供的值
            
        public:
            // 构造函数
            explicit CHTL自定义变量组节点(const std::string& 自定义名称);
            
            // 变量管理
            void 添加变量(const std::string& 变量名, const std::string& 变量值);
            void 移除变量(const std::string& 变量名);
            std::string 获取变量值(const std::string& 变量名) const;
            bool 存在变量(const std::string& 变量名) const;
            
            // 特例化值管理（语法文档变量组特例化：ThemeColor(tableColor = rgb(145, 155, 200))）
            void 设置特例化值(const std::string& 变量名, const std::string& 值);
            std::string 获取特例化值(const std::string& 变量名) const;
            bool 存在特例化值(const std::string& 变量名) const;
            void 设置特例化值表(const std::unordered_map<std::string, std::string>& 值表);
            
            // 变量解析（处理继承和特例化）
            std::unordered_map<std::string, std::string> 解析最终变量() const;
            std::string 解析变量引用(const std::string& 变量引用) const;
            
            // 自定义验证
            bool 验证特例化操作() const override;
            std::vector<std::string> 获取验证错误() const override;
            
            // 克隆
            std::unique_ptr<CHTL基础节点> 克隆() const override;
            
            // 调试输出
            std::string 转为字符串() const override;
            
        private:
            // 内部方法
            bool 验证变量特例化格式(const std::string& 变量引用) const;
            void 应用继承变量(const std::string& 继承名);
        };
        
        // 工具函数
        std::string 特例化操作类型转字符串(特例化操作类型 类型);
        std::string 插入位置类型转字符串(插入位置类型 类型);
        
    } // namespace 核心
} // namespace CHTL