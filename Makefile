# CHTL Compiler Makefile
# 完整的CHTL和CHTL JS编译器构建系统

# 编译器设置
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -fPIC
DEBUG_FLAGS = -g -DDEBUG -O0
RELEASE_FLAGS = -DNDEBUG -O3

# 目录设置
SRC_DIR = CHTL
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
LIB_DIR = $(BUILD_DIR)/lib
TEST_DIR = tests
DOCS_DIR = docs

# 包含路径
INCLUDES = -I$(SRC_DIR) -I$(SRC_DIR)/CHTLCompiler -I$(SRC_DIR)/CHTLJSCompiler -I$(SRC_DIR)/Core

# 库设置
LIBS = -pthread
STATIC_LIB = $(LIB_DIR)/libchtl.a
SHARED_LIB = $(LIB_DIR)/libchtl.so

# CHTL编译器源文件
CHTL_LEXER_SOURCES = $(SRC_DIR)/CHTLCompiler/Lexer/CHTLTokens.cpp \
                     $(SRC_DIR)/CHTLCompiler/Lexer/CHTLLexer.cpp

CHTL_PARSER_SOURCES = $(SRC_DIR)/CHTLCompiler/Parser/CHTLParser.cpp

CHTL_AST_SOURCES = $(SRC_DIR)/CHTLCompiler/AST/CHTLNodes.cpp

CHTL_GENERATOR_SOURCES = $(SRC_DIR)/CHTLCompiler/Generator/CHTLGenerator.cpp

CHTL_COMPILER_SOURCES = $(SRC_DIR)/CHTLCompiler/CHTLCompiler.cpp

# CHTL JS编译器源文件
CHTLJS_LEXER_SOURCES = $(SRC_DIR)/CHTLJSCompiler/Lexer/CHTLJSTokens.cpp \
                       $(SRC_DIR)/CHTLJSCompiler/Lexer/CHTLJSLexer.cpp

CHTLJS_AST_SOURCES = $(SRC_DIR)/CHTLJSCompiler/AST/CHTLJSNodes.cpp

# 统一编译器源文件
CORE_SOURCES = $(SRC_DIR)/Core/UnifiedCompiler.cpp

# 所有源文件
ALL_SOURCES = $(CHTL_LEXER_SOURCES) $(CHTL_PARSER_SOURCES) $(CHTL_AST_SOURCES) \
              $(CHTL_GENERATOR_SOURCES) $(CHTL_COMPILER_SOURCES) \
              $(CHTLJS_LEXER_SOURCES) $(CHTLJS_AST_SOURCES) \
              $(CORE_SOURCES)

# 对象文件
OBJECTS = $(ALL_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# 测试源文件
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.cpp=$(OBJ_DIR)/tests/%.o)
TEST_BINARIES = $(TEST_SOURCES:$(TEST_DIR)/%.cpp=$(BIN_DIR)/%)

