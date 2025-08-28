#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace CHTL {

// 全局符号类型
enum class GlobalSymbolType {
    TEMPLATE_STYLE,
    TEMPLATE_ELEMENT,
    TEMPLATE_VAR,
    CUSTOM_STYLE,
    CUSTOM_ELEMENT,
    CUSTOM_VAR,
    ORIGIN_HTML,
    ORIGIN_STYLE,
    ORIGIN_JAVASCRIPT,
    NAMESPACE,
    CONFIGURATION
};

// 全局符号信息
struct GlobalSymbol {
    std::string name;
    GlobalSymbolType type;
    std::string namespace_;
    std::string filePath;
    size_t line;
    size_t column;
    
    GlobalSymbol(const std::string& name, GlobalSymbolType type, 
                 const std::string& namespace_ = "", 
                 const std::string& filePath = "", 
                 size_t line = 0, size_t column = 0)
        : name(name), type(type), namespace_(namespace_), 
          filePath(filePath), line(line), column(column) {}
};

// CHTL全局映射表
class CHTLGlobalMap {
public:
    CHTLGlobalMap();
    ~CHTLGlobalMap() = default;
    
    // 添加符号
    void addSymbol(const GlobalSymbol& symbol);
    
    // 查找符号
    std::shared_ptr<GlobalSymbol> findSymbol(const std::string& name, 
                                            GlobalSymbolType type = GlobalSymbolType::TEMPLATE_STYLE,
                                            const std::string& namespace_ = "");
    
    // 查找命名空间中的符号
    std::vector<std::shared_ptr<GlobalSymbol>> findSymbolsInNamespace(const std::string& namespace_);
    
    // 检查符号是否存在
    bool hasSymbol(const std::string& name, 
                  GlobalSymbolType type = GlobalSymbolType::TEMPLATE_STYLE,
                  const std::string& namespace_ = "");
    
    // 移除符号
    bool removeSymbol(const std::string& name, 
                     GlobalSymbolType type = GlobalSymbolType::TEMPLATE_STYLE,
                     const std::string& namespace_ = "");
    
    // 清空所有符号
    void clear();
    
    // 获取所有符号
    const std::vector<std::shared_ptr<GlobalSymbol>>& getAllSymbols() const { return symbols_; }
    
    // 获取符号数量
    size_t getSymbolCount() const { return symbols_.size(); }
    
private:
    std::vector<std::shared_ptr<GlobalSymbol>> symbols_;
    
    // 创建符号的辅助函数
    std::shared_ptr<GlobalSymbol> createSymbol(const GlobalSymbol& symbol);
};

} // namespace CHTL