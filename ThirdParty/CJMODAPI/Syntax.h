#pragma once

#include <string>
#include <vector>
#include <cctype>

namespace CJMODAPI {

class Arg; // 前置声明

class Syntax {
public:
	static Arg analyze(const std::string& pattern) {
		// 简化实现：按空白分割并原样返回到Arg中
		std::vector<std::string> toks;
		std::string buf;
		for (char c : pattern) {
			if (std::isspace(static_cast<unsigned char>(c))) {
				if (!buf.empty()) { toks.push_back(buf); buf.clear(); }
			} else {
				buf.push_back(c);
			}
		}
		if (!buf.empty()) toks.push_back(buf);
		return Arg(toks);
	}

	static bool isObject(const std::string& s) {
		// 粗略判断：以{开头以}结尾
		if (s.size() < 2) return false;
		return s.front() == '{' && s.back() == '}';
	}

	static bool isFunction(const std::string& s) {
		// 粗略判断：包含"function"或"=>"
		return s.find("function") != std::string::npos || s.find("=>") != std::string::npos;
	}

	static bool isArray(const std::string& s) {
		if (s.size() < 2) return false;
		return s.front() == '[' && s.back() == ']';
	}

	static bool isCHTLJSFunction(const std::string& s) {
		// 粗略判断：形如 name { ... }
		auto p = s.find('{');
		auto q = s.rfind('}');
		if (p == std::string::npos || q == std::string::npos || q <= p) return false;
		// 名称部分为标识符
		if (p == 0) return false;
		char c = s[0];
		return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
	}
};

} // namespace CJMODAPI