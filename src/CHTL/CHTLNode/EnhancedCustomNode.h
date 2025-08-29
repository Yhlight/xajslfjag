#pragma once

#include "CustomNode.h"
#include "SpecializationNode.h"
#include "../../Util/Common.h"
#include <unordered_map>
#include <memory>
#include <functional>

namespace CHTL {

// 自定义操作类型增强
enum class EnhancedCustomOperationType {
    BASIC_CUSTOM,           // 基础自定义
    DELETE_OPERATION,       // 删除操作
    INSERT_OPERATION,       // 插入操作
    REPLACE_OPERATION,      // 替换操作
    MERGE_OPERATION,        // 合并操作
    EXTEND_OPERATION,       // 扩展操作
    OVERRIDE_OPERATION,     // 重写操作
    TRANSFORM_OPERATION,    // 转换操作
    CONDITIONAL_OPERATION,  // 条件操作
    BATCH_OPERATION,        // 批量操作
    ATOMIC_OPERATION,       // 原子操作
    ROLLBACK_OPERATION      // 回滚操作
};

// 操作上下文
struct CustomOperationContext {
    String currentNamespace;        // 当前命名空间
    String sourceTemplate;          // 源模板名
    String targetTemplate;          // 目标模板名
    StringUnorderedMap variables;   // 上下文变量
    StringVector operationHistory;  // 操作历史
    bool isTransactional;          // 是否事务性
    bool enableRollback;           // 是否启用回滚
    size_t operationDepth;         // 操作深度
    double timestamp;              // 时间戳
    
    CustomOperationContext() : isTransactional(false), enableRollback(false), operationDepth(0), timestamp(0.0) {}
};

// 操作结果
struct CustomOperationResult {
    bool success;                   // 操作是否成功
    String result;                  // 操作结果内容
    StringVector warnings;          // 警告信息
    StringVector errors;            // 错误信息
    StringUnorderedMap metadata;    // 元数据
    String rollbackData;            // 回滚数据
    double executionTime;           // 执行时间
    
    CustomOperationResult() : success(false), executionTime(0.0) {}
};

// 操作验证器
class CustomOperationValidator {
public:
    virtual ~CustomOperationValidator() = default;
    
    // 验证方法
    virtual bool validate(const String& operation, const CustomOperationContext& context) const = 0;
    virtual StringVector getValidationErrors(const String& operation, const CustomOperationContext& context) const = 0;
    virtual bool canExecute(const String& operation, const CustomOperationContext& context) const = 0;
    
    // 安全检查
    virtual bool isSafeOperation(const String& operation) const = 0;
    virtual StringVector getSecurityWarnings(const String& operation) const = 0;
    
    // 依赖检查
    virtual StringVector getDependencies(const String& operation) const = 0;
    virtual bool hasCyclicDependencies(const String& operation) const = 0;
};

// 删除操作验证器
class DeleteOperationValidator : public CustomOperationValidator {
public:
    bool validate(const String& operation, const CustomOperationContext& context) const override;
    StringVector getValidationErrors(const String& operation, const CustomOperationContext& context) const override;
    bool canExecute(const String& operation, const CustomOperationContext& context) const override;
    bool isSafeOperation(const String& operation) const override;
    StringVector getSecurityWarnings(const String& operation) const override;
    StringVector getDependencies(const String& operation) const override;
    bool hasCyclicDependencies(const String& operation) const override;
    
private:
    bool isValidDeleteTarget(const String& target) const;
    bool isProtectedElement(const String& element) const;
    StringVector getElementDependencies(const String& element) const;
};

// 插入操作验证器
class InsertOperationValidator : public CustomOperationValidator {
public:
    bool validate(const String& operation, const CustomOperationContext& context) const override;
    StringVector getValidationErrors(const String& operation, const CustomOperationContext& context) const override;
    bool canExecute(const String& operation, const CustomOperationContext& context) const override;
    bool isSafeOperation(const String& operation) const override;
    StringVector getSecurityWarnings(const String& operation) const override;
    StringVector getDependencies(const String& operation) const override;
    bool hasCyclicDependencies(const String& operation) const override;
    
private:
    bool isValidInsertPosition(const String& position) const;
    bool isValidInsertContent(const String& content) const;
    bool wouldCauseConflict(const String& position, const String& content) const;
};

// 增强自定义节点
class EnhancedCustomNode : public CustomNode {
public:
    EnhancedCustomOperationType operationType;      // 操作类型
    CustomOperationContext context;                 // 操作上下文
    std::vector<std::shared_ptr<BaseNode>> operands; // 操作数
    StringUnorderedMap operationOptions;           // 操作选项
    std::shared_ptr<CustomOperationValidator> validator; // 验证器
    StringVector preconditions;                     // 前置条件
    StringVector postconditions;                    // 后置条件
    std::function<CustomOperationResult(const CustomOperationContext&)> customExecutor; // 自定义执行器
    bool isDryRunMode;                             // 是否为试运行模式
    bool enableCaching;                            // 是否启用缓存
    String cacheKey;                               // 缓存键
    
