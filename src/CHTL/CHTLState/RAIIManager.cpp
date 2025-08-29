#include "RAIIManager.h"
#include <algorithm>
#include <stack>

namespace CHTL {

// ResourceHandle实现
ResourceHandle::ResourceHandle(const String& name, ResourceType type, void* ptr)
    : resourceName(name), type(type), resource(ptr), isValid(true), 
      refCount(1), creationTime(std::chrono::steady_clock::now()) {
}

ResourceHandle::~ResourceHandle() {
    if (isValid && resource && destructor) {
        destructor(resource);
    }
}

void ResourceHandle::addRef() {
    refCount++;
}

void ResourceHandle::release() {
    if (--refCount == 0) {
        isValid = false;
        if (resource && destructor) {
            destructor(resource);
            resource = nullptr;
        }
    }
}

bool ResourceHandle::isExpired() const {
    if (ttl.count() == 0) return false;
    
    auto now = std::chrono::steady_clock::now();
    return (now - creationTime) > ttl;
}

void ResourceHandle::setTTL(std::chrono::milliseconds timeToLive) {
    ttl = timeToLive;
}

void ResourceHandle::setDestructor(std::function<void(void*)> destructor_) {
    destructor = destructor_;
}

// StateSnapshot实现
StateSnapshot::StateSnapshot(const String& name) 
    : snapshotName(name), timestamp(std::chrono::steady_clock::now()) {
}

void StateSnapshot::saveVariable(const String& name, const String& value) {
    variables[name] = value;
}

String StateSnapshot::getVariable(const String& name) const {
    auto it = variables.find(name);
    return (it != variables.end()) ? it->second : "";
}

void StateSnapshot::saveResource(const String& name, std::shared_ptr<ResourceHandle> resource) {
    resources[name] = resource;
}

std::shared_ptr<ResourceHandle> StateSnapshot::getResource(const String& name) const {
    auto it = resources.find(name);
    return (it != resources.end()) ? it->second : nullptr;
}

bool StateSnapshot::hasVariable(const String& name) const {
    return variables.find(name) != variables.end();
}

bool StateSnapshot::hasResource(const String& name) const {
    return resources.find(name) != resources.end();
}

StringVector StateSnapshot::getVariableNames() const {
    StringVector names;
    for (const auto& pair : variables) {
        names.push_back(pair.first);
    }
    return names;
}

StringVector StateSnapshot::getResourceNames() const {
    StringVector names;
    for (const auto& pair : resources) {
        names.push_back(pair.first);
    }
    return names;
}

// ContextScope实现
ContextScope::ContextScope(const String& name, ContextType type) 
    : scopeName(name), type(type), isActive(true), 
      creationTime(std::chrono::steady_clock::now()) {
}

ContextScope::~ContextScope() {
    if (isActive) {
        cleanup();
    }
}

void ContextScope::addResource(const String& name, std::shared_ptr<ResourceHandle> resource) {
    if (resource) {
        resources[name] = resource;
        resource->addRef();
    }
}

void ContextScope::removeResource(const String& name) {
    auto it = resources.find(name);
    if (it != resources.end()) {
        if (it->second) {
            it->second->release();
        }
        resources.erase(it);
    }
}

std::shared_ptr<ResourceHandle> ContextScope::getResource(const String& name) const {
    auto it = resources.find(name);
    return (it != resources.end()) ? it->second : nullptr;
}

void ContextScope::setVariable(const String& name, const String& value) {
    variables[name] = value;
}

String ContextScope::getVariable(const String& name) const {
    auto it = variables.find(name);
    return (it != variables.end()) ? it->second : "";
}

void ContextScope::addExitHandler(std::function<void()> handler) {
    exitHandlers.push_back(handler);
}

void ContextScope::cleanup() {
    if (!isActive) return;
    
    // 执行退出处理器
    for (auto& handler : exitHandlers) {
        try {
            if (handler) {
                handler();
            }
        } catch (...) {
            // 忽略退出处理器中的异常
        }
    }
    
    // 释放所有资源
    for (auto& pair : resources) {
        if (pair.second) {
            pair.second->release();
        }
    }
    
    resources.clear();
    variables.clear();
    exitHandlers.clear();
    isActive = false;
}

bool ContextScope::hasResource(const String& name) const {
    return resources.find(name) != resources.end();
}

bool ContextScope::hasVariable(const String& name) const {
    return variables.find(name) != variables.end();
}

StringVector ContextScope::getResourceNames() const {
    StringVector names;
    for (const auto& pair : resources) {
        names.push_back(pair.first);
    }
    return names;
}

StringVector ContextScope::getVariableNames() const {
    StringVector names;
    for (const auto& pair : variables) {
        names.push_back(pair.first);
    }
    return names;
}

// RAIIManager实现
RAIIManager::RAIIManager(const RAIIConfig& config) 
    : config(config), isShuttingDown(false) {
    // 启动清理线程
    if (config.enableAutoCleanup) {
        startCleanupThread();
    }
}

RAIIManager::~RAIIManager() {
    shutdown();
}

std::shared_ptr<ResourceHandle> RAIIManager::acquireResource(const String& name, ResourceType type, void* resource) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (isShuttingDown) {
        return nullptr;
    }
    
