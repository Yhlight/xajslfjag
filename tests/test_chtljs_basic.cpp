#include "CHTLJSCompiler/CHTLJSCompiler.h"
#include <iostream>
#include <cassert>
int main() {
  std::cout << "CHTL JS基础测试..." << std::endl;
  CHTLJS::CHTLJSCompiler compiler;
  auto result = compiler.compile("const x = 42;");
  std::cout << "CHTL JS基础测试通过!" << std::endl;
  return 0;
}
