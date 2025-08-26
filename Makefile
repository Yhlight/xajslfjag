CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I.
LDFLAGS = 

# Source directories
CHTL_DIR = CHTL
SRC_DIRS = $(CHTL_DIR)/CHTLLexer $(CHTL_DIR)/CHTLParser $(CHTL_DIR)/CHTLGenerator \
           $(CHTL_DIR)/CHTLNode $(CHTL_DIR)/CHTLState $(CHTL_DIR)/CHTLContext \
           $(CHTL_DIR)/CHTLLoader $(CHTL_DIR)/CHTLManage

# Source files
SOURCES = $(CHTL_DIR)/CHTL.cpp \
          $(CHTL_DIR)/CHTLLexer/CHTLLexer.cpp \
          $(CHTL_DIR)/CHTLParser/CHTLParser.cpp \
          $(CHTL_DIR)/CHTLGenerator/CHTLGenerator.cpp \
          $(CHTL_DIR)/CHTLNode/CHTLNode.cpp \
          $(CHTL_DIR)/CHTLState/CHTLState.cpp \
          $(CHTL_DIR)/CHTLContext/CHTLContext.cpp \
          $(CHTL_DIR)/CHTLContext/GlobalMap.cpp \
          $(CHTL_DIR)/CHTLLoader/CHTLLoader.cpp \
          $(CHTL_DIR)/CHTLManage/CHTLManager.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Test program
TEST_SOURCES = test_chtl.cpp
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)

# Targets
TARGET = libchtl.a
TEST_TARGET = test_chtl

.PHONY: all clean test install

all: $(TARGET) $(TEST_TARGET)

# Build static library
$(TARGET): $(OBJECTS)
	ar rcs $@ $^
	@echo "Built CHTL static library: $(TARGET)"

# Build test program
$(TEST_TARGET): $(TEST_OBJECTS) $(TARGET)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built test program: $(TEST_TARGET)"

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run tests
test: $(TEST_TARGET)
	@echo "Running CHTL tests..."
	./$(TEST_TARGET)

# Clean build files
clean:
	rm -f $(OBJECTS) $(TEST_OBJECTS) $(TARGET) $(TEST_TARGET)
	rm -f test_output.html test_output.css
	@echo "Cleaned build files"

# Install headers and library (optional)
install: $(TARGET)
	mkdir -p /usr/local/include/chtl
	mkdir -p /usr/local/lib
	cp -r $(CHTL_DIR)/*.h /usr/local/include/chtl/
	cp -r $(CHTL_DIR)/*/*.h /usr/local/include/chtl/
	cp $(TARGET) /usr/local/lib/
	@echo "Installed CHTL library and headers"

# Show project structure
structure:
	@echo "CHTL Project Structure:"
	@find $(CHTL_DIR) -name "*.h" -o -name "*.cpp" | sort

# Development help
help:
	@echo "CHTL Compiler Build System"
	@echo "=========================="
	@echo "Targets:"
	@echo "  all       - Build library and test program"
	@echo "  test      - Build and run tests"
	@echo "  clean     - Remove build files"
	@echo "  install   - Install library and headers"
	@echo "  structure - Show project file structure"
	@echo "  help      - Show this help"