    // 检查是否已存在相同名称的资源
    auto it = resources.find(name);
    if (it != resources.end() && it->second && it->second->isValid) {
        it->second->addRef();
        return it->second;
    }
    
    // 创建新的资源句柄
    auto handle = std::make_shared<ResourceHandle>(name, type, resource);
    
    // 设置资源特定的析构函数
    setResourceDestructor(handle, type);
    
    // 如果启用了TTL，设置过期时间
    if (config.enableTTL) {
        handle->setTTL(config.defaultTTL);
    }
    
    resources[name] = handle;
    
    // 记录资源获取
    if (config.enableLogging) {
        logResourceAcquisition(name, type);
    }
    
    return handle;
}

void RAIIManager::releaseResource(const String& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = resources.find(name);
    if (it != resources.end()) {
        if (it->second) {
            it->second->release();
            if (it->second->refCount <= 0) {
                resources.erase(it);
            }
        }
        
        if (config.enableLogging) {
            logResourceRelease(name);
        }
    }
}

std::shared_ptr<ResourceHandle> RAIIManager::getResource(const String& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = resources.find(name);
    if (it != resources.end() && it->second && it->second->isValid) {
        return it->second;
    }
    
    return nullptr;
}

std::unique_ptr<ContextScope> RAIIManager::createScope(const String& name, ContextType type) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (isShuttingDown) {
        return nullptr;
    }
    
    auto scope = std::make_unique<ContextScope>(name, type);
    
    // 记录作用域创建
    if (config.enableLogging) {
        logScopeCreation(name, type);
    }
    
    return scope;
}

std::unique_ptr<StateSnapshot> RAIIManager::takeSnapshot(const String& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto snapshot = std::make_unique<StateSnapshot>(name);
    
    // 保存当前所有资源状态
    for (const auto& pair : resources) {
        if (pair.second && pair.second->isValid) {
            snapshot->saveResource(pair.first, pair.second);
        }
    }
    
    // 保存全局变量（如果有的话）
    for (const auto& pair : globalVariables) {
        snapshot->saveVariable(pair.first, pair.second);
    }
    
    snapshots[name] = std::shared_ptr<StateSnapshot>(snapshot.release());
    
    if (config.enableLogging) {
        logSnapshotCreation(name);
    }
    
    return std::unique_ptr<StateSnapshot>(snapshots[name].get());
}

