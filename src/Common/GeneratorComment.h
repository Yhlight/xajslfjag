#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {
namespace Common {

// 生成器注释的目标语言类型
enum class CommentTargetLanguage {
    HTML,           // HTML注释 <!-- -->
    CSS,            // CSS注释 /* */
    JAVASCRIPT,     // JavaScript注释 // 或 /* */
    CHTL,           // CHTL注释（保持原样或转换）
    AUTO_DETECT     // 自动检测目标语言
};

// 注释风格
enum class CommentStyle {
    SINGLE_LINE,    // 单行注释 //
    MULTI_LINE,     // 多行注释 /* */
    HTML_STYLE,     // HTML风格 <!-- -->
    AUTO            // 根据内容自动选择
};

// 上下文检测结果
enum class CommentContext {
    GLOBAL_HTML,        // 全局HTML上下文
    GLOBAL_STYLE,       // 全局样式块
    GLOBAL_SCRIPT,      // 全局脚本块
    LOCAL_STYLE,        // 局部样式块
    LOCAL_SCRIPT,       // 局部脚本块
    TEMPLATE_BLOCK,     // 模板块内
    CUSTOM_BLOCK,       // 自定义块内
    ORIGIN_HTML,        // 原始HTML嵌入
    ORIGIN_CSS,         // 原始CSS嵌入
    ORIGIN_JS,          // 原始JavaScript嵌入
    CONFIGURATION,      // 配置块
    NAMESPACE,          // 命名空间
    IMPORT,             // 导入块
    TEXT_NODE,          // 文本节点内
    UNKNOWN
};

// 生成器注释信息
struct GeneratorComment {
    std::string original_content;   // 原始--注释内容
    std::string generated_content;  // 生成后的注释内容
    CommentTargetLanguage target;   // 目标语言
    CommentStyle style;             // 注释风格
    CommentContext context;         // 上下文
    size_t line_number;             // 行号
    size_t column_number;           // 列号
    bool is_processed;              // 是否已处理
    
    GeneratorComment() : target(CommentTargetLanguage::AUTO_DETECT), 
                        style(CommentStyle::AUTO), 
                        context(CommentContext::UNKNOWN),
                        line_number(0), column_number(0), is_processed(false) {}
    
    GeneratorComment(const std::string& content, CommentContext ctx, size_t line, size_t col)
        : original_content(content), context(ctx), line_number(line), column_number(col),
          target(CommentTargetLanguage::AUTO_DETECT), style(CommentStyle::AUTO), 
          is_processed(false) {}
    
    bool needsProcessing() const { return !is_processed && !original_content.empty(); }
    
    std::string getLocationInfo() const {
        return "line " + std::to_string(line_number) + ", column " + std::to_string(column_number);
    }
};

// 生成器注释处理结果
struct CommentProcessResult {
    std::string processed_content;              // 处理后的完整内容
    std::vector<GeneratorComment> comments;     // 所有处理的注释
    std::vector<std::string> warnings;         // 警告信息
    bool success;                               // 是否成功
    
    CommentProcessResult() : success(false) {}
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasWarnings() const { return !warnings.empty(); }
    size_t getCommentCount() const { return comments.size(); }
};

// 生成器注释管理器
class GeneratorCommentManager {
public:
    GeneratorCommentManager();
    ~GeneratorCommentManager();
    
    // 配置
    void setDefaultTargetLanguage(CommentTargetLanguage lang) { default_target = lang; }
    void setDefaultCommentStyle(CommentStyle style) { default_style = style; }
    void setStrictContextDetection(bool strict) { strict_context = strict; }
    void setPreserveOriginalOnError(bool preserve) { preserve_on_error = preserve; }
    
    // 核心处理功能
    
    // 处理整个源代码中的所有--注释
    CommentProcessResult processAllComments(const std::string& source_code);
    
    // 处理单个--注释
    GeneratorComment processSingleComment(const std::string& comment_content, 
                                         CommentContext context);
    
    // 在指定上下文中生成注释
    std::string generateComment(const std::string& content, CommentContext context);
    
    // 上下文检测
    
    // 检测--注释所在的上下文
    CommentContext detectCommentContext(const std::string& source_code, size_t comment_position);
    
    // 检查是否在特定块内
    bool isInGlobalStyleBlock(const std::string& source, size_t position);
    bool isInGlobalScriptBlock(const std::string& source, size_t position);
    bool isInLocalStyleBlock(const std::string& source, size_t position);
    bool isInLocalScriptBlock(const std::string& source, size_t position);
    bool isInOriginBlock(const std::string& source, size_t position);
    bool isInTemplateBlock(const std::string& source, size_t position);
    bool isInConfigurationBlock(const std::string& source, size_t position);
    
    // 获取原始嵌入块的类型
    CommentTargetLanguage getOriginBlockType(const std::string& source, size_t position);
    
    // 注释生成规则
    
    // 根据上下文确定目标语言
    CommentTargetLanguage determineTargetLanguage(CommentContext context);
    
    // 根据上下文和目标语言确定注释风格
    CommentStyle determineCommentStyle(CommentContext context, CommentTargetLanguage target);
    
    // 生成特定语言的注释
    std::string generateHTMLComment(const std::string& content, CommentStyle style = CommentStyle::HTML_STYLE);
    std::string generateCSSComment(const std::string& content, CommentStyle style = CommentStyle::MULTI_LINE);
    std::string generateJavaScriptComment(const std::string& content, CommentStyle style = CommentStyle::AUTO);
    std::string generateCHTLComment(const std::string& content, CommentStyle style = CommentStyle::SINGLE_LINE);
    
