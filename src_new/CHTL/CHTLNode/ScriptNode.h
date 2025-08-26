#pragma once

#include "BaseNode/BaseNode.h"

namespace CHTL {
namespace Node {

/**
 * ScriptNode - 脚本节点
 * 处理script块的JavaScript/CHTL JS代码
 */
class ScriptNode : public BaseNode {
public:
    enum class ScriptType {
        PURE_JAVASCRIPT,    // 纯JavaScript
        CHTL_JS,           // CHTL JS语法
        MIXED              // 混合
    };

private:
    std::string script_content_;
    ScriptType script_type_;
    bool is_local_;                     // 是否为局部脚本
    std::string parent_context_;        // 父上下文（局部脚本时）
    std::vector<std::string> chtljs_fragments_; // CHTL JS片段
    std::vector<std::string> js_fragments_;     // 纯JS片段
    bool auto_add_class_;              // 是否自动添加class
    bool auto_add_id_;                 // 是否自动添加id
    
public:
    /**
     * 构造函数
     * @param script_content 脚本内容
     * @param is_local 是否为局部脚本
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    ScriptNode(const std::string& script_content,
               bool is_local = false,
               const std::string& location = "", 
               int line = 0, 
               int column = 0);
    
    /**
     * 获取脚本内容
     * @return 脚本内容
     */
    const std::string& getScriptContent() const;
    
    /**
     * 设置脚本内容
     * @param content 脚本内容
     */
    void setScriptContent(const std::string& content);
    
    /**
     * 获取脚本类型
     * @return 脚本类型
     */
    ScriptType getScriptType() const;
    
    /**
     * 设置脚本类型
     * @param type 脚本类型
     */
    void setScriptType(ScriptType type);
    
    /**
     * 是否为局部脚本
     * @return 是否为局部脚本
     */
    bool isLocal() const;
    
    /**
     * 设置是否为局部脚本
     * @param is_local 是否为局部脚本
     */
    void setLocal(bool is_local);
    
    /**
     * 获取父上下文
     * @return 父上下文
     */
    const std::string& getParentContext() const;
    
    /**
     * 设置父上下文
     * @param context 父上下文
     */
    void setParentContext(const std::string& context);
    
    /**
     * 获取CHTL JS片段
     * @return CHTL JS片段列表
     */
    const std::vector<std::string>& getCHTLJSFragments() const;
    
    /**
     * 获取纯JS片段
     * @return 纯JS片段列表
     */
    const std::vector<std::string>& getJSFragments() const;
    
    /**
     * 是否自动添加class
     * @return 是否自动添加
     */
    bool shouldAutoAddClass() const;
    
    /**
     * 设置是否自动添加class
     * @param auto_add 是否自动添加
     */
    void setAutoAddClass(bool auto_add);
    
    /**
     * 是否自动添加id
     * @return 是否自动添加
     */
    bool shouldAutoAddId() const;
    
    /**
     * 设置是否自动添加id
     * @param auto_add 是否自动添加
     */
    void setAutoAddId(bool auto_add);
    
    /**
     * 转换为字符串表示
     * @return 字符串表示
     */
    std::string toString() const override;
    
    /**
     * 接受访问者模式
     * @param visitor 访问者
     */
    void accept(NodeVisitor& visitor) override;
    
    /**
     * 克隆节点
     * @return 克隆的节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * 分析脚本类型
     */
    void analyzeScriptType();
    
    /**
     * 分离CHTL JS和纯JS
     */
    void separateFragments();
    
    /**
     * 生成最终JavaScript
     * @return 生成的JavaScript
     */
    std::string generateJavaScript() const;
    
    /**
     * 脚本类型转字符串
     * @param type 脚本类型
     * @return 字符串
     */
    static std::string scriptTypeToString(ScriptType type);
};

} // namespace Node
} // namespace CHTL