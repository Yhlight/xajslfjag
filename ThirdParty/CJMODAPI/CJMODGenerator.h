#pragma once

#include <string>
#include <iostream>
#include "Arg.h"

namespace CJMODAPI {

class CJMODGenerator {
public:
	static void exportResult(const Arg& arg) {
		std::cout << arg.transformed() << std::endl;
	}
};

} // namespace CJMODAPI