    explicit EnhancedCustomNode(EnhancedCustomOperationType type, const Position& pos = Position());
    ~EnhancedCustomNode() override = default;
    
    // 操作数管理
    void addOperand(std::shared_ptr<BaseNode> operand);
    void removeOperand(size_t index);
    void removeOperand(std::shared_ptr<BaseNode> operand);
    std::shared_ptr<BaseNode> getOperand(size_t index) const;
    size_t getOperandCount() const { return operands.size(); }
    void clearOperands();
    
    // 选项管理
    void setOption(const String& key, const String& value);
    String getOption(const String& key) const;
    bool hasOption(const String& key) const;
    void removeOption(const String& key);
    StringUnorderedMap getAllOptions() const { return operationOptions; }
    
    // 验证器管理
    void setValidator(std::shared_ptr<CustomOperationValidator> validator);
    std::shared_ptr<CustomOperationValidator> getValidator() const { return validator; }
    bool hasValidator() const { return validator != nullptr; }
    
    // 条件管理
    void addPrecondition(const String& condition);
    void addPostcondition(const String& condition);
    void removePrecondition(const String& condition);
    void removePostcondition(const String& condition);
    StringVector getPreconditions() const { return preconditions; }
    StringVector getPostconditions() const { return postconditions; }
    
    // 操作执行
    CustomOperationResult execute();
    CustomOperationResult dryRun();
    CustomOperationResult executeWithRollback();
    bool canExecute() const;
    StringVector getExecutionErrors() const;
    
    // 操作验证
    bool validate() const;
    StringVector getValidationErrors() const;
    bool checkPreconditions() const;
    bool checkPostconditions(const CustomOperationResult& result) const;
    
    // 特定操作实现
    CustomOperationResult executeDelete();
    CustomOperationResult executeInsert();
    CustomOperationResult executeReplace();
    CustomOperationResult executeMerge();
    CustomOperationResult executeExtend();
    CustomOperationResult executeOverride();
    CustomOperationResult executeTransform();
    CustomOperationResult executeConditional();
    CustomOperationResult executeBatch();
    CustomOperationResult executeAtomic();
    
    // 事务支持
    void beginTransaction();
    void commitTransaction();
    void rollbackTransaction();
    bool isInTransaction() const;
    String getTransactionId() const;
    
    // 缓存支持
    void enableCache(bool enable = true);
    void setCacheKey(const String& key);
    bool isCached() const;
    CustomOperationResult getCachedResult() const;
    void invalidateCache();
    
    // 错误恢复
    CustomOperationResult recoverFromError(const CustomOperationResult& failedResult);
    bool canRecover(const CustomOperationResult& failedResult) const;
    StringVector getRecoveryOptions(const CustomOperationResult& failedResult) const;
    
    // 性能优化
    void optimize();
    void preprocess();
    void postprocess(CustomOperationResult& result);
    double getEstimatedExecutionTime() const;
    size_t getEstimatedMemoryUsage() const;
    
    // 调试支持
    void enableDebugMode(bool enable = true);
    void setBreakpoint(const String& breakpointId);
    void removeBreakpoint(const String& breakpointId);
    StringVector getDebugInfo() const;
    void dumpOperationState() const;
    
