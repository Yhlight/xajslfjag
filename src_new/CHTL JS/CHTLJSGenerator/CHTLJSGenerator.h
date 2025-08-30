#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../CHTLJSNode/BaseNode.h"
#include "../CHTLJSContext/Context.h"

namespace CHTL {

class CHTLJSGenerator {
public:
    CHTLJSGenerator();
    ~CHTLJSGenerator();
    
    // 主生成接口
    std::string generateJavaScript(std::shared_ptr<BaseNode> rootNode);
    
    // 设置和配置
    void setContext(std::shared_ptr<CHTLJSContext> context);
    
    // 调试
    void enableDebugMode(bool enable) { m_debugMode = enable; }
    std::vector<std::string> getErrors() const { return m_errors; }
    std::vector<std::string> getWarnings() const { return m_warnings; }
    void clearErrors();
    
private:
    std::shared_ptr<CHTLJSContext> m_context;
    bool m_debugMode;
    std::vector<std::string> m_errors;
    std::vector<std::string> m_warnings;
    
    // 生成方法
    std::string generateJSNode(std::shared_ptr<BaseNode> node, int depth = 0);
    std::string generateModuleImport(std::shared_ptr<BaseNode> node, int depth);
    std::string generateEnhancedSelector(std::shared_ptr<BaseNode> node);
    std::string generateEventBinding(std::shared_ptr<BaseNode> node, int depth);
    std::string generateAnimateBlock(std::shared_ptr<BaseNode> node, int depth);
    std::string generateListenBlock(std::shared_ptr<BaseNode> node, int depth);
    std::string generateVirtualObject(std::shared_ptr<BaseNode> node, int depth);
    std::string generateDelegateBlock(std::shared_ptr<BaseNode> node, int depth);
    
    // 辅助方法
    std::string getIndentation(int depth) const;
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
};

}