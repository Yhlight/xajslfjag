CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O3 -DNDEBUG

# 源文件
SOURCES = CHTLLexer/CHTLToken.cpp \
          CHTLState/CHTLState.cpp \
          CHTLManage/CHTLGlobalMap.cpp \
          CHTLContext/CHTLContext.cpp

TEST_SOURCES = test_basic.cpp
TEST2_SOURCES = test_state_context.cpp

# 目标文件
OBJECTS = $(SOURCES:.cpp=.o)
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)
TEST2_OBJECTS = $(TEST2_SOURCES:.cpp=.o)

# 可执行文件
TEST_EXEC = test_basic
TEST2_EXEC = test_state_context

# 默认目标
all: $(TEST_EXEC) $(TEST2_EXEC)

# 测试程序
$(TEST_EXEC): $(OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -o $@ $^

$(TEST2_EXEC): $(OBJECTS) $(TEST2_OBJECTS)
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -o $@ $^

# 编译规则
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -c -o $@ $<

# 运行测试
test: $(TEST_EXEC) $(TEST2_EXEC)
	@echo "Running basic tests..."
	./$(TEST_EXEC)
	@echo "\nRunning state and context tests..."
	./$(TEST2_EXEC)

test-basic: $(TEST_EXEC)
	./$(TEST_EXEC)

test-state: $(TEST2_EXEC)
	./$(TEST2_EXEC)

# 清理
clean:
	rm -f $(OBJECTS) $(TEST_OBJECTS) $(TEST2_OBJECTS) $(TEST_EXEC) $(TEST2_EXEC)
	find . -name "*.o" -delete

# 依赖关系
CHTLLexer/CHTLToken.o: CHTLLexer/CHTLToken.h
CHTLState/CHTLState.o: CHTLState/CHTLState.h
CHTLManage/CHTLGlobalMap.o: CHTLManage/CHTLGlobalMap.h
CHTLContext/CHTLContext.o: CHTLContext/CHTLContext.h CHTLState/CHTLState.h CHTLManage/CHTLGlobalMap.h
test_basic.o: CHTLLexer/CHTLToken.h
test_state_context.o: CHTLState/CHTLState.h CHTLContext/CHTLContext.h CHTLManage/CHTLGlobalMap.h

.PHONY: all test test-basic test-state clean