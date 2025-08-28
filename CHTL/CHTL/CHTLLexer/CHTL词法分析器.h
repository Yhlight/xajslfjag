#pragma once
#include "CHTL词法单元.h"
#include "CHTL全局映射表.h"
#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace CHTL {
    namespace 核心 {
        
        /**
         * CHTL词法分析器
         * 负责将CHTL源代码转换为词法单元序列
         * 严格按照CHTL语法文档的词法规则进行解析，绝对不私自扩展
         */
        class CHTL词法分析器 {
        private:
            std::string 源代码_;                        // CHTL源代码
            size_t 当前位置_;                          // 当前解析位置
            size_t 当前行号_;                          // 当前行号（从1开始）
            size_t 当前列号_;                          // 当前列号（从1开始）
            std::vector<CHTL词法单元> 词法单元列表_;    // 解析出的词法单元序列
            
        public:
            explicit CHTL词法分析器(const std::string& 源代码);
            
            // 词法分析主方法
            std::vector<CHTL词法单元> 执行词法分析();
            
            // 获取解析结果
            const std::vector<CHTL词法单元>& 获取词法单元列表() const { return 词法单元列表_; }
            
            // 调试和诊断方法
            void 打印词法单元() const;
            std::string 获取词法单元字符串() const;
            
        private:
            // 内部解析方法
            void 扫描所有词法单元();
            CHTL词法单元 扫描单个词法单元();
            
            // 字符检查方法
            char 查看当前字符() const;
            char 查看下一字符() const;
            char 前进一个字符();
            bool 是否到达文件末尾() const;
            bool 匹配字符(char 期望字符);
            bool 是字母(char 字符) const;
            bool 是数字(char 字符) const;
            bool 是字母或数字(char 字符) const;
            bool 是空白字符(char 字符) const;
            
            // 具体词法单元解析方法（严格按照语法文档）
            CHTL词法单元 扫描字符串字面量(char 引号字符);
            CHTL词法单元 扫描无修饰字面量();
            CHTL词法单元 扫描数字();
            CHTL词法单元 扫描标识符();
            CHTL词法单元 扫描双斜杠注释();
            CHTL词法单元 扫描生成器注释();
            CHTL词法单元 扫描块注释();
            CHTL词法单元 扫描块标识符();
            CHTL词法单元 扫描类型标识符();
            CHTL词法单元 扫描CHTL_JS选择器();
            CHTL词法单元 扫描箭头操作符();
            
            // 关键字和特殊字符处理
            CHTL词法单元类型 获取标识符类型(const std::string& 文本) const;
            CHTL词法单元 创建词法单元(CHTL词法单元类型 类型, const std::string& 值 = "");
            
            // 错误处理和辅助方法
            void 报告错误(const std::string& 错误信息);
            void 跳过空白字符();
            void 前进到下一行();
        };
        
    } // namespace 核心
} // namespace CHTL