    // BaseNode接口重写
    NodeType getType() const override { return NodeType::CUSTOM; }
    String getValue() const override;
    std::unique_ptr<BaseNode> clone() const override;
    
    // 序列化增强
    String toEnhancedString() const;
    String toExecutionPlan() const;
    String toDebugString() const;
    
    // 静态工厂方法
    static std::unique_ptr<EnhancedCustomNode> createDeleteOperation(const Position& pos = Position());
    static std::unique_ptr<EnhancedCustomNode> createInsertOperation(const Position& pos = Position());
    static std::unique_ptr<EnhancedCustomNode> createReplaceOperation(const Position& pos = Position());
    static std::unique_ptr<EnhancedCustomNode> createMergeOperation(const Position& pos = Position());
    static std::unique_ptr<EnhancedCustomNode> createBatchOperation(const Position& pos = Position());
    
    // 工具方法
    static EnhancedCustomOperationType parseOperationType(const String& typeString);
    static String operationTypeToString(EnhancedCustomOperationType type);
    static bool isDestructiveOperation(EnhancedCustomOperationType type);
    static bool requiresValidation(EnhancedCustomOperationType type);
    
private:
    bool debugMode;                     // 调试模式
    StringUnorderedSet breakpoints;     // 断点集合
    String transactionId;               // 事务ID
    bool inTransaction;                 // 是否在事务中
    StringVector transactionLog;        // 事务日志
    CustomOperationResult cachedResult; // 缓存的结果
    bool resultCached;                  // 结果是否已缓存
    
    // 内部执行方法
    CustomOperationResult executeInternal();
    CustomOperationResult executeWithValidation();
    CustomOperationResult executeWithContext();
    
    // 内部验证方法
    bool validateOperands() const;
    bool validateOptions() const;
    bool validateContext() const;
    
    // 内部工具方法
    void updateContext();
    void logOperation(const String& operation);
    void recordExecutionTime(double time);
    bool shouldBreak(const String& breakpointId) const;
    String generateTransactionId() const;
    void cleanup();
    
    // 错误处理
    CustomOperationResult createErrorResult(const String& error) const;
    CustomOperationResult createSuccessResult(const String& result) const;
    void addWarning(CustomOperationResult& result, const String& warning) const;
    void addError(CustomOperationResult& result, const String& error) const;
};

// 自定义操作管理器
class EnhancedCustomOperationManager {
public:
    static EnhancedCustomOperationManager& getInstance();
    
    // 操作注册
    void registerOperationType(EnhancedCustomOperationType type, 
                              std::shared_ptr<CustomOperationValidator> validator);
    void unregisterOperationType(EnhancedCustomOperationType type);
    std::shared_ptr<CustomOperationValidator> getValidator(EnhancedCustomOperationType type) const;
    bool hasValidator(EnhancedCustomOperationType type) const;
    
    // 全局设置
    void setGlobalDryRunMode(bool dryRun);
    bool isGlobalDryRunMode() const;
    void setGlobalCaching(bool caching);
    bool isGlobalCachingEnabled() const;
    void setMaxOperationDepth(size_t maxDepth);
    size_t getMaxOperationDepth() const;
    
    // 操作统计
    size_t getExecutedOperationCount() const;
    size_t getFailedOperationCount() const;
    double getAverageExecutionTime() const;
    std::unordered_map<String, size_t> getOperationTypeStatistics() const;
    
    // 缓存管理
    void clearAllCaches();
    void setCacheSize(size_t maxSize);
    size_t getCacheHitCount() const;
    size_t getCacheMissCount() const;
    double getCacheHitRatio() const;
    
    // 事务管理
    String beginGlobalTransaction();
    void commitGlobalTransaction(const String& transactionId);
    void rollbackGlobalTransaction(const String& transactionId);
    bool isGlobalTransactionActive() const;
    
    // 错误处理
    void setGlobalErrorHandler(std::function<void(const CustomOperationResult&)> handler);
    void setGlobalWarningHandler(std::function<void(const String&)> handler);
    
    // 调试支持
    void enableGlobalDebugMode(bool enable = true);
    void dumpGlobalStatistics() const;
    void exportOperationLog(const String& filePath) const;
    
private:
    EnhancedCustomOperationManager() = default;
    ~EnhancedCustomOperationManager() = default;
    
