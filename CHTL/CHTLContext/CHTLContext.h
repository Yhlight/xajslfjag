#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "../CHTLState/CHTLState.h"

namespace CHTL {

class CHTLGlobalMap; // 前置声明，已独立实现

class CHTLContext {
public:
	CHTLContext();
	~CHTLContext() = default;

	void setFilePath(const std::string& path) { filePath_ = path; }
	const std::string& getFilePath() const { return filePath_; }

	CHTLStateStack& state() { return stateStack_; }
	const CHTLStateStack& state() const { return stateStack_; }

	void setGlobalMap(std::shared_ptr<CHTLGlobalMap> map) { globalMap_ = std::move(map); }
	std::shared_ptr<CHTLGlobalMap> getGlobalMap() const { return globalMap_; }

private:
	std::string filePath_;
	CHTLStateStack stateStack_;
	std::shared_ptr<CHTLGlobalMap> globalMap_;
};

} // namespace CHTL