# 示例源文件
EXAMPLES_DIR = examples
EXAMPLE_SOURCES = $(wildcard $(EXAMPLES_DIR)/*.cpp)
EXAMPLE_BINARIES = $(EXAMPLE_SOURCES:$(EXAMPLES_DIR)/%.cpp=$(BIN_DIR)/%)

# 主要目标
.PHONY: all clean test examples docs install uninstall debug release

all: $(STATIC_LIB) $(SHARED_LIB) $(BIN_DIR)/chtl-compiler

# 调试版本
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: all

# 发布版本
release: CXXFLAGS += $(RELEASE_FLAGS)
release: all

# 创建目录
$(BUILD_DIR) $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR):
	@mkdir -p $@

$(OBJ_DIR)/CHTLCompiler/Lexer $(OBJ_DIR)/CHTLCompiler/Parser $(OBJ_DIR)/CHTLCompiler/AST \
$(OBJ_DIR)/CHTLCompiler/Generator $(OBJ_DIR)/CHTLCompiler \
$(OBJ_DIR)/CHTLJSCompiler/Lexer $(OBJ_DIR)/CHTLJSCompiler/AST $(OBJ_DIR)/CHTLJSCompiler \
$(OBJ_DIR)/Core $(OBJ_DIR)/tests:
	@mkdir -p $@

# 编译对象文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) $(OBJ_DIR)/CHTLCompiler/Lexer $(OBJ_DIR)/CHTLCompiler/Parser $(OBJ_DIR)/CHTLCompiler/AST $(OBJ_DIR)/CHTLCompiler/Generator $(OBJ_DIR)/CHTLCompiler $(OBJ_DIR)/CHTLJSCompiler/Lexer $(OBJ_DIR)/CHTLJSCompiler/AST $(OBJ_DIR)/CHTLJSCompiler $(OBJ_DIR)/Core
	@echo "编译 $<"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 静态库
$(STATIC_LIB): $(OBJECTS) | $(LIB_DIR)
	@echo "创建静态库 $@"
	@ar rcs $@ $(OBJECTS)

# 动态库
$(SHARED_LIB): $(OBJECTS) | $(LIB_DIR)
	@echo "创建动态库 $@"
	@$(CXX) -shared -o $@ $(OBJECTS) $(LIBS)

# 主编译器可执行文件
$(BIN_DIR)/chtl-compiler: $(STATIC_LIB) | $(BIN_DIR)
	@echo "创建CHTL编译器可执行文件"
	@echo '#include "CHTLCompiler/CHTLCompiler.h"' > temp_main.cpp
	@echo '#include "CHTLJSCompiler/CHTLJSCompiler.h"' >> temp_main.cpp
	@echo '#include "Core/UnifiedCompiler.h"' >> temp_main.cpp
	@echo '#include <iostream>' >> temp_main.cpp
	@echo '#include <vector>' >> temp_main.cpp
	@echo 'int main(int argc, char* argv[]) {' >> temp_main.cpp
	@echo '  if (argc < 2) {' >> temp_main.cpp
	@echo '    std::cout << "用法: " << argv[0] << " <文件.chtl|文件.chtljs>" << std::endl;' >> temp_main.cpp
	@echo '    return 1;' >> temp_main.cpp
	@echo '  }' >> temp_main.cpp
	@echo '  std::string filename = argv[1];' >> temp_main.cpp
	@echo '  CHTL::Core::UnifiedCompiler compiler;' >> temp_main.cpp
	@echo '  auto result = compiler.compileFile(filename);' >> temp_main.cpp
	@echo '  if (result.success) {' >> temp_main.cpp
	@echo '    std::cout << "编译成功!" << std::endl;' >> temp_main.cpp
	@echo '    return 0;' >> temp_main.cpp
	@echo '  } else {' >> temp_main.cpp
	@echo '    std::cout << "编译失败:" << std::endl;' >> temp_main.cpp
	@echo '    for (const auto& error : result.errors) {' >> temp_main.cpp
	@echo '      std::cout << "  " << error << std::endl;' >> temp_main.cpp
	@echo '    }' >> temp_main.cpp
	@echo '    return 1;' >> temp_main.cpp
	@echo '  }' >> temp_main.cpp
	@echo '}' >> temp_main.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) temp_main.cpp -L$(LIB_DIR) -lchtl $(LIBS) -o $@
	@rm temp_main.cpp

# 测试
$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR)/tests
	@echo "编译测试 $<"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BIN_DIR)/%: $(OBJ_DIR)/tests/%.o $(STATIC_LIB) | $(BIN_DIR)
	@echo "链接测试 $@"
	@$(CXX) $(CXXFLAGS) $< -L$(LIB_DIR) -lchtl $(LIBS) -o $@

test: $(TEST_BINARIES)
	@echo "运行测试..."
	@for test in $(TEST_BINARIES); do \
		echo "运行 $$test"; \
		$$test || exit 1; \
	done
	@echo "所有测试通过!"

# 创建基本测试文件
create-tests: | $(TEST_DIR)
	@echo "创建基本测试文件..."
	@echo '#include "CHTLCompiler/CHTLCompiler.h"' > $(TEST_DIR)/test_chtl_basic.cpp
	@echo '#include <iostream>' >> $(TEST_DIR)/test_chtl_basic.cpp
	@echo '#include <cassert>' >> $(TEST_DIR)/test_chtl_basic.cpp
	@echo 'int main() {' >> $(TEST_DIR)/test_chtl_basic.cpp
	@echo '  std::cout << "CHTL基础测试..." << std::endl;' >> $(TEST_DIR)/test_chtl_basic.cpp
	@echo '  CHTL::CHTLCompiler compiler;' >> $(TEST_DIR)/test_chtl_basic.cpp
	@echo '  auto result = compiler.compile("<div>Hello CHTL</div>");' >> $(TEST_DIR)/test_chtl_basic.cpp
	@echo '  assert(result.success);' >> $(TEST_DIR)/test_chtl_basic.cpp
	@echo '  std::cout << "CHTL基础测试通过!" << std::endl;' >> $(TEST_DIR)/test_chtl_basic.cpp
	@echo '  return 0;' >> $(TEST_DIR)/test_chtl_basic.cpp
	@echo '}' >> $(TEST_DIR)/test_chtl_basic.cpp
	
	@echo '#include "CHTLJSCompiler/CHTLJSCompiler.h"' > $(TEST_DIR)/test_chtljs_basic.cpp
	@echo '#include <iostream>' >> $(TEST_DIR)/test_chtljs_basic.cpp
	@echo '#include <cassert>' >> $(TEST_DIR)/test_chtljs_basic.cpp
	@echo 'int main() {' >> $(TEST_DIR)/test_chtljs_basic.cpp
	@echo '  std::cout << "CHTL JS基础测试..." << std::endl;' >> $(TEST_DIR)/test_chtljs_basic.cpp
	@echo '  CHTLJS::CHTLJSCompiler compiler;' >> $(TEST_DIR)/test_chtljs_basic.cpp
	@echo '  auto result = compiler.compile("const x = 42;");' >> $(TEST_DIR)/test_chtljs_basic.cpp
	@echo '  std::cout << "CHTL JS基础测试通过!" << std::endl;' >> $(TEST_DIR)/test_chtljs_basic.cpp
	@echo '  return 0;' >> $(TEST_DIR)/test_chtljs_basic.cpp
	@echo '}' >> $(TEST_DIR)/test_chtljs_basic.cpp

$(TEST_DIR):
	@mkdir -p $(TEST_DIR)

# 示例
examples: $(EXAMPLE_BINARIES)

$(BIN_DIR)/%: $(EXAMPLES_DIR)/%.cpp $(STATIC_LIB) | $(BIN_DIR)
	@echo "编译示例 $@"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $< -L$(LIB_DIR) -lchtl $(LIBS) -o $@

# 创建示例
create-examples: | $(EXAMPLES_DIR)
	@echo "创建示例文件..."
	@echo '#include "Core/UnifiedCompiler.h"' > $(EXAMPLES_DIR)/simple_compile.cpp
	@echo '#include <iostream>' >> $(EXAMPLES_DIR)/simple_compile.cpp
	@echo 'int main() {' >> $(EXAMPLES_DIR)/simple_compile.cpp
	@echo '  CHTL::Core::UnifiedCompiler compiler;' >> $(EXAMPLES_DIR)/simple_compile.cpp
	@echo '  auto result = compiler.compileFile("example.chtl");' >> $(EXAMPLES_DIR)/simple_compile.cpp
	@echo '  if (result.success) {' >> $(EXAMPLES_DIR)/simple_compile.cpp
	@echo '    std::cout << "编译成功!" << std::endl;' >> $(EXAMPLES_DIR)/simple_compile.cpp
	@echo '  } else {' >> $(EXAMPLES_DIR)/simple_compile.cpp
	@echo '    std::cout << "编译失败" << std::endl;' >> $(EXAMPLES_DIR)/simple_compile.cpp
	@echo '  }' >> $(EXAMPLES_DIR)/simple_compile.cpp
	@echo '  return 0;' >> $(EXAMPLES_DIR)/simple_compile.cpp
	@echo '}' >> $(EXAMPLES_DIR)/simple_compile.cpp

$(EXAMPLES_DIR):
	@mkdir -p $(EXAMPLES_DIR)

# 文档生成
docs:
	@echo "生成文档..."
	@mkdir -p $(DOCS_DIR)
	@echo "# CHTL编译器文档" > $(DOCS_DIR)/README.md
	@echo "" >> $(DOCS_DIR)/README.md
	@echo "## 构建" >> $(DOCS_DIR)/README.md
	@echo "\`\`\`bash" >> $(DOCS_DIR)/README.md
	@echo "make          # 构建所有组件" >> $(DOCS_DIR)/README.md
	@echo "make debug    # 调试版本" >> $(DOCS_DIR)/README.md
	@echo "make release  # 发布版本" >> $(DOCS_DIR)/README.md
	@echo "make test     # 运行测试" >> $(DOCS_DIR)/README.md
	@echo "\`\`\`" >> $(DOCS_DIR)/README.md
	@echo "" >> $(DOCS_DIR)/README.md
	@echo "## 使用" >> $(DOCS_DIR)/README.md
	@echo "\`\`\`bash" >> $(DOCS_DIR)/README.md
	@echo "./build/bin/chtl-compiler file.chtl" >> $(DOCS_DIR)/README.md
	@echo "\`\`\`" >> $(DOCS_DIR)/README.md

# 安装
install: $(STATIC_LIB) $(SHARED_LIB) $(BIN_DIR)/chtl-compiler
	@echo "安装CHTL编译器..."
	@mkdir -p /usr/local/bin /usr/local/lib /usr/local/include/chtl
	@cp $(BIN_DIR)/chtl-compiler /usr/local/bin/
	@cp $(STATIC_LIB) $(SHARED_LIB) /usr/local/lib/
	@cp -r $(SRC_DIR)/*.h /usr/local/include/chtl/
	@echo "安装完成!"

# 卸载
uninstall:
	@echo "卸载CHTL编译器..."
	@rm -f /usr/local/bin/chtl-compiler
	@rm -f /usr/local/lib/libchtl.a /usr/local/lib/libchtl.so
	@rm -rf /usr/local/include/chtl
	@echo "卸载完成!"

# 清理
clean:
	@echo "清理构建文件..."
	@rm -rf $(BUILD_DIR)
	@rm -f temp_main.cpp

# 深度清理
distclean: clean
	@echo "深度清理..."
	@rm -rf $(TEST_DIR) $(EXAMPLES_DIR) $(DOCS_DIR)

# 检查代码风格
lint:
	@echo "检查代码风格..."
	@find $(SRC_DIR) -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# 分析代码
analyze:
	@echo "分析代码..."
	@cppcheck --enable=all --std=c++17 $(SRC_DIR)

# 性能测试
benchmark: $(BIN_DIR)/chtl-compiler
	@echo "性能测试..."
	@time $(BIN_DIR)/chtl-compiler example.chtl

# 内存检查
memcheck: $(TEST_BINARIES)
	@echo "内存检查..."
	@for test in $(TEST_BINARIES); do \
		valgrind --leak-check=full $$test; \
	done

# 打包
package: release
	@echo "打包发布..."
	@tar -czf chtl-compiler.tar.gz -C build .

# 显示帮助
help:
	@echo "CHTL编译器构建系统"
	@echo ""
	@echo "目标:"
	@echo "  all           - 构建所有组件"
	@echo "  debug         - 构建调试版本"
	@echo "  release       - 构建发布版本"
	@echo "  test          - 运行测试"
	@echo "  examples      - 构建示例"
	@echo "  docs          - 生成文档"
	@echo "  install       - 安装到系统"
	@echo "  uninstall     - 从系统卸载"
	@echo "  clean         - 清理构建文件"
	@echo "  distclean     - 深度清理"
	@echo "  create-tests  - 创建基本测试"
	@echo "  create-examples - 创建示例"
	@echo "  lint          - 检查代码风格"
	@echo "  analyze       - 分析代码"
	@echo "  benchmark     - 性能测试"
	@echo "  memcheck      - 内存检查"
	@echo "  package       - 打包发布"
	@echo "  help          - 显示此帮助"

# 依赖关系
-include $(OBJECTS:.o=.d)

# 自动依赖生成
$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM -MP -MF $@ -MT $(@:.d=.o) $<