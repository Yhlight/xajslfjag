#pragma once

#include "BaseNode/BaseNode.h"

namespace CHTL {
namespace Node {

/**
 * ImportNode - 导入节点
 * 处理[Import]语法的所有导入类型
 */
class ImportNode : public BaseNode {
public:
    enum class ImportType {
        HTML,           // @Html
        STYLE,          // @Style
        JAVASCRIPT,     // @JavaScript
        CHTL,           // @Chtl
        CJMOD,          // @CJmod
        TEMPLATE,       // [Template]
        CUSTOM,         // [Custom]
        ORIGIN,         // [Origin]
        CONFIG          // @Config
    };

private:
    ImportType import_type_;
    std::string import_path_;
    std::string alias_name_;            // as语法的别名
    bool has_alias_;                    // 是否有别名
    bool is_official_prefix_;           // 是否使用官方模块前缀（chtl::）
    bool is_wildcard_;                  // 是否为通配符导入（.*或/*）
    bool is_submodule_;                 // 是否为子模块导入（Module.Submodule）
    std::vector<std::string> specific_items_; // 具体导入项目
    
public:
    /**
     * 构造函数
     * @param type 导入类型
     * @param path 导入路径
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    ImportNode(ImportType type,
               const std::string& path,
               const std::string& location = "", 
               int line = 0, 
               int column = 0);
    
    /**
     * 获取导入类型
     * @return 导入类型
     */
    ImportType getImportType() const;
    
    /**
     * 设置导入类型
     * @param type 导入类型
     */
    void setImportType(ImportType type);
    
    /**
     * 获取导入路径
     * @return 导入路径
     */
    const std::string& getImportPath() const;
    
    /**
     * 设置导入路径
     * @param path 导入路径
     */
    void setImportPath(const std::string& path);
    
    /**
     * 获取别名
     * @return 别名
     */
    const std::string& getAlias() const;
    
    /**
     * 设置别名
     * @param alias 别名
     */
    void setAlias(const std::string& alias);
    
    /**
     * 是否有别名
     * @return 是否有别名
     */
    bool hasAlias() const;
    
    /**
     * 是否使用官方模块前缀
     * @return 是否使用官方模块前缀
     */
    bool isOfficialPrefix() const;
    
    /**
     * 设置是否使用官方模块前缀
     * @param is_official 是否使用官方模块前缀
     */
    void setOfficialPrefix(bool is_official);
    
    /**
     * 是否为通配符导入
     * @return 是否为通配符导入
     */
    bool isWildcard() const;
    
    /**
     * 设置是否为通配符导入
     * @param is_wildcard 是否为通配符导入
     */
    void setWildcard(bool is_wildcard);
    
    /**
     * 是否为子模块导入
     * @return 是否为子模块导入
     */
    bool isSubmodule() const;
    
    /**
     * 设置是否为子模块导入
     * @param is_submodule 是否为子模块导入
     */
    void setSubmodule(bool is_submodule);
    
    /**
     * 添加具体导入项目
     * @param item 导入项目
     */
    void addSpecificItem(const std::string& item);
    
    /**
     * 获取具体导入项目列表
     * @return 导入项目列表
     */
    const std::vector<std::string>& getSpecificItems() const;
    
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
     * 解析导入路径
     * @return 解析结果
     */
    bool parseImportPath();
    
    /**
     * 解析官方模块前缀
     * @param path 路径
     * @return 是否为官方模块前缀
     */
    static bool parseOfficialPrefix(const std::string& path);
    
    /**
     * 解析通配符
     * @param path 路径
     * @return 是否为通配符
     */
    static bool parseWildcard(const std::string& path);
    
    /**
     * 解析子模块
     * @param path 路径
     * @return 是否为子模块
     */
    static bool parseSubmodule(const std::string& path);
    
    /**
     * 导入类型转字符串
     * @param type 导入类型
     * @return 字符串
     */
    static std::string importTypeToString(ImportType type);
    
    /**
     * 字符串转导入类型
     * @param type_str 类型字符串
     * @return 导入类型
     */
    static ImportType stringToImportType(const std::string& type_str);
};

} // namespace Node
} // namespace CHTL