bool RAIIManager::restoreSnapshot(const String& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = snapshots.find(name);
    if (it == snapshots.end() || !it->second) {
        return false;
    }
    
    auto snapshot = it->second;
    
    try {
        // 清理当前资源
        for (auto& pair : resources) {
            if (pair.second) {
                pair.second->release();
            }
        }
        resources.clear();
        
        // 恢复资源
        for (const String& resourceName : snapshot->getResourceNames()) {
            auto resource = snapshot->getResource(resourceName);
            if (resource && resource->isValid) {
                resources[resourceName] = resource;
                resource->addRef();
            }
        }
        
        // 恢复变量
        globalVariables.clear();
        for (const String& varName : snapshot->getVariableNames()) {
            String varValue = snapshot->getVariable(varName);
            globalVariables[varName] = varValue;
        }
        
        if (config.enableLogging) {
            logSnapshotRestore(name);
        }
        
        return true;
        
    } catch (...) {
        return false;
    }
}

void RAIIManager::deleteSnapshot(const String& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    snapshots.erase(name);
    
    if (config.enableLogging) {
        logSnapshotDeletion(name);
    }
}

void RAIIManager::setGlobalVariable(const String& name, const String& value) {
    std::lock_guard<std::mutex> lock(mutex);
    globalVariables[name] = value;
}

String RAIIManager::getGlobalVariable(const String& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = globalVariables.find(name);
    return (it != globalVariables.end()) ? it->second : "";
}

void RAIIManager::performCleanup() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (isShuttingDown) return;
    
    // 清理过期资源
    cleanupExpiredResources();
    
    // 清理无效快照
    cleanupInvalidSnapshots();
    
    // 垃圾收集
    if (config.enableGarbageCollection) {
        performGarbageCollection();
    }
}

void RAIIManager::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        isShuttingDown = true;
    }
    
    // 停止清理线程
    stopCleanupThread();
    
    // 清理所有资源
    std::lock_guard<std::mutex> lock(mutex);
    
    for (auto& pair : resources) {
        if (pair.second) {
            pair.second->release();
        }
    }
    
    resources.clear();
    snapshots.clear();
    globalVariables.clear();
    
    if (config.enableLogging) {
        logShutdown();
    }
}

void RAIIManager::startCleanupThread() {
    cleanupThread = std::thread([this]() {
        while (!isShuttingDown) {
            std::this_thread::sleep_for(config.cleanupInterval);
            
            if (!isShuttingDown) {
                performCleanup();
            }
        }
    });
}

void RAIIManager::stopCleanupThread() {
    if (cleanupThread.joinable()) {
        cleanupThread.join();
    }
}

void RAIIManager::cleanupExpiredResources() {
    auto it = resources.begin();
    while (it != resources.end()) {
        if (!it->second || !it->second->isValid || it->second->isExpired()) {
            if (it->second) {
                it->second->release();
            }
            it = resources.erase(it);
        } else {
            ++it;
        }
    }
}

void RAIIManager::cleanupInvalidSnapshots() {
    auto it = snapshots.begin();
    while (it != snapshots.end()) {
        if (!it->second) {
            it = snapshots.erase(it);
        } else {
            ++it;
        }
    }
}

void RAIIManager::performGarbageCollection() {
    // 检查引用计数为0的资源
    auto it = resources.begin();
    while (it != resources.end()) {
        if (it->second && it->second->refCount <= 0) {
            it->second->release();
            it = resources.erase(it);
        } else {
            ++it;
        }
    }
}

