# ANTLR集成指南

## 概述

CHTL项目中的CSS和JavaScript编译器使用ANTLR 4作为解析器生成器。本指南说明如何集成和使用ANTLR。

## 安装ANTLR

### 1. 下载ANTLR
```bash
# 下载ANTLR完整jar包
wget https://www.antlr.org/download/antlr-4.13.1-complete.jar

# 设置CLASSPATH
export CLASSPATH=".:/path/to/antlr-4.13.1-complete.jar:$CLASSPATH"

# 创建别名
alias antlr4='java -jar /path/to/antlr-4.13.1-complete.jar'
alias grun='java org.antlr.v4.gui.TestRig'
```

### 2. 安装C++运行时
```bash
# Ubuntu/Debian
sudo apt-get install libantlr4-runtime-dev

# macOS
brew install antlr4-cpp-runtime

# 或者从源码编译
git clone https://github.com/antlr/antlr4.git
cd antlr4/runtime/Cpp
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
sudo make install
```

## 生成解析器

### 1. CSS解析器
```bash
cd /workspace/src/CSS
antlr4 -Dlanguage=Cpp -visitor -no-listener CSS.g4
```

这将生成：
- `CSSLexer.h/cpp`
- `CSSParser.h/cpp`
- `CSSVisitor.h/cpp`
- `CSSBaseVisitor.h/cpp`

### 2. JavaScript解析器
```bash
cd /workspace/src/JS
antlr4 -Dlanguage=Cpp -visitor -no-listener JavaScript.g4
```

## 使用生成的解析器

### CSS编译器实现
```cpp
#include "CSSCompiler.h"
#include "CSSLexer.h"
#include "CSSParser.h"
#include "CSSBaseVisitor.h"
#include <antlr4-runtime.h>

using namespace antlr4;

class CSSVisitorImpl : public CSSBaseVisitor {
public:
    antlrcpp::Any visitStylesheet(CSSParser::StylesheetContext *ctx) override {
        // 处理样式表
        for (auto rule : ctx->rule()) {
            visit(rule);
        }
        return nullptr;
    }
    
    antlrcpp::Any visitRule(CSSParser::RuleContext *ctx) override {
        // 处理CSS规则
        auto selectors = ctx->selectorList();
        auto declarations = ctx->declarationBlock();
        
        // 生成输出代码...
        return nullptr;
    }
};

CompileResult CSSCompilerImpl::compile(const std::string& code, const CompileOptions& options) {
    CompileResult result;
    
    try {
        // 创建ANTLR输入流
        ANTLRInputStream input(code);
        
        // 创建词法分析器
        CSSLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        
        // 创建语法分析器
        CSSParser parser(&tokens);
        
        // 错误处理
        parser.removeErrorListeners();
        parser.addErrorListener(new ThrowingErrorListener());
        
        // 解析
        auto tree = parser.stylesheet();
        
        // 使用访问者模式处理AST
        CSSVisitorImpl visitor;
        visitor.visit(tree);
        
        // 获取生成的代码
        result.cssOutput = visitor.getGeneratedCode();
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back(e.what());
    }
    
    return result;
}
```

### JavaScript编译器实现
```cpp
class JavaScriptVisitorImpl : public JavaScriptBaseVisitor {
public:
    antlrcpp::Any visitProgram(JavaScriptParser::ProgramContext *ctx) override {
        // 处理程序
        for (auto statement : ctx->sourceElement()) {
            visit(statement);
        }
        return nullptr;
    }
    
    antlrcpp::Any visitFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext *ctx) override {
        // 处理函数声明
        std::string funcName = ctx->Identifier()->getText();
        // 处理参数和函数体...
        return nullptr;
    }
};
```

## CMake配置

```cmake
# 查找ANTLR
find_package(ANTLR REQUIRED)

# 生成CSS解析器
antlr_target(CSSGrammar CSS/CSS.g4 
    VISITOR
    PACKAGE CHTL)

# 生成JavaScript解析器
antlr_target(JSGrammar JS/JavaScript.g4
    VISITOR
    PACKAGE CHTL)

# 添加到目标
target_sources(CHTLCore PRIVATE
    ${ANTLR_CSSGrammar_CXX_OUTPUTS}
    ${ANTLR_JSGrammar_CXX_OUTPUTS}
)

target_include_directories(CHTLCore PRIVATE
    ${ANTLR_CSSGrammar_OUTPUT_DIR}
    ${ANTLR_JSGrammar_OUTPUT_DIR}
)

target_link_libraries(CHTLCore PRIVATE antlr4-runtime)
```

## 错误处理

### 自定义错误监听器
```cpp
class ThrowingErrorListener : public BaseErrorListener {
public:
    void syntaxError(Recognizer *recognizer, Token *offendingSymbol,
                    size_t line, size_t charPositionInLine,
                    const std::string &msg, std::exception_ptr e) override {
        std::ostringstream ss;
        ss << "Syntax error at line " << line << ":" << charPositionInLine 
           << " - " << msg;
        throw std::runtime_error(ss.str());
    }
};
```

## 性能优化

### 1. 使用SLL预测模式
```cpp
parser.getInterpreter<atn::ParserATNSimulator>()->setPredictionMode(atn::PredictionMode::SLL);
try {
    tree = parser.stylesheet();
} catch (std::exception &e) {
    // 如果SLL失败，回退到LL
    tokens.reset();
    parser.reset();
    parser.getInterpreter<atn::ParserATNSimulator>()->setPredictionMode(atn::PredictionMode::LL);
    tree = parser.stylesheet();
}
```

### 2. 缓存解析结果
```cpp
class ParserCache {
    std::unordered_map<std::string, ParseTreePtr> cache_;
    
public:
    ParseTreePtr getParsedTree(const std::string& code) {
        auto hash = std::hash<std::string>{}(code);
        auto it = cache_.find(hash);
        if (it != cache_.end()) {
            return it->second;
        }
        
        // 解析并缓存
        auto tree = parseCode(code);
        cache_[hash] = tree;
        return tree;
    }
};
```

## 调试技巧

### 1. 打印解析树
```cpp
std::string tree = ctx->toStringTree(&parser);
std::cout << "Parse tree: " << tree << std::endl;
```

### 2. 使用GUI查看解析树
```bash
# 编译测试文件
antlr4 CSS.g4
javac -cp ".:antlr-4.13.1-complete.jar" *.java

# 运行GUI
grun CSS stylesheet -gui < test.css
```

### 3. 生成解析树图像
```bash
grun CSS stylesheet -ps test.css > tree.ps
```

## 完整示例

### test.css
```css
body {
    background: white;
    color: #333;
}

.container {
    width: 100%;
    max-width: 1200px;
}
```

### 解析流程
1. FragmentCollector收集所有CSS片段
2. 合并成完整的CSS代码
3. 传递给CSS编译器
4. ANTLR解析完整的样式表
5. 访问者遍历AST生成优化后的CSS

## 故障排除

### 常见问题

1. **找不到ANTLR运行时**
   ```bash
   export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
   ```

2. **解析性能慢**
   - 使用SLL预测模式
   - 减少回溯
   - 简化语法规则

3. **内存使用高**
   - 及时释放解析树
   - 使用流式处理
   - 限制输入大小

## 总结

通过正确集成ANTLR，CHTL项目能够：
1. 使用专业的解析器处理CSS和JavaScript
2. 获得准确的语法分析和错误报告
3. 生成优化的输出代码
4. 支持未来的语言扩展