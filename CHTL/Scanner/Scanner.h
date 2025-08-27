#pragma once
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief 代码片段类型枚举
 */
enum class FragmentType {
    CHTL,       // CHTL代码片段
    CHTL_JS,    // CHTL JS代码片段
    CSS,        // CSS代码片段
    JS,         // JavaScript代码片段
    HTML,       // HTML代码片段
    UNKNOWN     // 未知类型
};

/**
 * @brief 代码片段结构
 */
struct CodeFragment {
    FragmentType type;          // 片段类型
    std::string content;        // 片段内容
    size_t start_line;         // 起始行号
    size_t start_column;       // 起始列号
    size_t end_line;           // 结束行号
    size_t end_column;         // 结束列号
    size_t start_offset;       // 起始偏移量
    size_t end_offset;         // 结束偏移量
    
    CodeFragment(FragmentType t = FragmentType::UNKNOWN)
        : type(t), start_line(1), start_column(1), end_line(1), end_column(1),
          start_offset(0), end_offset(0) {}
};

/**
 * @brief 统一代码扫描器
 * 负责将CHTL源代码切割为不同类型的代码片段
 * 这是整个编译器架构的基础组件
 */
class Scanner {
private:
    std::string source_;                        // 源代码
    std::vector<CodeFragment> fragments_;       // 切割后的片段列表
    size_t current_pos_;                       // 当前扫描位置
    size_t current_line_;                      // 当前行号
    size_t current_column_;                    // 当前列号
    bool strict_mode_;                         // 严格模式

public:
    /**
     * @brief 构造函数
     * @param source 源代码字符串
     */
    explicit Scanner(const std::string& source = "");

    /**
     * @brief 析构函数
     */
    ~Scanner() = default;

    /**
     * @brief 设置源代码
     * @param source 源代码字符串
     */
    void setSource(const std::string& source);

    /**
     * @brief 获取源代码
     * @return 源代码字符串
     */
    const std::string& getSource() const { return source_; }

    /**
     * @brief 设置严格模式
     * @param strict 是否启用严格模式
     */
    void setStrictMode(bool strict) { strict_mode_ = strict; }

    /**
     * @brief 检查是否为严格模式
     * @return 是否为严格模式
     */
    bool isStrictMode() const { return strict_mode_; }

    /**
     * @brief 执行代码扫描
     * @return 是否扫描成功
     */
    bool scan();

    /**
     * @brief 获取扫描后的片段列表
     * @return 片段列表的常量引用
     */
    const std::vector<CodeFragment>& getFragments() const { return fragments_; }

    /**
     * @brief 获取指定类型的片段
     * @param type 片段类型
     * @return 该类型的片段列表
     */
    std::vector<CodeFragment> getFragmentsByType(FragmentType type) const;

    /**
     * @brief 重置扫描器状态
     */
    void reset();

    /**
     * @brief 验证片段的完整性
     * @return 验证结果消息，空字符串表示验证通过
     */
    std::string validateFragments() const;

    /**
     * @brief 生成片段统计报告
     * @return 统计报告字符串
     */
    std::string generateReport() const;

    /**
     * @brief 片段类型转字符串
     * @param type 片段类型
     * @return 类型字符串
     */
    static std::string fragmentTypeToString(FragmentType type);

private:
    /**
     * @brief 检查是否到达源代码末尾
     * @return 是否到达末尾
     */
    bool isAtEnd() const;

    /**
     * @brief 获取当前字符
     * @return 当前字符
     */
    char peek() const;

    /**
     * @brief 获取下一个字符
     * @return 下一个字符
     */
    char peekNext() const;

    /**
     * @brief 前进一个字符
     * @return 被消费的字符
     */
    char advance();

    /**
     * @brief 检查当前字符是否匹配期望字符
     * @param expected 期望的字符
     * @return 匹配返回true并前进，否则返回false
     */
    bool match(char expected);

    /**
     * @brief 跳过空白字符
     */
    void skipWhitespace();

    /**
     * @brief 扫描CHTL代码块
     * @return 扫描的代码片段
     */
    CodeFragment scanCHTLBlock();

    /**
     * @brief 扫描样式块（style{}）
     * @return 扫描的代码片段
     */
    CodeFragment scanStyleBlock();

    /**
     * @brief 扫描脚本块（script{}）
     * @return 扫描的代码片段
     */
    CodeFragment scanScriptBlock();

    /**
     * @brief 扫描到匹配的右括号
     * @param open_brace 开始括号字符
     * @param close_brace 结束括号字符
     * @return 扫描的内容
     */
    std::string scanToMatchingBrace(char open_brace, char close_brace);

    /**
     * @brief 检测代码片段类型
     * @param content 代码内容
     * @return 片段类型
     */
    FragmentType detectFragmentType(const std::string& content) const;

    /**
     * @brief 更新行列信息
     * @param c 当前字符
     */
    void updatePosition(char c);

    /**
     * @brief 创建代码片段
     * @param type 片段类型
     * @param content 片段内容
     * @param start_pos 起始位置
     * @return 代码片段
     */
    CodeFragment createFragment(FragmentType type, const std::string& content, size_t start_pos);
};

/**
 * @brief Scanner智能指针类型定义
 */
using ScannerPtr = std::shared_ptr<Scanner>;

} // namespace CHTL