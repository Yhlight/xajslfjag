#pragma once

#include <string>
#include <vector>
#include "Arg.h"
#include "../../Scanner/CHTLUnifiedScanner.h"

namespace CJMODAPI {

class CJMODScannerAPI {
public:
	static Arg scan(const Arg& /*pattern*/, const std::string& keyword, const std::string& source) {
		Scanner::CHTLUnifiedScanner s; s.setSource(source);
		auto r = s.scanCJMODByTwoPointers(0, source.size());
		// 基于关键字对齐：合并tokens为连续字符串并查找keyword的边界
		// 简化策略：遍历tokens定位等于keyword的token；若找到，尝试组装[left, keyword, right]
		int idx = -1;
		for (size_t i = 0; i < r.tokens.size(); ++i) {
			if (r.tokens[i] == keyword) { idx = static_cast<int>(i); break; }
		}
		if (idx >= 0) {
			std::string left;
			for (int i = 0; i < idx; ++i) {
				if (!r.tokens[i].empty() && !std::isspace(static_cast<unsigned char>(r.tokens[i][0]))) {
					if (!left.empty()) left += " ";
					left += r.tokens[i];
				}
			}
			std::string right;
			for (size_t i = idx + 1; i < r.tokens.size(); ++i) {
				if (!r.tokens[i].empty() && !std::isspace(static_cast<unsigned char>(r.tokens[i][0]))) {
					if (!right.empty()) right += " ";
					right += r.tokens[i];
				}
			}
			std::vector<std::string> out;
			if (!left.empty()) out.push_back(left); else out.push_back("");
			out.push_back(keyword);
			if (!right.empty()) out.push_back(right); else out.push_back("");
			return Arg(out);
		}
		// 未找到关键字，直接返回原始切分
		return Arg(r.tokens);
	}
};

} // namespace CJMODAPI