#pragma once

// 将所有管理器组件整合到CHTLManage中

// 选择器自动化管理
#include "CHTLManage/SelectorAutomationManager.h"
#include "CHTLManage/SelectorAutomation.h"

// 命名空间管理
#include "CHTLManage/AdvancedNamespaceManager.h"

// 简化约束管理
#include "CHTLManage/SimpleConstraints.h"
#include "CHTLManage/SimpleIndexAccess.h"
#include "CHTLManage/SimpleUseSyntax.h"

// 字面量管理
#include "CHTLManage/UnquotedLiteralManager.h"

// 索引访问管理
#include "CHTLManage/IndexAccessManager.h"

namespace CHTL {
namespace Manage {

/**
 * CHTLManage - CHTL管理器统一接口
 * 整合所有CHTL管理功能
 */
class CHTLManage {
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
     * 获取选择器自动化管理器
     */
    static Common::SelectorAutomationManager& getSelectorAutomationManager();
    
    /**
     * 获取高级命名空间管理器
     */
    static Common::AdvancedNamespaceManager& getAdvancedNamespaceManager();
    
    /**
     * 获取简单约束管理器
     */
    static Common::SimpleConstraints& getSimpleConstraints();
    
    /**
     * 获取简单索引访问管理器
     */
    static Common::SimpleIndexAccess& getSimpleIndexAccess();
    
    /**
     * 获取简单Use语法管理器
     */
    static Common::SimpleUseSyntax& getSimpleUseSyntax();
    
    /**
     * 获取无修饰字面量管理器
     */
    static Common::UnquotedLiteralManager& getUnquotedLiteralManager();
    
    /**
     * 获取索引访问管理器
     */
    static Common::IndexAccessManager& getIndexAccessManager();

private:
    static bool initialized_;
    
    // 管理器实例
    static std::unique_ptr<Common::SelectorAutomationManager> selector_automation_manager_;
    static std::unique_ptr<Common::AdvancedNamespaceManager> advanced_namespace_manager_;
    static std::unique_ptr<Common::SimpleConstraints> simple_constraints_;
    static std::unique_ptr<Common::SimpleIndexAccess> simple_index_access_;
    static std::unique_ptr<Common::SimpleUseSyntax> simple_use_syntax_;
    static std::unique_ptr<Common::UnquotedLiteralManager> unquoted_literal_manager_;
    static std::unique_ptr<Common::IndexAccessManager> index_access_manager_;
};

} // namespace Manage
} // namespace CHTL