    // 注释检测和解析
    
    // 查找所有--注释
    std::vector<std::pair<size_t, std::string>> findAllGeneratorComments(const std::string& source);
    
    // 解析--注释内容
    std::string parseCommentContent(const std::string& raw_comment);
    
    // 检查是否是有效的--注释
    bool isValidGeneratorComment(const std::string& comment);
    
    // 验证和优化
    
    // 验证生成的注释语法
    bool validateGeneratedComment(const std::string& comment, CommentTargetLanguage target);
    
    // 优化注释内容（去除多余空格、格式化等）
    std::string optimizeCommentContent(const std::string& content);
    
    // 检查注释内容是否需要转义
    std::string escapeCommentContent(const std::string& content, CommentTargetLanguage target);
    
    // 多行注释处理
    
    // 处理多行--注释
    std::string processMultiLineComment(const std::string& content, CommentTargetLanguage target);
    
    // 格式化多行注释
    std::string formatMultiLineComment(const std::vector<std::string>& lines, 
                                      CommentTargetLanguage target);
    
    // 状态查询
    
    // 获取支持的目标语言
    std::vector<CommentTargetLanguage> getSupportedLanguages() const;
    
    // 获取语言的默认注释风格
    CommentStyle getDefaultStyleForLanguage(CommentTargetLanguage lang) const;
    
    // 检查上下文是否支持特定语言
    bool isLanguageSupportedInContext(CommentTargetLanguage lang, CommentContext context) const;
    
    // 统计和分析
    
    // 统计不同上下文中的注释数量
    std::unordered_map<CommentContext, size_t> getCommentStatistics(const std::string& source);
    
    // 分析注释分布
    std::unordered_map<CommentTargetLanguage, size_t> getLanguageDistribution(const std::string& source);
    
    // 调试和诊断
    void dumpCommentAnalysis(const std::string& source);
    std::vector<std::string> getProcessingWarnings() const { return processing_warnings; }
    void clearWarnings() { processing_warnings.clear(); }
    
    // 配置规则
    
    // 设置上下文到语言的映射规则
    void setContextLanguageMapping(CommentContext context, CommentTargetLanguage language);
    
    // 设置语言的默认注释风格
    void setLanguageDefaultStyle(CommentTargetLanguage language, CommentStyle style);
    
    // 应用预设配置
    void applyStandardConfig();   // 应用标准CHTL配置
    void applyWebConfig();        // 应用Web开发配置
    void applyMinimalConfig();    // 应用最小化配置

private:
    // 配置选项
    CommentTargetLanguage default_target;
    CommentStyle default_style;
    bool strict_context;
    bool preserve_on_error;
    
    // 映射规则
    std::unordered_map<CommentContext, CommentTargetLanguage> context_language_map;
    std::unordered_map<CommentTargetLanguage, CommentStyle> language_style_map;
    
    // 处理状态
    std::vector<std::string> processing_warnings;
    
    // 内部方法
    
    // 上下文分析
    CommentContext analyzeContext(const std::string& source, size_t position);
    std::pair<size_t, size_t> findEnclosingBlock(const std::string& source, size_t position);
    std::string getBlockType(const std::string& source, size_t block_start);
    
    // 块边界检测
    bool isInBlock(const std::string& source, size_t position, const std::string& block_type);
    std::pair<size_t, size_t> findBlockBoundaries(const std::string& source, size_t position);
    
    // 注释语法生成
    std::string wrapSingleLineComment(const std::string& content, const std::string& prefix);
    std::string wrapMultiLineComment(const std::string& content, 
                                    const std::string& start, const std::string& end);
    
    // 内容处理
    std::vector<std::string> splitIntoLines(const std::string& content);
    std::string trimComment(const std::string& content);
    bool containsUnsafeCharacters(const std::string& content, CommentTargetLanguage target);
    
    // 验证
    bool isValidHTMLComment(const std::string& comment);
    bool isValidCSSComment(const std::string& comment);
    bool isValidJSComment(const std::string& comment);
    
    // 工具方法
    std::string getLanguageName(CommentTargetLanguage lang) const;
    std::string getStyleName(CommentStyle style) const;
    std::string getContextName(CommentContext context) const;
    
    // 错误处理
    void addWarning(const std::string& warning);
    std::string generateFallbackComment(const std::string& content);
};

// 生成器注释工具类
class CommentUtils {
public:
    // 快速生成HTML注释
    static std::string toHTML(const std::string& content);
    
    // 快速生成CSS注释
    static std::string toCSS(const std::string& content);
    
    // 快速生成JavaScript注释
    static std::string toJS(const std::string& content, bool multiline = false);
    
    // 检测注释语言
    static CommentTargetLanguage detectLanguage(const std::string& context);
    
    // 验证注释内容安全性
    static bool isSafeContent(const std::string& content, CommentTargetLanguage target);
    
    // 规范化注释内容
    static std::string normalizeContent(const std::string& content);
    
    // 计算注释长度（考虑多行）
    static size_t calculateCommentLength(const std::string& content, CommentTargetLanguage target);
    
    // 检查是否需要多行注释
    static bool needsMultiLine(const std::string& content);
    
    // 获取注释前缀和后缀
    static std::pair<std::string, std::string> getCommentDelimiters(CommentTargetLanguage target, 
                                                                   CommentStyle style);
};

} // namespace Common
} // namespace CHTL