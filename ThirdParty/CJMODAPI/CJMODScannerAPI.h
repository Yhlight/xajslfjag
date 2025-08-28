#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "Arg.h"

namespace CJMODAPI {

class CJMODScannerAPI {
public:
	static Arg scan(const Arg& pattern, const std::string& keyword, const std::string& source) {
		// 简化的扫描实现，避免循环依赖
		std::vector<std::string> tokens;
		
		// 简单的token分割
		std::istringstream iss(source);
		std::string token;
		while (iss >> token) {
			tokens.push_back(token);
		}
		
		// 基于关键字对齐：合并tokens为连续字符串并查找keyword的边界
		int idx = -1;
		for (size_t i = 0; i < tokens.size(); ++i) {
			if (tokens[i] == keyword) { 
				idx = static_cast<int>(i); 
				break; 
			}
		}
		
		if (idx >= 0) {
			std::string left;
			for (int i = 0; i < idx; ++i) {
				if (!tokens[i].empty() && !std::isspace(static_cast<unsigned char>(tokens[i][0]))) {
					if (!left.empty()) left += " ";
					left += tokens[i];
				}
			}
			
			std::string right;
			for (size_t i = idx + 1; i < tokens.size(); ++i) {
				if (!tokens[i].empty() && !std::isspace(static_cast<unsigned char>(tokens[i][0]))) {
					if (!right.empty()) right += " ";
					right += tokens[i];
				}
			}
			
			std::vector<std::string> out;
			if (!left.empty()) out.push_back(left); else out.push_back("");
			out.push_back(keyword);
			if (!right.empty()) out.push_back(right); else out.push_back("");
			return Arg(out);
		}
		
		// 未找到关键字，直接返回原始切分
		return Arg(tokens);
	}
};

} // namespace CJMODAPI