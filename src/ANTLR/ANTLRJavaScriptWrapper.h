#pragma once

#include "../../Util/String.h"
#include "antlr4-runtime.h"
#include "JavaScript/JavaScriptLexer.h"
#include "JavaScript/JavaScriptParser.h"
#include "JavaScript/JavaScriptParserBaseListener.h"

namespace CHTL {
namespace ANTLR {

    /**
     * @brief ANTLR JavaScript解析器包装器
     * 为CHTL项目提供JavaScript解析能力
     */
    class ANTLRJavaScriptWrapper {
    public:
        /**
         * @brief JavaScript解析结果
         */
        struct ParseResult {
            bool success = false;
            String errorMessage;
            String cleanedJavaScript;
            std::vector<String> syntaxErrors;
            std::vector<String> warnings;
            
            // AST信息
            bool hasValidSyntax = false;
            bool hasModules = false;
            bool hasArrowFunctions = false;
            bool hasAsyncAwait = false;
            bool hasClasses = false;
            
            void clear() {
                success = false;
                errorMessage.clear();
                cleanedJavaScript.clear();
                syntaxErrors.clear();
                warnings.clear();
                hasValidSyntax = false;
                hasModules = false;
                hasArrowFunctions = false;
                hasAsyncAwait = false;
                hasClasses = false;
            }
        };

        /**
         * @brief 解析配置
         */
        struct ParseConfig {
            bool strictMode = true;
            bool reportWarnings = true;
            bool extractFeatures = true;
            bool cleanOutput = true;
            bool validateSyntax = true;
            
            // 容错设置
            int maxErrorCount = 10;
            bool bailOnFirstError = false;
            bool recoverFromErrors = true;
        };

        ANTLRJavaScriptWrapper();
        ~ANTLRJavaScriptWrapper();

        /**
         * @brief 解析JavaScript代码
         * @param jsCode JavaScript代码字符串
         * @param config 解析配置
         * @return 解析结果
         */
        ParseResult parseJavaScript(const String& jsCode, const ParseConfig& config = ParseConfig());

        /**
         * @brief 验证JavaScript语法
         * @param jsCode JavaScript代码
         * @return 是否语法正确
         */
        bool validateJavaScriptSyntax(const String& jsCode);

        /**
         * @brief 清理JavaScript代码
         * 移除注释，规范化格式
         * @param jsCode 原始JavaScript代码
         * @return 清理后的JavaScript代码
         */
        String cleanJavaScriptCode(const String& jsCode);

        /**
         * @brief 检测JavaScript特性
         * @param jsCode JavaScript代码
         * @return 特性检测结果
         */
        ParseResult detectJavaScriptFeatures(const String& jsCode);

        /**
         * @brief 获取最后的错误信息
         */
        const String& getLastError() const { return lastError; }

        /**
         * @brief 重置状态
         */
        void reset();

    private:
        String lastError;
        ParseConfig defaultConfig;

        // ANTLR组件
        std::unique_ptr<antlr4::ANTLRInputStream> inputStream;
        std::unique_ptr<JavaScriptLexer> lexer;
        std::unique_ptr<antlr4::CommonTokenStream> tokens;
        std::unique_ptr<JavaScriptParser> parser;

        // 内部辅助方法
        void setupParser(const String& jsCode);
        void collectErrors(ParseResult& result);
        void analyzeAST(JavaScriptParser::ProgramContext* tree, ParseResult& result);
        String extractCleanCode(JavaScriptParser::ProgramContext* tree);
    };

    /**
     * @brief JavaScript语法错误监听器
     */
    class JavaScriptErrorListener : public antlr4::BaseErrorListener {
    public:
        JavaScriptErrorListener(ANTLRJavaScriptWrapper::ParseResult* result);

        void syntaxError(antlr4::Recognizer* recognizer,
                        antlr4::Token* offendingSymbol,
                        size_t line,
                        size_t charPositionInLine,
                        const std::string& msg,
                        std::exception_ptr e) override;

    private:
        ANTLRJavaScriptWrapper::ParseResult* parseResult;
    };

} // namespace ANTLR
} // namespace CHTL