void RAIIManager::setResourceDestructor(std::shared_ptr<ResourceHandle> handle, ResourceType type) {
    if (!handle) return;
    
    switch (type) {
        case ResourceType::MEMORY:
            handle->setDestructor([](void* ptr) {
                if (ptr) {
                    delete static_cast<char*>(ptr);
                }
            });
            break;
            
        case ResourceType::FILE:
            handle->setDestructor([](void* ptr) {
                if (ptr) {
                    auto* file = static_cast<std::FILE*>(ptr);
                    std::fclose(file);
                }
            });
            break;
            
        case ResourceType::AST_NODE:
            handle->setDestructor([](void* ptr) {
                if (ptr) {
                    delete static_cast<BaseNode*>(ptr);
                }
            });
            break;
            
        case ResourceType::PARSER:
            handle->setDestructor([](void* ptr) {
                // Parser有特殊的清理需求
                if (ptr) {
                    // 具体的Parser析构逻辑
                }
            });
            break;
            
        case ResourceType::GENERATOR:
            handle->setDestructor([](void* ptr) {
                // Generator有特殊的清理需求
                if (ptr) {
                    // 具体的Generator析构逻辑
                }
            });
            break;
            
        default:
            // 默认不设置析构函数
            break;
    }
}

void RAIIManager::logResourceAcquisition(const String& name, ResourceType type) {
    logs.push_back("资源获取: " + name + " (类型: " + resourceTypeToString(type) + ")");
}

void RAIIManager::logResourceRelease(const String& name) {
    logs.push_back("资源释放: " + name);
}

void RAIIManager::logScopeCreation(const String& name, ContextType type) {
    logs.push_back("作用域创建: " + name + " (类型: " + contextTypeToString(type) + ")");
}

void RAIIManager::logSnapshotCreation(const String& name) {
    logs.push_back("快照创建: " + name);
}

void RAIIManager::logSnapshotRestore(const String& name) {
    logs.push_back("快照恢复: " + name);
}

void RAIIManager::logSnapshotDeletion(const String& name) {
    logs.push_back("快照删除: " + name);
}

void RAIIManager::logShutdown() {
    logs.push_back("RAII管理器关闭");
}

String RAIIManager::resourceTypeToString(ResourceType type) {
    switch (type) {
        case ResourceType::MEMORY: return "MEMORY";
        case ResourceType::FILE: return "FILE";
        case ResourceType::AST_NODE: return "AST_NODE";
        case ResourceType::PARSER: return "PARSER";
        case ResourceType::GENERATOR: return "GENERATOR";
        default: return "UNKNOWN";
    }
}

String RAIIManager::contextTypeToString(ContextType type) {
    switch (type) {
        case ContextType::GLOBAL: return "GLOBAL";
        case ContextType::NAMESPACE: return "NAMESPACE";
        case ContextType::FUNCTION: return "FUNCTION";
        case ContextType::BLOCK: return "BLOCK";
        case ContextType::TEMPLATE: return "TEMPLATE";
        default: return "UNKNOWN";
    }
}

bool RAIIManager::hasResource(const String& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = resources.find(name);
    return it != resources.end() && it->second && it->second->isValid;
}

bool RAIIManager::hasSnapshot(const String& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    return snapshots.find(name) != snapshots.end();
}

StringVector RAIIManager::getResourceNames() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    StringVector names;
    for (const auto& pair : resources) {
        if (pair.second && pair.second->isValid) {
            names.push_back(pair.first);
        }
    }
    return names;
}

StringVector RAIIManager::getSnapshotNames() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    StringVector names;
    for (const auto& pair : snapshots) {
        names.push_back(pair.first);
    }
    return names;
}

size_t RAIIManager::getResourceCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    return resources.size();
}

size_t RAIIManager::getSnapshotCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    return snapshots.size();
}

StringVector RAIIManager::getLogs() const {
    std::lock_guard<std::mutex> lock(mutex);
    return logs;
}

void RAIIManager::clearLogs() {
    std::lock_guard<std::mutex> lock(mutex);
    logs.clear();
}

void RAIIManager::setConfig(const RAIIConfig& newConfig) {
    std::lock_guard<std::mutex> lock(mutex);
    config = newConfig;
}

RAIIConfig RAIIManager::getConfig() const {
    std::lock_guard<std::mutex> lock(mutex);
    return config;
}

} // namespace CHTL