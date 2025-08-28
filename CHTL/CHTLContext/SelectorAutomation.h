#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

struct SelectorConfig {
	bool disableStyleAutoAddClass = false;
	bool disableStyleAutoAddId = false;
	bool disableScriptAutoAddClass = true;
	bool disableScriptAutoAddId = true;
};

class SelectorAutomation {
public:
	SelectorAutomation() = default;
	
	// 设置配置
	void setConfig(const SelectorConfig& config) { config_ = config; }
	
	// 局部样式块自动化
	std::string processLocalStyle(const std::string& styleContent, const std::string& elementSelector);
	
	// 局部脚本块自动化
	std::string processLocalScript(const std::string& scriptContent, const std::string& elementSelector);
	
	// 引用选择器处理 (&)
	std::string resolveReferenceSelector(const std::string& selector, const std::string& elementSelector);
	
	// 自动添加class/id
	std::string autoAddSelector(const std::string& content, const std::string& elementSelector, bool isStyle);

private:
	SelectorConfig config_;
	
	// 辅助函数
	std::vector<std::string> extractSelectors(const std::string& content);
	std::string findFirstClassSelector(const std::vector<std::string>& selectors);
	std::string findFirstIdSelector(const std::vector<std::string>& selectors);
	bool hasClassAttribute(const std::string& elementSelector);
	bool hasIdAttribute(const std::string& elementSelector);
};

} // namespace CHTL