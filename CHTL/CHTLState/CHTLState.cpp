#include "CHTLState.h"
#include <iostream>
#include <stdexcept>

namespace CHTL {

// StateGuard实现
StateGuard::StateGuard(CHTLState* manager, ParseState newState, BlockType blockType)
    : stateManager(manager), committed(false) {
    if (stateManager) {
        stateManager->pushState(newState, blockType);
    }
}

StateGuard::~StateGuard() {
    if (stateManager && !committed) {
        stateManager->popState();
    }
}

void StateGuard::setBlockInfo(const std::string& name, const std::string& type) {
    if (stateManager && !stateManager->stateStack.empty()) {
        stateManager->stateStack.top().blockName = name;
        stateManager->stateStack.top().blockTypeStr = type;
    }
}

// CHTLState实现
CHTLState::CHTLState() : currentState(ParseState::TOP_LEVEL) {
    // 初始化顶层状态
    StateContext topLevel(ParseState::TOP_LEVEL, BlockType::NONE);
    stateStack.push(topLevel);
}

void CHTLState::pushState(ParseState newState, BlockType blockType) {
    // 验证状态转换是否有效
    if (!isValidTransition(currentState, newState)) {
        throw std::runtime_error("Invalid state transition from " + 
                                stateToString(currentState) + " to " + 
                                stateToString(newState));
    }
    
    StateContext newContext(newState, blockType);
    
    // 根据状态设置特殊标志
    if (newState == ParseState::IN_STYLE && isInGlobalScope()) {
        newContext.isGlobalStyle = true;
    } else if (newState == ParseState::IN_SCRIPT && isInGlobalScope()) {
        newContext.isGlobalScript = true;
    }
    
    stateStack.push(newContext);
    currentState = newState;
    updateConstraints();
}

void CHTLState::popState() {
    if (stateStack.size() <= 1) {
        throw std::runtime_error("Cannot pop the root state");
    }
    
    stateStack.pop();
    currentState = stateStack.top().state;
    updateConstraints();
}

StateContext CHTLState::getCurrentContext() const {
    if (stateStack.empty()) {
        return StateContext();
    }
    return stateStack.top();
}

std::unique_ptr<StateGuard> CHTLState::createStateGuard(ParseState newState, BlockType blockType) {
    return std::make_unique<StateGuard>(this, newState, blockType);
}

bool CHTLState::isValidTransition(ParseState from, ParseState to) const {
    // 定义有效的状态转换规则
    // 这里根据CHTL语法规则实现
    
    // 从顶层可以进入的状态
    if (from == ParseState::TOP_LEVEL) {
        return to == ParseState::IN_USE_DECLARATION ||
               to == ParseState::IN_NAMESPACE ||
               to == ParseState::IN_CONFIGURATION ||
               to == ParseState::IN_TEMPLATE ||
               to == ParseState::IN_CUSTOM ||
               to == ParseState::IN_ORIGIN ||
               to == ParseState::IN_IMPORT ||
               to == ParseState::IN_ELEMENT ||
               to == ParseState::IN_STYLE ||   // 全局样式
               to == ParseState::IN_SCRIPT ||  // 全局脚本
               to == ParseState::IN_COMMENT;
    }
    
    // 从元素内部可以进入的状态
    if (from == ParseState::IN_ELEMENT) {
        return to == ParseState::IN_ELEMENT ||      // 嵌套元素
               to == ParseState::IN_TEXT ||
               to == ParseState::IN_STYLE ||        // 局部样式
               to == ParseState::IN_SCRIPT ||       // 局部脚本
               to == ParseState::IN_ATTRIBUTE ||
               to == ParseState::IN_COMMENT ||
               to == ParseState::IN_SPECIALIZATION;
    }
    
    // 从模板/自定义定义可以进入的状态
    if (from == ParseState::IN_TEMPLATE || from == ParseState::IN_CUSTOM) {
        return to == ParseState::IN_ELEMENT ||
               to == ParseState::IN_STYLE_RULE ||
               to == ParseState::IN_COMMENT ||
               to == ParseState::IN_ATTRIBUTE;
    }
    
    // 其他转换规则...
    return true; // 暂时允许所有其他转换
}

void CHTLState::updateConstraints() {
    // 根据当前状态更新允许的元素约束
    allowedInCurrentContext.clear();
    
    switch (currentState) {
        case ParseState::IN_STYLE:
            if (getCurrentContext().isGlobalStyle) {
                // 全局样式块允许的CHTL语法
                allowedInCurrentContext.insert("@Style");
                allowedInCurrentContext.insert("@Var");
                allowedInCurrentContext.insert("from");
                allowedInCurrentContext.insert("delete");
                allowedInCurrentContext.insert("inherit");
                allowedInCurrentContext.insert("[Origin]");
                allowedInCurrentContext.insert("--");
            } else {
                // 局部样式块允许的CHTL语法
                allowedInCurrentContext.insert("@Style");
                allowedInCurrentContext.insert("@Var");
                allowedInCurrentContext.insert("&");
                allowedInCurrentContext.insert(".");
                allowedInCurrentContext.insert("#");
                allowedInCurrentContext.insert("from");
                allowedInCurrentContext.insert("delete");
                allowedInCurrentContext.insert("inherit");
                allowedInCurrentContext.insert("[Origin]");
                allowedInCurrentContext.insert("--");
            }
            break;
            
        case ParseState::IN_SCRIPT:
            if (!getCurrentContext().isGlobalScript) {
                // 局部脚本块允许的CHTL语法
                allowedInCurrentContext.insert("@Var");
                allowedInCurrentContext.insert("from");
                allowedInCurrentContext.insert("[Origin]");
                allowedInCurrentContext.insert("--");
            } else {
                // 全局脚本块只允许原始嵌入和生成器注释
                allowedInCurrentContext.insert("[Origin]");
                allowedInCurrentContext.insert("--");
            }
            break;
            
        default:
            // 其他状态的约束...
            break;
    }
}

// 状态查询方法
bool CHTLState::isInState(ParseState state) const {
    return currentState == state;
}

bool CHTLState::isInBlock(BlockType blockType) const {
    for (size_t i = 0; i < stateStack.size(); ++i) {
        auto temp = stateStack;
        for (size_t j = 0; j < i; ++j) {
            temp.pop();
        }
        if (temp.top().blockType == blockType) {
            return true;
        }
    }
    return false;
}

bool CHTLState::isInGlobalScope() const {
    // 在顶层或命名空间中视为全局作用域
    return currentState == ParseState::TOP_LEVEL || 
           currentState == ParseState::IN_NAMESPACE ||
           (stateStack.size() <= 2 && !isInBlock(BlockType::ELEMENT_BLOCK));
}

bool CHTLState::isInLocalScope() const {
    return !isInGlobalScope();
}

bool CHTLState::isInStyleContext() const {
    return currentState == ParseState::IN_STYLE ||
           currentState == ParseState::IN_STYLE_RULE ||
           currentState == ParseState::IN_STYLE_SELECTOR ||
           currentState == ParseState::IN_STYLE_PROPERTY;
}

bool CHTLState::isInScriptContext() const {
    return currentState == ParseState::IN_SCRIPT;
}

bool CHTLState::isInTemplateDefinition() const {
    return isInState(ParseState::IN_TEMPLATE) || isInBlock(BlockType::TEMPLATE_BLOCK);
}

bool CHTLState::isInCustomDefinition() const {
    return isInState(ParseState::IN_CUSTOM) || isInBlock(BlockType::CUSTOM_BLOCK);
}

bool CHTLState::isInSpecialization() const {
    return isInState(ParseState::IN_SPECIALIZATION) || 
           getCurrentContext().hasSpecialization;
}

// 约束检查
bool CHTLState::isAllowedInCurrentContext(const std::string& element) const {
    // 检查是否在例外列表中
    if (exceptedInCurrentContext.find(element) != exceptedInCurrentContext.end()) {
        return false;
    }
    
    // 如果没有特定约束，默认允许
    if (allowedInCurrentContext.empty()) {
        return true;
    }
    
    // 检查是否在允许列表中
    return allowedInCurrentContext.find(element) != allowedInCurrentContext.end();
}

void CHTLState::addException(const std::string& element) {
    exceptedInCurrentContext.insert(element);
}

void CHTLState::clearExceptions() {
    exceptedInCurrentContext.clear();
}

// 特殊状态检查
bool CHTLState::canUseTemplateVariable() const {
    // 模板变量可以在样式块和局部脚本中使用
    return isInStyleContext() || 
           (isInScriptContext() && !getCurrentContext().isGlobalScript);
}

bool CHTLState::canUseCustomVariable() const {
    // 自定义变量组可以在样式块和局部脚本中使用
    return isInStyleContext() || 
           (isInScriptContext() && !getCurrentContext().isGlobalScript);
}

bool CHTLState::canUseStyleGroup() const {
    // 样式组只能在样式上下文中使用
    return isInStyleContext();
}

bool CHTLState::canUseFrom() const {
    // from可以在样式块、局部脚本和导入语句中使用
    return isInStyleContext() || 
           (isInScriptContext() && !getCurrentContext().isGlobalScript) ||
           isInState(ParseState::IN_IMPORT);
}

bool CHTLState::canUseDelete() const {
    // delete可以在样式块和特例化中使用
    return isInStyleContext() || isInSpecialization();
}

bool CHTLState::canUseInherit() const {
    // inherit只能在样式上下文中使用
    return isInStyleContext();
}

bool CHTLState::canUseSpecialization() const {
    // 特例化只能在自定义定义和使用自定义元素时使用
    return isInCustomDefinition() || getCurrentContext().hasSpecialization;
}

// 获取当前块信息
std::string CHTLState::getCurrentBlockName() const {
    if (stateStack.empty()) {
        return "";
    }
    return stateStack.top().blockName;
}

std::string CHTLState::getCurrentBlockType() const {
    if (stateStack.empty()) {
        return "";
    }
    return stateStack.top().blockTypeStr;
}

BlockType CHTLState::getCurrentBlockTypeEnum() const {
    if (stateStack.empty()) {
        return BlockType::NONE;
    }
    return stateStack.top().blockType;
}

// 调试方法
void CHTLState::dumpStateStack() const {
    std::cout << "=== State Stack ===" << std::endl;
    auto temp = stateStack;
    int level = 0;
    while (!temp.empty()) {
        auto context = temp.top();
        std::cout << "Level " << level++ << ": " 
                  << stateToString(context.state) 
                  << " (" << blockTypeToString(context.blockType) << ")";
        if (!context.blockName.empty()) {
            std::cout << " - " << context.blockName;
        }
        std::cout << std::endl;
        temp.pop();
    }
}

std::string CHTLState::stateToString(ParseState state) const {
    switch (state) {
        case ParseState::TOP_LEVEL: return "TOP_LEVEL";
        case ParseState::IN_USE_DECLARATION: return "IN_USE_DECLARATION";
        case ParseState::IN_NAMESPACE: return "IN_NAMESPACE";
        case ParseState::IN_CONFIGURATION: return "IN_CONFIGURATION";
        case ParseState::IN_TEMPLATE: return "IN_TEMPLATE";
        case ParseState::IN_CUSTOM: return "IN_CUSTOM";
        case ParseState::IN_ORIGIN: return "IN_ORIGIN";
        case ParseState::IN_IMPORT: return "IN_IMPORT";
        case ParseState::IN_INFO: return "IN_INFO";
        case ParseState::IN_EXPORT: return "IN_EXPORT";
        case ParseState::IN_ELEMENT: return "IN_ELEMENT";
        case ParseState::IN_TEXT: return "IN_TEXT";
        case ParseState::IN_STYLE: return "IN_STYLE";
        case ParseState::IN_SCRIPT: return "IN_SCRIPT";
        case ParseState::IN_STYLE_RULE: return "IN_STYLE_RULE";
        case ParseState::IN_STYLE_SELECTOR: return "IN_STYLE_SELECTOR";
        case ParseState::IN_STYLE_PROPERTY: return "IN_STYLE_PROPERTY";
        case ParseState::IN_ATTRIBUTE: return "IN_ATTRIBUTE";
        case ParseState::IN_STRING_LITERAL: return "IN_STRING_LITERAL";
        case ParseState::IN_COMMENT: return "IN_COMMENT";
        case ParseState::IN_SPECIAL_BLOCK: return "IN_SPECIAL_BLOCK";
        case ParseState::IN_SPECIALIZATION: return "IN_SPECIALIZATION";
        case ParseState::IN_DELETE_STATEMENT: return "IN_DELETE_STATEMENT";
        case ParseState::IN_INSERT_STATEMENT: return "IN_INSERT_STATEMENT";
        case ParseState::IN_EXCEPT_STATEMENT: return "IN_EXCEPT_STATEMENT";
        default: return "UNKNOWN";
    }
}

std::string CHTLState::blockTypeToString(BlockType type) const {
    switch (type) {
        case BlockType::NONE: return "NONE";
        case BlockType::NAMESPACE_BLOCK: return "NAMESPACE_BLOCK";
        case BlockType::CONFIGURATION_BLOCK: return "CONFIGURATION_BLOCK";
        case BlockType::TEMPLATE_BLOCK: return "TEMPLATE_BLOCK";
        case BlockType::CUSTOM_BLOCK: return "CUSTOM_BLOCK";
        case BlockType::ORIGIN_BLOCK: return "ORIGIN_BLOCK";
        case BlockType::ELEMENT_BLOCK: return "ELEMENT_BLOCK";
        case BlockType::TEXT_BLOCK: return "TEXT_BLOCK";
        case BlockType::STYLE_BLOCK: return "STYLE_BLOCK";
        case BlockType::SCRIPT_BLOCK: return "SCRIPT_BLOCK";
        case BlockType::INFO_BLOCK: return "INFO_BLOCK";
        case BlockType::EXPORT_BLOCK: return "EXPORT_BLOCK";
        case BlockType::NAME_BLOCK: return "NAME_BLOCK";
        case BlockType::ORIGIN_TYPE_BLOCK: return "ORIGIN_TYPE_BLOCK";
        case BlockType::SPECIALIZATION_BLOCK: return "SPECIALIZATION_BLOCK";
        default: return "UNKNOWN";
    }
}

void CHTLState::reset() {
    while (stateStack.size() > 1) {
        stateStack.pop();
    }
    currentState = ParseState::TOP_LEVEL;
    allowedInCurrentContext.clear();
    exceptedInCurrentContext.clear();
}

} // namespace CHTL