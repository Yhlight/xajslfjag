#pragma once
#include "../../CHTL/CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

namespace CHTL {
namespace Test {

/**
 * Token表格显示工具类
 * 提供表格形式的Token展示功能
 */
class TokenTable {
public:
    /**
     * 表格列定义
     */
    enum class TableColumn {
        INDEX,          // 索引
        TYPE,           // 类型
        VALUE,          // 值
        LINE,           // 行号
        COLUMN,         // 列号
        OFFSET,         // 偏移量
        LENGTH,         // 长度
        CATEGORY        // 分类
    };
    
    /**
     * 表格样式
     */
    enum class TableStyle {
        SIMPLE,         // 简单样式
        ASCII,          // ASCII艺术样式
        UNICODE,        // Unicode样式
        HTML,           // HTML表格
        MARKDOWN        // Markdown表格
    };
    
    /**
     * 构造函数
     */
    TokenTable(TableStyle style = TableStyle::ASCII);
    
    /**
     * 设置表格样式
     */
    void setStyle(TableStyle style);
    
    /**
     * 设置显示的列
     */
    void setColumns(const std::vector<TableColumn>& columns);
    
    /**
     * 设置列宽
     */
    void setColumnWidth(TableColumn column, size_t width);
    
    /**
     * 启用/禁用行号
     */
    void setShowRowNumbers(bool show);
    
    /**
     * 启用/禁用颜色
     */
    void setUseColors(bool useColors);
    
    /**
     * 显示Token序列为表格
     */
    void displayTokens(const TokenSequence& tokens);
    
    /**
     * 生成Token表格字符串
     */
    std::string generateTable(const TokenSequence& tokens);
    
    /**
     * 保存表格到文件
     */
    bool saveTableToFile(const TokenSequence& tokens, const std::string& filename);
    
    /**
     * 显示Token统计表格
     */
    void displayTokenStatistics(const TokenSequence& tokens);
    
    /**
     * 显示Token类型分布表格
     */
    void displayTokenTypeDistribution(const TokenSequence& tokens);
    
    /**
     * 显示Token错误表格
     */
    void displayTokenErrors(const TokenSequence& tokens);
    
    /**
     * 按页显示Token（分页）
     */
    void displayTokensPaged(const TokenSequence& tokens, size_t pageSize = 20);

private:
    TableStyle m_style;                                     // 表格样式
    std::vector<TableColumn> m_columns;                     // 显示的列
    std::unordered_map<TableColumn, size_t> m_columnWidths; // 列宽
    bool m_showRowNumbers;                                  // 是否显示行号
    bool m_useColors;                                       // 是否使用颜色
    
    /**
     * 获取列标题
     */
    std::string getColumnHeader(TableColumn column) const;
    
    /**
     * 获取Token的列值
     */
    std::string getTokenColumnValue(const Token& token, TableColumn column, size_t index = 0) const;
    
    /**
     * 格式化单元格内容
     */
    std::string formatCell(const std::string& content, size_t width, bool rightAlign = false) const;
    
    /**
     * 生成水平分隔线
     */
    std::string generateHorizontalLine(const std::vector<size_t>& columnWidths) const;
    
    /**
     * 生成表格头部
     */
    std::string generateTableHeader(const std::vector<size_t>& columnWidths) const;
    
    /**
     * 生成表格行
     */
    std::string generateTableRow(const Token& token, size_t index, const std::vector<size_t>& columnWidths) const;
    
    /**
     * 计算列宽
     */
    std::vector<size_t> calculateColumnWidths(const TokenSequence& tokens) const;
    
    /**
     * 获取Token类型的颜色
     */
    std::string getTokenTypeColor(TokenType type) const;
    
    /**
     * 重置颜色
     */
    std::string resetColor() const;
    
    /**
     * 转义HTML字符
     */
    std::string escapeHtml(const std::string& text) const;
    
    /**
     * 转义Markdown字符
     */
    std::string escapeMarkdown(const std::string& text) const;
    
    /**
     * 生成HTML表格
     */
    std::string generateHtmlTable(const TokenSequence& tokens) const;
    
    /**
     * 生成Markdown表格
     */
    std::string generateMarkdownTable(const TokenSequence& tokens) const;
};

/**
 * Token矩阵显示工具类
 * 提供矩阵形式的Token可视化
 */
class TokenMatrix {
public:
    /**
     * 显示Token的位置矩阵
     */
    static void displayPositionMatrix(const TokenSequence& tokens);
    
    /**
     * 显示Token类型的热力图
     */
    static void displayTypeHeatMap(const TokenSequence& tokens, size_t width = 80);
    
    /**
     * 显示Token密度图
     */
    static void displayDensityMap(const TokenSequence& tokens, size_t width = 80, size_t height = 20);
    
    /**
     * 生成Token流图
     */
    static std::string generateFlowChart(const TokenSequence& tokens);
    
    /**
     * 生成Token树状图
     */
    static std::string generateTreeView(const TokenSequence& tokens);

private:
    /**
     * 获取Token类型的符号
     */
    static char getTokenTypeSymbol(TokenType type);
    
    /**
     * 获取Token密度的符号
     */
    static char getDensitySymbol(double density);
};

/**
 * Token导出工具类
 */
class TokenExporter {
public:
    /**
     * 导出为JSON格式
     */
    static std::string exportToJson(const TokenSequence& tokens, bool prettyPrint = true);
    
    /**
     * 导出为XML格式
     */
    static std::string exportToXml(const TokenSequence& tokens);
    
    /**
     * 导出为CSV格式
     */
    static std::string exportToCsv(const TokenSequence& tokens);
    
    /**
     * 导出为TSV格式
     */
    static std::string exportToTsv(const TokenSequence& tokens);
    
    /**
     * 导出为SQLite数据库
     */
    static bool exportToSqlite(const TokenSequence& tokens, const std::string& dbPath);
    
    /**
     * 导出为二进制格式
     */
    static bool exportToBinary(const TokenSequence& tokens, const std::string& filePath);
    
    /**
     * 从二进制格式导入
     */
    static TokenSequence importFromBinary(const std::string& filePath);
};

} // namespace Test
} // namespace CHTL