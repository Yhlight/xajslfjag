#pragma once

#include "../Util/String.h"

// 包含您提供的ANTLR JavaScript文件
#include "../../js/JavaScriptLexer.h"
#include "../../js/JavaScriptParser.h"
#include "../../js/JavaScriptParserBaseListener.h"
#include "../../js/JavaScriptLexerBase.h"
#include "../../js/JavaScriptParserBase.h"

// ANTLR4运行时
#include "../../ANTLR4/include/antlr4-runtime.h"

namespace CHTL {
namespace ANTLR {

    /**
     * @brief ANTLR JavaScript解析器包装器
     * 使用您提供的Windows ANTLR4环境
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
            
            // AST特性检测
            bool hasValidSyntax = false;
            bool hasModules = false;
            bool hasArrowFunctions = false;
            bool hasAsyncAwait = false;
            bool hasClasses = false;
            bool hasES6Features = false;
            
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
                hasES6Features = false;
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
            
            // Windows环境优化
            bool useWindowsLineEndings = true;
            bool handleUTF8BOM = true;
            
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
         * @brief 清理JavaScript代码 - 为CHTL的"完整JS字符串"需求服务
         * 移除注释，规范化格式，确保输出是纯净的JavaScript
         * @param jsCode 原始JavaScript代码
         * @return 清理后的完整JavaScript字符串
         */
        String cleanJavaScriptCode(const String& jsCode);

        /**
         * @brief 检测JavaScript特性
         * @param jsCode JavaScript代码
         * @return 特性检测结果
         */
        ParseResult detectJavaScriptFeatures(const String& jsCode);

        /**
         * @brief 处理混合的CHTL JS和纯JavaScript代码
         * 这是为了解决您提到的"JS编译器需要接收完整的JS字符串"问题
         * @param mixedCode 包含CHTL JS语法和纯JavaScript的混合代码
         * @return 分离后的纯JavaScript代码
         */
        String extractPureJavaScript(const String& mixedCode);

        /**
         * @brief 获取最后的错误信息
         */
        const String& getLastError() const { return lastError; }

        /**
         * @brief 重置状态
         */
        void reset();

        /**
         * @brief 检查Windows环境兼容性
         */
        static bool checkWindowsCompatibility();

    private:
        String lastError;
        ParseConfig defaultConfig;

        // ANTLR组件（使用您提供的文件）
        std::unique_ptr<antlr4::ANTLRInputStream> inputStream;
        std::unique_ptr<JavaScriptLexer> lexer;
        std::unique_ptr<antlr4::CommonTokenStream> tokens;
        std::unique_ptr<JavaScriptParser> parser;

        // 内部辅助方法
        void setupParser(const String& jsCode);
        void collectErrors(ParseResult& result);
        void analyzeAST(JavaScriptParser::ProgramContext* tree, ParseResult& result);
        String extractCleanCode(JavaScriptParser::ProgramContext* tree);
        String normalizeLineEndings(const String& code, bool useWindows = true);
        String handleUTF8BOM(const String& code);
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