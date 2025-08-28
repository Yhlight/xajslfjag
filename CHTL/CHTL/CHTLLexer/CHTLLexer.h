#pragma once
#include "CHTLToken.h"
#include "CHTLGlobalMap.h"
#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace CHTL {
    namespace Core {
        
        /**
         * CHTL词法分析器
         * 负责将CHTL源代码转换为词法单元序列
         * 严格按照CHTL语法文档的词法规则进行解析
         */
        class CHTLLexer {
        private:
            std::string source_;           // 源代码
            size_t position_;              // 当前位置
            size_t line_;                  // 当前行号
            size_t column_;                // 当前列号
            std::vector<CHTLToken> tokens_; // 解析出的词法单元
            
        public:
            explicit CHTLLexer(const std::string& source);
            
            // 词法分析主方法
            std::vector<CHTLToken> Tokenize();
            
            // 获取解析结果
            const std::vector<CHTLToken>& GetTokens() const { return tokens_; }
            
            // 调试和诊断方法
            void PrintTokens() const;
            std::string GetTokensAsString() const;
            
        private:
            // 内部解析方法
            void ScanTokens();
            CHTLToken ScanToken();
            
            // 字符检查方法
            char Peek() const;
            char PeekNext() const;
            char Advance();
            bool IsAtEnd() const;
            bool Match(char expected);
            bool IsAlpha(char c) const;
            bool IsDigit(char c) const;
            bool IsAlphaNumeric(char c) const;
            bool IsWhitespace(char c) const;
            
            // 具体词法单元解析方法
            CHTLToken ScanString(char quote);
            CHTLToken ScanUnquotedLiteral();
            CHTLToken ScanNumber();
            CHTLToken ScanIdentifier();
            CHTLToken ScanComment();
            CHTLToken ScanGeneratorComment();
            CHTLToken ScanBlockComment();
            CHTLToken ScanBlockIdentifier();
            CHTLToken ScanTypeIdentifier();
            CHTLToken ScanSelector();
            
            // 关键字和特殊字符处理
            CHTLTokenType GetIdentifierType(const std::string& text) const;
            CHTLToken CreateToken(CHTLTokenType type, const std::string& value = "");
            
            // 错误处理
            void Error(const std::string& message);
            void SkipWhitespace();
            void AdvanceLine();
        };
        
    } // namespace Core
} // namespace CHTL