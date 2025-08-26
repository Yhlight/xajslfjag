#include "CHTLCompiler/CHTLCompiler.h"
#include <iostream>
#include <cassert>
int main() {
  std::cout << "CHTL基础测试..." << std::endl;
  CHTL::CHTLCompiler compiler;
  auto result = compiler.compile("<div>Hello CHTL</div>");
  assert(result.success);
  std::cout << "CHTL基础测试通过!" << std::endl;
  return 0;
}