    std::unordered_map<EnhancedCustomOperationType, std::shared_ptr<CustomOperationValidator>> validators;
    std::unordered_map<String, CustomOperationResult> operationCache;
    std::unordered_map<String, StringVector> transactionLogs;
    
    bool globalDryRunMode;
    bool globalCachingEnabled;
    bool globalDebugMode;
    size_t maxOperationDepth;
    size_t maxCacheSize;
    
    size_t executedOperations;
    size_t failedOperations;
    double totalExecutionTime;
    size_t cacheHits;
    size_t cacheMisses;
    
    String currentGlobalTransaction;
    std::function<void(const CustomOperationResult&)> errorHandler;
    std::function<void(const String&)> warningHandler;
    
    // 内部方法
    void initializeDefaultValidators();
    void updateStatistics(const CustomOperationResult& result);
    bool isValidCacheKey(const String& key) const;
    void cleanupExpiredCache();
};

// 操作构建器
class EnhancedCustomOperationBuilder {
public:
    explicit EnhancedCustomOperationBuilder(EnhancedCustomOperationType type);
    ~EnhancedCustomOperationBuilder() = default;
    
    // 基础配置
    EnhancedCustomOperationBuilder& target(const String& targetName);
    EnhancedCustomOperationBuilder& source(const String& sourceName);
    EnhancedCustomOperationBuilder& content(const String& operationContent);
    EnhancedCustomOperationBuilder& position(const String& positionSpec);
    
    // 操作数配置
    EnhancedCustomOperationBuilder& operand(std::shared_ptr<BaseNode> node);
    EnhancedCustomOperationBuilder& operands(const std::vector<std::shared_ptr<BaseNode>>& nodes);
    
    // 选项配置
    EnhancedCustomOperationBuilder& option(const String& key, const String& value);
    EnhancedCustomOperationBuilder& options(const StringUnorderedMap& opts);
    
    // 条件配置
    EnhancedCustomOperationBuilder& precondition(const String& condition);
    EnhancedCustomOperationBuilder& postcondition(const String& condition);
    
    // 验证配置
    EnhancedCustomOperationBuilder& validator(std::shared_ptr<CustomOperationValidator> val);
    EnhancedCustomOperationBuilder& enableValidation(bool enable = true);
    
    // 事务配置
    EnhancedCustomOperationBuilder& transactional(bool enable = true);
    EnhancedCustomOperationBuilder& enableRollback(bool enable = true);
    
    // 缓存配置
    EnhancedCustomOperationBuilder& cached(bool enable = true);
    EnhancedCustomOperationBuilder& cacheKey(const String& key);
    
    // 调试配置
    EnhancedCustomOperationBuilder& debug(bool enable = true);
    EnhancedCustomOperationBuilder& breakpoint(const String& breakpointId);
    
    // 构建方法
    std::unique_ptr<EnhancedCustomNode> build();
    String buildToString();
    CustomOperationResult execute();
    CustomOperationResult dryRun();
    
private:
    std::unique_ptr<EnhancedCustomNode> operation;
    StringVector buildErrors;
    
    bool validateBuild() const;
    void addBuildError(const String& error);
};

// 操作模式匹配器
class CustomOperationPatternMatcher {
public:
    CustomOperationPatternMatcher() = default;
    ~CustomOperationPatternMatcher() = default;
    
    // 模式注册
    void registerPattern(const String& pattern, EnhancedCustomOperationType type);
    void unregisterPattern(const String& pattern);
    
    // 模式匹配
    EnhancedCustomOperationType matchPattern(const String& input) const;
    StringVector getAllPatterns() const;
    bool hasPattern(const String& pattern) const;
    
    // 模式验证
    bool isValidPattern(const String& pattern) const;
    StringVector extractParameters(const String& pattern, const String& input) const;
    
    // 内置模式
    void registerBuiltinPatterns();
    
private:
    std::unordered_map<String, EnhancedCustomOperationType> patterns;
    
    // 内部方法
    bool matchesPattern(const String& pattern, const String& input) const;
    String normalizePattern(const String& pattern) const;
};

} // namespace CHTL