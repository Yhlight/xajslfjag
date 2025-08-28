#pragma once

#include <string>
#include "Arg.h"
#include "../../Scanner/CHTLUnifiedScanner.h"

namespace CJMODAPI {

class CJMODScannerAPI {
public:
	static Arg scan(const Arg& pattern, const std::string& keyword, const std::string& source) {
		// 基础桥接：调用统一扫描器的双指针切分，然后按keyword对齐（简化）
		Scanner::CHTLUnifiedScanner s; s.setSource(source);
		auto r = s.scanCJMODByTwoPointers(0, source.size());
		// 简化：直接返回切分结果
		return Arg(r.tokens);
	}
};

} // namespace CJMODAPI