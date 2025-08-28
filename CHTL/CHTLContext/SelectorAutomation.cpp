#include "SelectorAutomation.h"
#include <regex>
#include <algorithm>

namespace CHTL {

std::string SelectorAutomation::processLocalStyle(const std::string& styleContent, const std::string& elementSelector) {
	if (config_.disableStyleAutoAddClass && config_.disableStyleAutoAddId) {
		return styleContent; // 完全禁用自动化
	}
	
	auto selectors = extractSelectors(styleContent);
	std::string result = styleContent;
	
	// 自动添加class
	if (!config_.disableStyleAutoAddClass) {
		std::string firstClass = findFirstClassSelector(selectors);
		if (!firstClass.empty() && !hasClassAttribute(elementSelector)) {
			// 在元素选择器中添加class
			if (elementSelector.find("class=") == std::string::npos) {
				result = elementSelector + " class=\"" + firstClass.substr(1) + "\"";
			}
		}
	}
	
	// 自动添加id
	if (!config_.disableStyleAutoAddId) {
		std::string firstId = findFirstIdSelector(selectors);
		if (!firstId.empty() && !hasIdAttribute(elementSelector)) {
			// 在元素选择器中添加id
			if (elementSelector.find("id=") == std::string::npos) {
				if (result != elementSelector) result += " ";
				result += "id=\"" + firstId.substr(1) + "\"";
			}
		}
	}
	
	return result;
}

std::string SelectorAutomation::processLocalScript(const std::string& scriptContent, const std::string& elementSelector) {
	if (config_.disableScriptAutoAddClass && config_.disableScriptAutoAddId) {
		return scriptContent; // 完全禁用自动化
	}
	
	auto selectors = extractSelectors(scriptContent);
	std::string result = scriptContent;
	
	// 自动添加class（如果style没有触发）
	if (!config_.disableScriptAutoAddClass && !hasClassAttribute(elementSelector)) {
		std::string firstClass = findFirstClassSelector(selectors);
		if (!firstClass.empty()) {
			// 在script中添加class引用
			result = "class=\"" + firstClass.substr(1) + "\"; " + result;
		}
	}
	
	// 自动添加id（如果style没有触发）
	if (!config_.disableScriptAutoAddId && !hasIdAttribute(elementSelector)) {
		std::string firstId = findFirstIdSelector(selectors);
		if (!firstId.empty()) {
			// 在script中添加id引用
			result = "id=\"" + firstId.substr(1) + "\"; " + result;
		}
	}
	
	return result;
}

std::string SelectorAutomation::resolveReferenceSelector(const std::string& selector, const std::string& elementSelector) {
	if (selector.find("&") == std::string::npos) {
		return selector; // 不是引用选择器
	}
	
	// 对于局部style，&优先选择class
	if (!config_.disableStyleAutoAddClass) {
		std::string classAttr = extractClassAttribute(elementSelector);
		if (!classAttr.empty()) {
			return std::regex_replace(selector, std::regex("&"), "." + classAttr);
		}
	}
	
	// 对于局部script，&优先选择id
	if (!config_.disableScriptAutoAddId) {
		std::string idAttr = extractIdAttribute(elementSelector);
		if (!idAttr.empty()) {
			return std::regex_replace(selector, std::regex("&"), "#" + idAttr);
		}
	}
	
	return selector; // 无法解析引用
}

std::string SelectorAutomation::autoAddSelector(const std::string& content, const std::string& elementSelector, bool isStyle) {
	if (isStyle) {
		return processLocalStyle(content, elementSelector);
	} else {
		return processLocalScript(content, elementSelector);
	}
}

std::vector<std::string> SelectorAutomation::extractSelectors(const std::string& content) {
	std::vector<std::string> selectors;
	
	// 简化实现：查找CSS选择器模式
	std::regex selectorPattern(R"(([.#][a-zA-Z_][a-zA-Z0-9_-]*))");
	std::sregex_iterator iter(content.begin(), content.end(), selectorPattern);
	std::sregex_iterator end;
	
	for (; iter != end; ++iter) {
		selectors.push_back(iter->str());
	}
	
	return selectors;
}

std::string SelectorAutomation::findFirstClassSelector(const std::vector<std::string>& selectors) {
	for (const auto& selector : selectors) {
		if (selector[0] == '.') {
			return selector;
		}
	}
	return "";
}

std::string SelectorAutomation::findFirstIdSelector(const std::vector<std::string>& selectors) {
	for (const auto& selector : selectors) {
		if (selector[0] == '#') {
			return selector;
		}
	}
	return "";
}

bool SelectorAutomation::hasClassAttribute(const std::string& elementSelector) {
	return elementSelector.find("class=") != std::string::npos;
}

bool SelectorAutomation::hasIdAttribute(const std::string& elementSelector) {
	return elementSelector.find("id=") != std::string::npos;
}

std::string SelectorAutomation::extractClassAttribute(const std::string& elementSelector) {
	// 简化实现：提取class值
	std::regex classPattern("class=\"([^\"]*)\"");
	std::smatch match;
	if (std::regex_search(elementSelector, match, classPattern)) {
		return match[1].str();
	}
	return "";
}

std::string SelectorAutomation::extractIdAttribute(const std::string& elementSelector) {
	// 简化实现：提取id值
	std::regex idPattern("id=\"([^\"]*)\"");
	std::smatch match;
	if (std::regex_search(elementSelector, match, idPattern)) {
		return match[1].str();
	}
	return "";
}

} // namespace CHTL