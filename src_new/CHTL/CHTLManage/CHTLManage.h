#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {
namespace Manage {

/**
 * CHTLManage - CHTL管理器统一接口
 * 整合所有CHTL管理功能
 */
class CHTLManage {
public:
    /**
     * 管理器类型
     */
    enum class ManagerType {
        SELECTOR_AUTOMATION,    // 选择器自动化管理
        NAMESPACE,             // 命名空间管理
        CONSTRAINT,            // 约束管理
        INDEX_ACCESS,          // 索引访问管理
        USE_SYNTAX,            // Use语法管理
        UNQUOTED_LITERAL       // 无修饰字面量管理
    };

public:
    /**
     * 初始化所有管理器
     */
    static void initialize();
    
    /**
     * 清理所有管理器
     */
    static void cleanup();
    
    /**
     * 注册管理器
     * @param type 管理器类型
     * @param manager 管理器实例
     */
    static void registerManager(ManagerType type, std::shared_ptr<void> manager);
    
    /**
     * 获取管理器
     * @param type 管理器类型
     * @return 管理器实例
     */
    static std::shared_ptr<void> getManager(ManagerType type);
    
    /**
     * 处理选择器自动化
     * @param selectors 选择器列表
     * @param context 上下文
     * @return 处理结果
     */
    static std::string processSelectorAutomation(const std::vector<std::string>& selectors,
                                                 const std::string& context);
    
    /**
     * 处理命名空间
     * @param namespace_name 命名空间名称
     * @param symbols 符号列表
     * @return 处理结果
     */
    static bool processNamespace(const std::string& namespace_name,
                                 const std::unordered_map<std::string, std::string>& symbols);
    
    /**
     * 处理约束
     * @param constraint_type 约束类型
     * @param context 上下文
     * @return 是否满足约束
     */
    static bool processConstraint(const std::string& constraint_type,
                                 const std::string& context);
    
    /**
     * 处理索引访问
     * @param element 元素
     * @param index 索引
     * @return 访问结果
     */
    static std::string processIndexAccess(const std::string& element,
                                         const std::string& index);
    
    /**
     * 处理Use语法
     * @param use_statement Use语句
     * @return 处理结果
     */
    static std::string processUseSyntax(const std::string& use_statement);
    
    /**
     * 处理无修饰字面量
     * @param literal 字面量
     * @param context 上下文
     * @return 处理结果
     */
    static std::string processUnquotedLiteral(const std::string& literal,
                                             const std::string& context);

private:
    static bool initialized_;
    static std::unordered_map<ManagerType, std::shared_ptr<void>> managers_;
    
    /**
     * 管理器类型转字符串
     * @param type 管理器类型
     * @return 字符串表示
     */
    static std::string managerTypeToString(ManagerType type);
};

} // namespace Manage
} // namespace CHTL