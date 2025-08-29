#pragma once

#include "../Util/String.h"

// 包含您提供的ANTLR CSS文件
#include "../../css/css3Lexer.h"
#include "../../css/css3Parser.h"
#include "../../css/css3ParserBaseListener.h"

// ANTLR4运行时
#include "../../ANTLR4/include/antlr4-runtime.h"

namespace CHTL {
namespace ANTLR {

    /**
     * @brief ANTLR CSS解析器包装器
     * 使用您提供的Windows ANTLR4环境
     */
    class ANTLRCSSWrapper {
    public:
        /**
         * @brief CSS解析结果
         */
        struct ParseResult {
            bool success = false;
            String errorMessage;
            String cleanedCSS;
            std::vector<String> syntaxErrors;
            std::vector<String> warnings;
            
            // CSS特性检测
            bool hasValidSyntax = false;
            bool hasMediaQueries = false;
            bool hasKeyframes = false;
            bool hasCustomProperties = false;
            bool hasGridLayout = false;
            bool hasFlexbox = false;
            bool hasSelectors = false;
            
            // CHTL CSS特性
            bool hasCHTLSelectors = false;
            bool hasReferenceSelectors = false;
            
            void clear() {
                success = false;
                errorMessage.clear();
                cleanedCSS.clear();
                syntaxErrors.clear();
                warnings.clear();
                hasValidSyntax = false;
                hasMediaQueries = false;
                hasKeyframes = false;
                hasCustomProperties = false;
                hasGridLayout = false;
                hasFlexbox = false;
                hasSelectors = false;
                hasCHTLSelectors = false;
                hasReferenceSelectors = false;
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
            
            // CHTL CSS特定设置
            bool processReferenceSelectors = true;
            bool autoAddClasses = true;
            bool autoAddIds = false;
            
            // Windows环境优化
            bool useWindowsLineEndings = true;
            bool handleUTF8BOM = true;
            
            // 容错设置
            int maxErrorCount = 10;
            bool bailOnFirstError = false;
            bool recoverFromErrors = true;
        };

        ANTLRCSSWrapper();
        ~ANTLRCSSWrapper();

        /**
         * @brief 解析CSS代码
         * @param cssCode CSS代码字符串
         * @param config 解析配置
         * @return 解析结果
         */
        ParseResult parseCSS(const String& cssCode, const ParseConfig& config = ParseConfig());

        /**
         * @brief 验证CSS语法
         * @param cssCode CSS代码
         * @return 是否语法正确
         */
        bool validateCSSSyntax(const String& cssCode);

        /**
         * @brief 清理CSS代码
         * 移除注释，规范化格式
         * @param cssCode 原始CSS代码
         * @return 清理后的CSS代码
         */
        String cleanCSSCode(const String& cssCode);

        /**
         * @brief 检测CSS特性
         * @param cssCode CSS代码
         * @return 特性检测结果
         */
        ParseResult detectCSSFeatures(const String& cssCode);

        /**
         * @brief 处理CHTL CSS特性
         * 包括引用选择器(&)和自动添加类/ID
         * @param cssCode 包含CHTL CSS语法的代码
         * @return 处理后的标准CSS代码
         */
        String processCHTLCSS(const String& cssCode);

        /**
         * @brief 提取选择器列表
         * @param cssCode CSS代码
         * @return 选择器列表
         */
        std::vector<String> extractSelectors(const String& cssCode);

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
        std::unique_ptr<css3Lexer> lexer;
        std::unique_ptr<antlr4::CommonTokenStream> tokens;
        std::unique_ptr<css3Parser> parser;

        // 内部辅助方法
        void setupParser(const String& cssCode);
        void collectErrors(ParseResult& result);
        void analyzeAST(css3Parser::StylesheetContext* tree, ParseResult& result);
        String extractCleanCode(css3Parser::StylesheetContext* tree);
        String normalizeLineEndings(const String& code, bool useWindows = true);
        String handleUTF8BOM(const String& code);
        String processReferenceSelectors(const String& cssCode);
    };

    /**
     * @brief CSS语法错误监听器
     */
    class CSSErrorListener : public antlr4::BaseErrorListener {
    public:
        CSSErrorListener(ANTLRCSSWrapper::ParseResult* result);

        void syntaxError(antlr4::Recognizer* recognizer,
                        antlr4::Token* offendingSymbol,
                        size_t line,
                        size_t charPositionInLine,
                        const std::string& msg,
                        std::exception_ptr e) override;

    private:
        ANTLRCSSWrapper::ParseResult* parseResult;
    };

} // namespace ANTLR
} // namespace CHTL