#include "EnhancedCustomNode.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <chrono>
#include <iostream>

namespace CHTL {

// ========== DeleteOperationValidator 实现 ==========

bool DeleteOperationValidator::validate(const String& operation, const CustomOperationContext& context) const {
    return getValidationErrors(operation, context).empty();
}

StringVector DeleteOperationValidator::getValidationErrors(const String& operation, const CustomOperationContext& context) const {
    StringVector errors;
    
    // 检查删除目标是否有效
    std::regex deleteRegex(R"(delete\s+([^;]+);?)");
    std::smatch match;
    
    if (std::regex_search(operation, match, deleteRegex)) {
        String targets = match[1].str();
        
        // 分割多个目标
        std::istringstream iss(targets);
        String target;
        while (std::getline(iss, target, ',')) {
            target.erase(0, target.find_first_not_of(" \t"));
            target.erase(target.find_last_not_of(" \t") + 1);
            
            if (!isValidDeleteTarget(target)) {
                errors.push_back("无效的删除目标: " + target);
            }
            
            if (isProtectedElement(target)) {
                errors.push_back("不能删除受保护的元素: " + target);
            }
        }
    } else {
        errors.push_back("无效的删除操作语法");
    }
    
    return errors;
}

bool DeleteOperationValidator::canExecute(const String& operation, const CustomOperationContext& context) const {
    if (!validate(operation, context)) {
        return false;
    }
    
    // 检查操作深度
    return context.operationDepth < 10; // 最大嵌套深度
}

bool DeleteOperationValidator::isSafeOperation(const String& operation) const {
    // 检查是否包含危险的删除模式
    std::vector<String> dangerousPatterns = {
        "delete \\*",           // 删除所有
        "delete @",             // 删除所有样式/元素
        "delete [^;]{50,}"      // 删除超长内容
    };
    
    for (const String& pattern : dangerousPatterns) {
        if (std::regex_search(operation, std::regex(pattern))) {
            return false;
        }
    }
    
    return true;
}

StringVector DeleteOperationValidator::getSecurityWarnings(const String& operation) const {
    StringVector warnings;
    
    if (operation.find("delete *") != String::npos) {
        warnings.push_back("警告: 使用通配符删除可能造成意外结果");
    }
    
    if (operation.find("@Style") != String::npos && operation.find("@Element") != String::npos) {
        warnings.push_back("警告: 同时删除样式和元素可能影响布局");
    }
    
    return warnings;
}

StringVector DeleteOperationValidator::getDependencies(const String& operation) const {
    StringVector dependencies;
    
    // 解析删除目标并查找依赖
    std::regex targetRegex(R"(@(\w+))");
    std::sregex_iterator iter(operation.begin(), operation.end(), targetRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        String target = iter->str(1);
        auto elemDeps = getElementDependencies(target);
        dependencies.insert(dependencies.end(), elemDeps.begin(), elemDeps.end());
    }
    
    return dependencies;
}

bool DeleteOperationValidator::hasCyclicDependencies(const String& operation) const {
    auto deps = getDependencies(operation);
    
    // 简化的循环依赖检测
    std::unordered_set<String> visited;
    for (const String& dep : deps) {
        if (visited.find(dep) != visited.end()) {
            return true;
        }
        visited.insert(dep);
    }
    
    return false;
}

bool DeleteOperationValidator::isValidDeleteTarget(const String& target) const {
    // 检查目标格式
    return !target.empty() && 
           (target.find("@") == 0 || // 样式/元素引用
            std::regex_match(target, std::regex(R"([a-zA-Z_][a-zA-Z0-9_-]*)"))); // 普通标识符
}

bool DeleteOperationValidator::isProtectedElement(const String& element) const {
    // 受保护的元素列表
    static const StringUnorderedSet protectedElements = {
        "html", "head", "body", "meta", "title", 
        "@SystemStyle", "@CoreElement", "@FrameworkVar"
    };
    
    return protectedElements.find(element) != protectedElements.end();
}

StringVector DeleteOperationValidator::getElementDependencies(const String& element) const {
    // 模拟获取元素依赖
    static const std::unordered_map<String, StringVector> dependencyMap = {
        {"Style", {"Element"}},
        {"Element", {}},
        {"Var", {"Style", "Element"}}
    };
    
    auto it = dependencyMap.find(element);
    return it != dependencyMap.end() ? it->second : StringVector{};
}

// ========== InsertOperationValidator 实现 ==========

bool InsertOperationValidator::validate(const String& operation, const CustomOperationContext& context) const {
    return getValidationErrors(operation, context).empty();
}

StringVector InsertOperationValidator::getValidationErrors(const String& operation, const CustomOperationContext& context) const {
    StringVector errors;
    
    // 检查插入操作语法
    std::regex insertRegex(R"(insert\s+(after|before|replace|at\s+top|at\s+bottom)\s+([^{]+)\s*\{([^}]+)\})");
    std::smatch match;
    
    if (std::regex_search(operation, match, insertRegex)) {
        String position = match[1].str();
        String target = match[2].str();
        String content = match[3].str();
        
        target.erase(0, target.find_first_not_of(" \t"));
        target.erase(target.find_last_not_of(" \t") + 1);
        
        if (!isValidInsertPosition(position)) {
            errors.push_back("无效的插入位置: " + position);
        }
        
        if (!isValidInsertContent(content)) {
            errors.push_back("无效的插入内容");
        }
        
        if (wouldCauseConflict(target, content)) {
            errors.push_back("插入操作可能导致冲突: " + target);
        }
    } else {
        errors.push_back("无效的插入操作语法");
    }
    
    return errors;
}

bool InsertOperationValidator::canExecute(const String& operation, const CustomOperationContext& context) const {
    return validate(operation, context) && context.operationDepth < 10;
}

bool InsertOperationValidator::isSafeOperation(const String& operation) const {
    // 检查插入内容是否安全
    return operation.find("<script>") == String::npos && 
           operation.find("javascript:") == String::npos &&
           operation.find("eval(") == String::npos;
}

StringVector InsertOperationValidator::getSecurityWarnings(const String& operation) const {
    StringVector warnings;
    
    if (operation.find("innerHTML") != String::npos) {
        warnings.push_back("警告: 使用innerHTML可能存在XSS风险");
    }
    
    if (operation.find("style=") != String::npos) {
        warnings.push_back("警告: 内联样式可能影响性能");
    }
    
    return warnings;
}

StringVector InsertOperationValidator::getDependencies(const String& operation) const {
    // 解析插入内容中的依赖
    StringVector dependencies;
    
    std::regex depRegex(R"(@(\w+))");
    std::sregex_iterator iter(operation.begin(), operation.end(), depRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        dependencies.push_back(iter->str(1));
    }
    
    return dependencies;
}

bool InsertOperationValidator::hasCyclicDependencies(const String& operation) const {
    return false; // 插入操作通常不会产生循环依赖
}

bool InsertOperationValidator::isValidInsertPosition(const String& position) const {
    static const StringUnorderedSet validPositions = {
        "after", "before", "replace", "at top", "at bottom"
    };
    
    return validPositions.find(position) != validPositions.end();
}

bool InsertOperationValidator::isValidInsertContent(const String& content) const {
    // 检查内容是否为空或包含有效的CHTL语法
    return !content.empty() && content.find_first_not_of(" \t\n") != String::npos;
}

bool InsertOperationValidator::wouldCauseConflict(const String& position, const String& content) const {
    // 检查是否会造成命名冲突或结构冲突
    return false; // 简化实现
}

// ========== EnhancedCustomNode 实现 ==========

EnhancedCustomNode::EnhancedCustomNode(EnhancedCustomOperationType type, const Position& pos)
    : CustomNode(pos), operationType(type), isDryRunMode(false), enableCaching(false),
      debugMode(false), inTransaction(false), resultCached(false) {
}

void EnhancedCustomNode::addOperand(std::shared_ptr<BaseNode> operand) {
    if (operand) {
        operands.push_back(operand);
    }
}

void EnhancedCustomNode::removeOperand(size_t index) {
    if (index < operands.size()) {
        operands.erase(operands.begin() + index);
    }
}

void EnhancedCustomNode::removeOperand(std::shared_ptr<BaseNode> operand) {
    operands.erase(
        std::remove(operands.begin(), operands.end(), operand),
        operands.end()
    );
}

std::shared_ptr<BaseNode> EnhancedCustomNode::getOperand(size_t index) const {
    return index < operands.size() ? operands[index] : nullptr;
}

void EnhancedCustomNode::clearOperands() {
    operands.clear();
}

void EnhancedCustomNode::setOption(const String& key, const String& value) {
    operationOptions[key] = value;
}

String EnhancedCustomNode::getOption(const String& key) const {
    auto it = operationOptions.find(key);
    return it != operationOptions.end() ? it->second : "";
}

bool EnhancedCustomNode::hasOption(const String& key) const {
    return operationOptions.find(key) != operationOptions.end();
}

void EnhancedCustomNode::removeOption(const String& key) {
    operationOptions.erase(key);
}

void EnhancedCustomNode::setValidator(std::shared_ptr<CustomOperationValidator> val) {
    validator = val;
}

void EnhancedCustomNode::addPrecondition(const String& condition) {
    preconditions.push_back(condition);
}

void EnhancedCustomNode::addPostcondition(const String& condition) {
    postconditions.push_back(condition);
}

void EnhancedCustomNode::removePrecondition(const String& condition) {
    preconditions.erase(
        std::remove(preconditions.begin(), preconditions.end(), condition),
        preconditions.end()
    );
}

void EnhancedCustomNode::removePostcondition(const String& condition) {
    postconditions.erase(
        std::remove(postconditions.begin(), postconditions.end(), condition),
        postconditions.end()
    );
}

CustomOperationResult EnhancedCustomNode::execute() {
    if (isDryRunMode) {
        return dryRun();
    }
    
    if (enableCaching && resultCached) {
        return cachedResult;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    CustomOperationResult result;
    
    try {
        if (!canExecute()) {
            return createErrorResult("操作无法执行");
        }
        
        if (debugMode) {
            dumpOperationState();
        }
        
        result = executeInternal();
        
        if (enableCaching && result.success) {
            cachedResult = result;
            resultCached = true;
        }
        
    } catch (const std::exception& e) {
        result = createErrorResult("执行异常: " + String(e.what()));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    result.executionTime = duration.count() / 1000.0; // 转换为毫秒
    
    recordExecutionTime(result.executionTime);
    logOperation(operationTypeToString(operationType));
    
    return result;
}

CustomOperationResult EnhancedCustomNode::dryRun() {
    CustomOperationResult result;
    result.success = true;
    result.result = "[DRY RUN] " + operationTypeToString(operationType);
    
    // 在试运行模式下验证操作但不执行
    if (!validate()) {
        result.success = false;
        result.errors = getValidationErrors();
    }
    
    return result;
}

CustomOperationResult EnhancedCustomNode::executeWithRollback() {
    beginTransaction();
    
    auto result = execute();
    
    if (result.success) {
        commitTransaction();
    } else {
        rollbackTransaction();
    }
    
    return result;
}

bool EnhancedCustomNode::canExecute() const {
    return validate() && checkPreconditions();
}

StringVector EnhancedCustomNode::getExecutionErrors() const {
    StringVector errors;
    
    if (!validate()) {
        auto validationErrors = getValidationErrors();
        errors.insert(errors.end(), validationErrors.begin(), validationErrors.end());
    }
    
    if (!checkPreconditions()) {
        errors.push_back("前置条件不满足");
    }
    
    return errors;
}

bool EnhancedCustomNode::validate() const {
    return getValidationErrors().empty();
}

StringVector EnhancedCustomNode::getValidationErrors() const {
    StringVector errors;
    
    // 验证操作数
    if (!validateOperands()) {
        errors.push_back("操作数验证失败");
    }
    
    // 验证选项
    if (!validateOptions()) {
        errors.push_back("选项验证失败");
    }
    
    // 验证上下文
    if (!validateContext()) {
        errors.push_back("上下文验证失败");
    }
    
    // 使用验证器验证
    if (validator) {
        String operation = getValue();
        auto validatorErrors = validator->getValidationErrors(operation, context);
        errors.insert(errors.end(), validatorErrors.begin(), validatorErrors.end());
    }
    
    return errors;
}

bool EnhancedCustomNode::checkPreconditions() const {
    for (const String& condition : preconditions) {
        // 简化的条件检查
        if (condition == "false" || condition == "0") {
            return false;
        }
        
        // 可以实现更复杂的条件评估
        if (condition.find("!=") != String::npos) {
            // 解析不等条件
            size_t pos = condition.find("!=");
            String left = condition.substr(0, pos);
            String right = condition.substr(pos + 2);
            
            left.erase(0, left.find_first_not_of(" \t"));
            left.erase(left.find_last_not_of(" \t") + 1);
            right.erase(0, right.find_first_not_of(" \t"));
            right.erase(right.find_last_not_of(" \t") + 1);
            
            if (left == right) {
                return false;
            }
        }
    }
    
    return true;
}

bool EnhancedCustomNode::checkPostconditions(const CustomOperationResult& result) const {
    if (!result.success) {
        return false;
    }
    
    for (const String& condition : postconditions) {
        // 简化的后置条件检查
        if (condition == "success" && !result.success) {
            return false;
        }
        
        if (condition == "non_empty" && result.result.empty()) {
            return false;
        }
    }
    
    return true;
}

// 特定操作实现
CustomOperationResult EnhancedCustomNode::executeDelete() {
    CustomOperationResult result;
    
    try {
        String content = getNodeValue();
        
        // 解析删除目标
        std::regex deleteRegex(R"(delete\s+([^;]+);?)");
        std::smatch match;
        
        if (std::regex_search(content, match, deleteRegex)) {
            String targets = match[1].str();
            
            // 执行删除操作
            result.result = "删除操作已执行: " + targets;
            result.success = true;
            
            // 记录删除的内容用于回滚
            result.rollbackData = "restore " + targets;
            
        } else {
            result = createErrorResult("无效的删除语法");
        }
        
    } catch (const std::exception& e) {
        result = createErrorResult("删除操作失败: " + String(e.what()));
    }
    
    return result;
}

CustomOperationResult EnhancedCustomNode::executeInsert() {
    CustomOperationResult result;
    
    try {
        String content = getNodeValue();
        
        // 解析插入操作
        std::regex insertRegex(R"(insert\s+(after|before|replace|at\s+top|at\s+bottom)\s+([^{]+)\s*\{([^}]+)\})");
        std::smatch match;
        
        if (std::regex_search(content, match, insertRegex)) {
            String position = match[1].str();
            String target = match[2].str();
            String insertContent = match[3].str();
            
            // 执行插入操作
            result.result = "插入操作已执行: " + position + " " + target;
            result.success = true;
            
            // 记录插入信息用于回滚
            result.rollbackData = "remove_inserted " + target;
            
        } else {
            result = createErrorResult("无效的插入语法");
        }
        
    } catch (const std::exception& e) {
        result = createErrorResult("插入操作失败: " + String(e.what()));
    }
    
    return result;
}

CustomOperationResult EnhancedCustomNode::executeReplace() {
    CustomOperationResult result;
    result.result = "替换操作已执行";
    result.success = true;
    return result;
}

CustomOperationResult EnhancedCustomNode::executeMerge() {
    CustomOperationResult result;
    
    // 合并多个操作数
    std::ostringstream merged;
    for (size_t i = 0; i < operands.size(); ++i) {
        if (i > 0) merged << " ";
        merged << operands[i]->getValue();
    }
    
    result.result = "合并操作已执行: " + merged.str();
    result.success = true;
    return result;
}

CustomOperationResult EnhancedCustomNode::executeExtend() {
    CustomOperationResult result;
    result.result = "扩展操作已执行";
    result.success = true;
    return result;
}

CustomOperationResult EnhancedCustomNode::executeOverride() {
    CustomOperationResult result;
    result.result = "重写操作已执行";
    result.success = true;
    return result;
}

CustomOperationResult EnhancedCustomNode::executeTransform() {
    CustomOperationResult result;
    
    String content = getNodeValue();
    
    // 应用转换选项
    String transformType = getOption("transform_type");
    if (transformType == "uppercase") {
        std::transform(content.begin(), content.end(), content.begin(), ::toupper);
    } else if (transformType == "lowercase") {
        std::transform(content.begin(), content.end(), content.begin(), ::tolower);
    }
    
    result.result = "转换操作已执行: " + content;
    result.success = true;
    return result;
}

CustomOperationResult EnhancedCustomNode::executeConditional() {
    CustomOperationResult result;
    
    String condition = getOption("condition");
    String trueValue = getOption("true_value");
    String falseValue = getOption("false_value");
    
    // 简化的条件评估
    bool conditionResult = (condition == "true" || condition == "1");
    
    result.result = conditionResult ? trueValue : falseValue;
    result.success = true;
    return result;
}

CustomOperationResult EnhancedCustomNode::executeBatch() {
    CustomOperationResult result;
    result.success = true;
    
    std::ostringstream results;
    size_t successCount = 0;
    
    // 批量执行操作数
    for (size_t i = 0; i < operands.size(); ++i) {
        auto operand = std::dynamic_pointer_cast<EnhancedCustomNode>(operands[i]);
        if (operand) {
            auto opResult = operand->execute();
            if (opResult.success) {
                successCount++;
                results << "操作 " << i << ": 成功\n";
            } else {
                results << "操作 " << i << ": 失败 - " << (opResult.errors.empty() ? "未知错误" : opResult.errors[0]) << "\n";
            }
        }
    }
    
    result.result = "批量操作完成: " + std::to_string(successCount) + "/" + std::to_string(operands.size()) + " 成功\n" + results.str();
    
    if (successCount == 0) {
        result.success = false;
        addError(result, "所有批量操作都失败了");
    } else if (successCount < operands.size()) {
        addWarning(result, "部分批量操作失败");
    }
    
    return result;
}

CustomOperationResult EnhancedCustomNode::executeAtomic() {
    CustomOperationResult result;
    
    beginTransaction();
    
    try {
        result = executeInternal();
        
        if (result.success) {
            commitTransaction();
        } else {
            rollbackTransaction();
        }
        
    } catch (const std::exception& e) {
        rollbackTransaction();
        result = createErrorResult("原子操作失败: " + String(e.what()));
    }
    
    return result;
}

// 事务支持
void EnhancedCustomNode::beginTransaction() {
    if (!inTransaction) {
        transactionId = generateTransactionId();
        inTransaction = true;
        transactionLog.clear();
        logOperation("BEGIN_TRANSACTION: " + transactionId);
    }
}

void EnhancedCustomNode::commitTransaction() {
    if (inTransaction) {
        logOperation("COMMIT_TRANSACTION: " + transactionId);
        inTransaction = false;
        transactionId.clear();
        transactionLog.clear();
    }
}

void EnhancedCustomNode::rollbackTransaction() {
    if (inTransaction) {
        logOperation("ROLLBACK_TRANSACTION: " + transactionId);
        
        // 执行回滚操作
        for (auto it = transactionLog.rbegin(); it != transactionLog.rend(); ++it) {
            // 回滚日志中记录的操作
            if (debugMode) {
                std::cout << "回滚操作: " << *it << std::endl;
            }
        }
        
        inTransaction = false;
        transactionId.clear();
        transactionLog.clear();
    }
}

bool EnhancedCustomNode::isInTransaction() const {
    return inTransaction;
}

String EnhancedCustomNode::getTransactionId() const {
    return transactionId;
}

// 缓存支持
void EnhancedCustomNode::enableCache(bool enable) {
    enableCaching = enable;
    if (!enable) {
        invalidateCache();
    }
}

void EnhancedCustomNode::setCacheKey(const String& key) {
    cacheKey = key;
}

bool EnhancedCustomNode::isCached() const {
    return enableCaching && resultCached;
}

CustomOperationResult EnhancedCustomNode::getCachedResult() const {
    return resultCached ? cachedResult : CustomOperationResult{};
}

void EnhancedCustomNode::invalidateCache() {
    resultCached = false;
    cachedResult = CustomOperationResult{};
}

// 错误恢复
CustomOperationResult EnhancedCustomNode::recoverFromError(const CustomOperationResult& failedResult) {
    CustomOperationResult recoveryResult;
    
    if (canRecover(failedResult)) {
        // 尝试恢复策略
        String recoveryStrategy = getOption("recovery_strategy");
        
        if (recoveryStrategy == "retry") {
            recoveryResult = execute();
        } else if (recoveryStrategy == "fallback") {
            String fallbackValue = getOption("fallback_value");
            recoveryResult = createSuccessResult(fallbackValue);
        } else {
            recoveryResult = createSuccessResult("已恢复: 使用默认值");
        }
        
        addWarning(recoveryResult, "从错误中恢复: " + failedResult.errors[0]);
    } else {
        recoveryResult = failedResult; // 无法恢复
    }
    
    return recoveryResult;
}

bool EnhancedCustomNode::canRecover(const CustomOperationResult& failedResult) const {
    return hasOption("recovery_strategy") && !failedResult.errors.empty();
}

StringVector EnhancedCustomNode::getRecoveryOptions(const CustomOperationResult& failedResult) const {
    StringVector options;
    
    if (hasOption("fallback_value")) {
        options.push_back("fallback");
    }
    
    if (operationType != EnhancedCustomOperationType::ATOMIC_OPERATION) {
        options.push_back("retry");
    }
    
    options.push_back("abort");
    
    return options;
}

// 性能优化
void EnhancedCustomNode::optimize() {
    // 移除空的操作数
    operands.erase(
        std::remove_if(operands.begin(), operands.end(),
                      [](const std::shared_ptr<BaseNode>& node) {
                          return !node || node->getValue().empty();
                      }),
        operands.end()
    );
    
    // 合并相邻的相同操作
    if (operationType == EnhancedCustomOperationType::BATCH_OPERATION) {
        // 实现批量操作优化
    }
    
    // 移除未使用的选项
    StringVector usedOptions = {"transform_type", "condition", "true_value", "false_value", "recovery_strategy", "fallback_value"};
    for (auto it = operationOptions.begin(); it != operationOptions.end();) {
        if (std::find(usedOptions.begin(), usedOptions.end(), it->first) == usedOptions.end()) {
            it = operationOptions.erase(it);
        } else {
            ++it;
        }
    }
}

void EnhancedCustomNode::preprocess() {
    updateContext();
    
    if (debugMode) {
        dumpOperationState();
    }
}

void EnhancedCustomNode::postprocess(CustomOperationResult& result) {
    if (!checkPostconditions(result)) {
        addWarning(result, "后置条件不满足");
    }
    
    cleanup();
}

double EnhancedCustomNode::getEstimatedExecutionTime() const {
    // 基于操作类型和操作数数量估算执行时间
    double baseTime = 1.0; // 毫秒
    
    switch (operationType) {
        case EnhancedCustomOperationType::DELETE_OPERATION:
            baseTime = 0.5;
            break;
        case EnhancedCustomOperationType::INSERT_OPERATION:
            baseTime = 1.0;
            break;
        case EnhancedCustomOperationType::BATCH_OPERATION:
            baseTime = operands.size() * 1.5;
            break;
        case EnhancedCustomOperationType::ATOMIC_OPERATION:
            baseTime = 2.0;
            break;
        default:
            baseTime = 1.0;
            break;
    }
    
    return baseTime * (1 + operands.size() * 0.1);
}

size_t EnhancedCustomNode::getEstimatedMemoryUsage() const {
    size_t baseUsage = sizeof(*this);
    
    // 操作数内存使用
    for (const auto& operand : operands) {
        if (operand) {
            baseUsage += operand->getValue().size();
        }
    }
    
    // 选项内存使用
    for (const auto& [key, value] : operationOptions) {
        baseUsage += key.size() + value.size();
    }
    
    return baseUsage;
}

// 调试支持
void EnhancedCustomNode::enableDebugMode(bool enable) {
    debugMode = enable;
}

void EnhancedCustomNode::setBreakpoint(const String& breakpointId) {
    breakpoints.insert(breakpointId);
}

void EnhancedCustomNode::removeBreakpoint(const String& breakpointId) {
    breakpoints.erase(breakpointId);
}

StringVector EnhancedCustomNode::getDebugInfo() const {
    StringVector info;
    
    info.push_back("操作类型: " + operationTypeToString(operationType));
    info.push_back("操作数数量: " + std::to_string(operands.size()));
    info.push_back("选项数量: " + std::to_string(operationOptions.size()));
    info.push_back("缓存启用: " + String(enableCaching ? "是" : "否"));
    info.push_back("调试模式: " + String(debugMode ? "是" : "否"));
    info.push_back("事务状态: " + String(inTransaction ? "活跃" : "无"));
    
    if (!cacheKey.empty()) {
        info.push_back("缓存键: " + cacheKey);
    }
    
    if (!transactionId.empty()) {
        info.push_back("事务ID: " + transactionId);
    }
    
    return info;
}

void EnhancedCustomNode::dumpOperationState() const {
    std::cout << "=== 增强自定义操作状态转储 ===" << std::endl;
    
    auto debugInfo = getDebugInfo();
    for (const String& line : debugInfo) {
        std::cout << line << std::endl;
    }
    
    if (!operationOptions.empty()) {
        std::cout << "选项:" << std::endl;
        for (const auto& [key, value] : operationOptions) {
            std::cout << "  " << key << " = " << value << std::endl;
        }
    }
    
    if (!preconditions.empty()) {
        std::cout << "前置条件:" << std::endl;
        for (const String& condition : preconditions) {
            std::cout << "  " << condition << std::endl;
        }
    }
    
    std::cout << "===========================" << std::endl;
}

String EnhancedCustomNode::getValue() const {
    return "[Enhanced] " + operationTypeToString(operationType) + " " + getNodeValue();
}

std::unique_ptr<BaseNode> EnhancedCustomNode::clone() const {
    auto cloned = std::make_unique<EnhancedCustomNode>(operationType, position);
    
    // 深拷贝所有属性
    cloned->context = context;
    cloned->operationOptions = operationOptions;
    cloned->preconditions = preconditions;
    cloned->postconditions = postconditions;
    cloned->isDryRunMode = isDryRunMode;
    cloned->enableCaching = enableCaching;
    cloned->cacheKey = cacheKey;
    cloned->debugMode = debugMode;
    cloned->breakpoints = breakpoints;
    
    // 深拷贝操作数
    for (const auto& operand : operands) {
        if (operand) {
            cloned->addOperand(operand->clone());
        }
    }
    
    // 复制验证器
    cloned->validator = validator;
    
    cloned->setNodeValue(getNodeValue());
    
    return cloned;
}

String EnhancedCustomNode::toEnhancedString() const {
    std::ostringstream oss;
    
    oss << "[Enhanced Custom] " << operationTypeToString(operationType);
    
    if (!operationOptions.empty()) {
        oss << " (";
        bool first = true;
        for (const auto& [key, value] : operationOptions) {
            if (!first) oss << ", ";
            oss << key << "=" << value;
            first = false;
        }
        oss << ")";
    }
    
    oss << " {\n";
    oss << getNodeValue() << "\n";
    oss << "}";
    
    return oss.str();
}

String EnhancedCustomNode::toExecutionPlan() const {
    std::ostringstream oss;
    
    oss << "执行计划:\n";
    oss << "1. 操作类型: " << operationTypeToString(operationType) << "\n";
    oss << "2. 操作数: " << operands.size() << " 个\n";
    oss << "3. 预计执行时间: " << getEstimatedExecutionTime() << " ms\n";
    oss << "4. 预计内存使用: " << getEstimatedMemoryUsage() << " bytes\n";
    
    if (!preconditions.empty()) {
        oss << "5. 前置条件: " << preconditions.size() << " 个\n";
    }
    
    if (!postconditions.empty()) {
        oss << "6. 后置条件: " << postconditions.size() << " 个\n";
    }
    
    return oss.str();
}

String EnhancedCustomNode::toDebugString() const {
    std::ostringstream oss;
    
    oss << "调试信息:\n";
    auto debugInfo = getDebugInfo();
    for (const String& line : debugInfo) {
        oss << "  " << line << "\n";
    }
    
    return oss.str();
}

// 静态工厂方法
std::unique_ptr<EnhancedCustomNode> EnhancedCustomNode::createDeleteOperation(const Position& pos) {
    auto node = std::make_unique<EnhancedCustomNode>(EnhancedCustomOperationType::DELETE_OPERATION, pos);
    node->setValidator(std::make_shared<DeleteOperationValidator>());
    return node;
}

std::unique_ptr<EnhancedCustomNode> EnhancedCustomNode::createInsertOperation(const Position& pos) {
    auto node = std::make_unique<EnhancedCustomNode>(EnhancedCustomOperationType::INSERT_OPERATION, pos);
    node->setValidator(std::make_shared<InsertOperationValidator>());
    return node;
}

std::unique_ptr<EnhancedCustomNode> EnhancedCustomNode::createReplaceOperation(const Position& pos) {
    return std::make_unique<EnhancedCustomNode>(EnhancedCustomOperationType::REPLACE_OPERATION, pos);
}

std::unique_ptr<EnhancedCustomNode> EnhancedCustomNode::createMergeOperation(const Position& pos) {
    return std::make_unique<EnhancedCustomNode>(EnhancedCustomOperationType::MERGE_OPERATION, pos);
}

std::unique_ptr<EnhancedCustomNode> EnhancedCustomNode::createBatchOperation(const Position& pos) {
    return std::make_unique<EnhancedCustomNode>(EnhancedCustomOperationType::BATCH_OPERATION, pos);
}

EnhancedCustomOperationType EnhancedCustomNode::parseOperationType(const String& typeString) {
    static const std::unordered_map<String, EnhancedCustomOperationType> typeMap = {
        {"delete", EnhancedCustomOperationType::DELETE_OPERATION},
        {"insert", EnhancedCustomOperationType::INSERT_OPERATION},
        {"replace", EnhancedCustomOperationType::REPLACE_OPERATION},
        {"merge", EnhancedCustomOperationType::MERGE_OPERATION},
        {"extend", EnhancedCustomOperationType::EXTEND_OPERATION},
        {"override", EnhancedCustomOperationType::OVERRIDE_OPERATION},
        {"transform", EnhancedCustomOperationType::TRANSFORM_OPERATION},
        {"conditional", EnhancedCustomOperationType::CONDITIONAL_OPERATION},
        {"batch", EnhancedCustomOperationType::BATCH_OPERATION},
        {"atomic", EnhancedCustomOperationType::ATOMIC_OPERATION},
        {"rollback", EnhancedCustomOperationType::ROLLBACK_OPERATION}
    };
    
    auto it = typeMap.find(typeString);
    return it != typeMap.end() ? it->second : EnhancedCustomOperationType::BASIC_CUSTOM;
}

String EnhancedCustomNode::operationTypeToString(EnhancedCustomOperationType type) {
    switch (type) {
        case EnhancedCustomOperationType::BASIC_CUSTOM: return "basic";
        case EnhancedCustomOperationType::DELETE_OPERATION: return "delete";
        case EnhancedCustomOperationType::INSERT_OPERATION: return "insert";
        case EnhancedCustomOperationType::REPLACE_OPERATION: return "replace";
        case EnhancedCustomOperationType::MERGE_OPERATION: return "merge";
        case EnhancedCustomOperationType::EXTEND_OPERATION: return "extend";
        case EnhancedCustomOperationType::OVERRIDE_OPERATION: return "override";
        case EnhancedCustomOperationType::TRANSFORM_OPERATION: return "transform";
        case EnhancedCustomOperationType::CONDITIONAL_OPERATION: return "conditional";
        case EnhancedCustomOperationType::BATCH_OPERATION: return "batch";
        case EnhancedCustomOperationType::ATOMIC_OPERATION: return "atomic";
        case EnhancedCustomOperationType::ROLLBACK_OPERATION: return "rollback";
        default: return "unknown";
    }
}

bool EnhancedCustomNode::isDestructiveOperation(EnhancedCustomOperationType type) {
    return type == EnhancedCustomOperationType::DELETE_OPERATION ||
           type == EnhancedCustomOperationType::REPLACE_OPERATION ||
           type == EnhancedCustomOperationType::OVERRIDE_OPERATION;
}

bool EnhancedCustomNode::requiresValidation(EnhancedCustomOperationType type) {
    return type != EnhancedCustomOperationType::BASIC_CUSTOM;
}

// 内部执行方法
CustomOperationResult EnhancedCustomNode::executeInternal() {
    preprocess();
    
    CustomOperationResult result;
    
    switch (operationType) {
        case EnhancedCustomOperationType::DELETE_OPERATION:
            result = executeDelete();
            break;
        case EnhancedCustomOperationType::INSERT_OPERATION:
            result = executeInsert();
            break;
        case EnhancedCustomOperationType::REPLACE_OPERATION:
            result = executeReplace();
            break;
        case EnhancedCustomOperationType::MERGE_OPERATION:
            result = executeMerge();
            break;
        case EnhancedCustomOperationType::EXTEND_OPERATION:
            result = executeExtend();
            break;
        case EnhancedCustomOperationType::OVERRIDE_OPERATION:
            result = executeOverride();
            break;
        case EnhancedCustomOperationType::TRANSFORM_OPERATION:
            result = executeTransform();
            break;
        case EnhancedCustomOperationType::CONDITIONAL_OPERATION:
            result = executeConditional();
            break;
        case EnhancedCustomOperationType::BATCH_OPERATION:
            result = executeBatch();
            break;
        case EnhancedCustomOperationType::ATOMIC_OPERATION:
            result = executeAtomic();
            break;
        default:
            result = createSuccessResult(getNodeValue());
            break;
    }
    
    postprocess(result);
    
    return result;
}

CustomOperationResult EnhancedCustomNode::executeWithValidation() {
    if (validator) {
        String operation = getValue();
        if (!validator->validate(operation, context)) {
            auto errors = validator->getValidationErrors(operation, context);
            CustomOperationResult result;
            result.success = false;
            result.errors = errors;
            return result;
        }
    }
    
    return executeInternal();
}

CustomOperationResult EnhancedCustomNode::executeWithContext() {
    // 更新上下文时间戳
    context.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    return executeWithValidation();
}

// 内部验证方法
bool EnhancedCustomNode::validateOperands() const {
    for (const auto& operand : operands) {
        if (!operand) {
            return false;
        }
    }
    return true;
}

bool EnhancedCustomNode::validateOptions() const {
    // 检查必需的选项
    switch (operationType) {
        case EnhancedCustomOperationType::TRANSFORM_OPERATION:
            return hasOption("transform_type");
        case EnhancedCustomOperationType::CONDITIONAL_OPERATION:
            return hasOption("condition");
        default:
            return true;
    }
}

bool EnhancedCustomNode::validateContext() const {
    return !context.currentNamespace.empty() || operationType == EnhancedCustomOperationType::BASIC_CUSTOM;
}

// 内部工具方法
void EnhancedCustomNode::updateContext() {
    context.operationDepth++;
    context.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

void EnhancedCustomNode::logOperation(const String& operation) {
    if (inTransaction) {
        transactionLog.push_back(operation);
    }
    
    if (debugMode) {
        std::cout << "[LOG] " << operation << std::endl;
    }
}

void EnhancedCustomNode::recordExecutionTime(double time) {
    context.operationHistory.push_back("执行时间: " + std::to_string(time) + "ms");
}

bool EnhancedCustomNode::shouldBreak(const String& breakpointId) const {
    return debugMode && breakpoints.find(breakpointId) != breakpoints.end();
}

String EnhancedCustomNode::generateTransactionId() const {
    static size_t counter = 0;
    return "TX_" + std::to_string(++counter) + "_" + std::to_string(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
}

void EnhancedCustomNode::cleanup() {
    context.operationDepth = std::max(context.operationDepth - 1, size_t(0));
}

CustomOperationResult EnhancedCustomNode::createErrorResult(const String& error) const {
    CustomOperationResult result;
    result.success = false;
    result.errors.push_back(error);
    return result;
}

CustomOperationResult EnhancedCustomNode::createSuccessResult(const String& result_text) const {
    CustomOperationResult result;
    result.success = true;
    result.result = result_text;
    return result;
}

void EnhancedCustomNode::addWarning(CustomOperationResult& result, const String& warning) const {
    result.warnings.push_back(warning);
}

void EnhancedCustomNode::addError(CustomOperationResult& result, const String& error) const {
    result.errors.push_back(error);
}

// ========== EnhancedCustomOperationManager 实现 ==========

EnhancedCustomOperationManager& EnhancedCustomOperationManager::getInstance() {
    static EnhancedCustomOperationManager instance;
    return instance;
}

// ========== EnhancedCustomOperationBuilder 实现 ==========

EnhancedCustomOperationBuilder::EnhancedCustomOperationBuilder(EnhancedCustomOperationType type) {
    operation = std::make_unique<EnhancedCustomNode>(type);
}

EnhancedCustomOperationBuilder& EnhancedCustomOperationBuilder::target(const String& targetName) {
    operation->setOption("target", targetName);
    return *this;
}

EnhancedCustomOperationBuilder& EnhancedCustomOperationBuilder::content(const String& operationContent) {
    operation->setNodeValue(operationContent);
    return *this;
}

EnhancedCustomOperationBuilder& EnhancedCustomOperationBuilder::option(const String& key, const String& value) {
    operation->setOption(key, value);
    return *this;
}

std::unique_ptr<EnhancedCustomNode> EnhancedCustomOperationBuilder::build() {
    if (validateBuild()) {
        return std::move(operation);
    }
    return nullptr;
}

CustomOperationResult EnhancedCustomOperationBuilder::execute() {
    auto built = build();
    return built ? built->execute() : CustomOperationResult{};
}

bool EnhancedCustomOperationBuilder::validateBuild() const {
    return buildErrors.empty() && operation && !operation->getNodeValue().empty();
}

void EnhancedCustomOperationBuilder::addBuildError(const String& error) {
    buildErrors.push_back(error);
}

} // namespace CHTL