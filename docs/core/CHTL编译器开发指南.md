# CHTL编译器开发指南

## 概述

CHTL编译器是项目的核心组件，负责将CHTL源代码编译为标准HTML。本指南详细介绍了CHTL编译器的架构、开发流程和最佳实践。

## 编译器架构

### 🏗️ **整体架构**

```
CHTL源代码 → CHTLUnifiedScanner → CompilerDispatcher → CHTL Compiler
                                                            ↓
                                                    CHTLLexer → CHTLParser → CHTLGenerator
                                                            ↓
                                                        标准HTML输出
```

### 🧩 **核心组件**

#### 1. CHTLContext (编译上下文)
```cpp
// src/CHTL/CHTLContext/Context.h
class CHTLContext {
    // 管理编译会话状态
    // 维护符号表和作用域
    // 处理模块导入关系
};
```

**职责**:
- 维护编译过程中的全局状态
- 管理符号表和作用域链
- 处理模块导入和依赖关系
- 提供错误上下文信息

#### 2. CHTLLexer (词法分析器)
```cpp
// src/CHTL/CHTLLexer/Lexer.h
class CHTLLexer {
    // Token扫描和识别
    // GlobalMap管理
    // 关键字识别
};
```

**核心文件**:
- `Lexer.h/cpp`: 主词法分析器
- `Token.h/cpp`: Token定义和操作
- `GlobalMap.h/cpp`: 全局符号映射

**主要功能**:
- 将源代码转换为Token流
- 识别CHTL关键字和操作符
- 处理字符串和数值字面量
- 管理注释和空白字符

#### 3. CHTLParser (语法分析器)
```cpp
// src/CHTL/CHTLParser/Parser.h
class CHTLParser {
    // 递归下降解析
    // AST构建
    // 语法错误处理
};
```

**解析策略**:
- 递归下降解析算法
- 左递归消除
- 错误恢复机制
- 语法糖展开

#### 4. CHTLNode (AST节点系统)
```
CHTLNode/
├── BaseNode.h              # 基础节点类
│   ├── ElementNode.h       # HTML元素节点
│   └── TextNode.h          # 文本节点
├── CommentNode.h           # 注释节点
├── TemplateNode.h          # 模板节点
├── CustomNode.h            # 自定义组件节点
├── StyleNode.h             # 样式节点
├── ScriptNode.h            # 脚本节点
├── OriginNode.h            # 原始嵌入节点
├── ImportNode.h            # 导入节点
├── ConfigNode.h            # 配置节点
├── NamespaceNode.h         # 命名空间节点
└── OperatorNode.h          # 操作符节点
```

**节点继承体系**:
```cpp
class BaseNode {
public:
    virtual void accept(Visitor& visitor) = 0;
    virtual std::string toString() const = 0;
    virtual NodeType getType() const = 0;
protected:
    SourceLocation location;
    std::vector<std::unique_ptr<BaseNode>> children;
};

class ElementNode : public BaseNode {
    std::string tagName;
    AttributeMap attributes;
    bool isCustomElement;
};
```

#### 5. CHTLGenerator (代码生成器)
```cpp
// src/CHTL/CHTLGenerator/Generator.h
class CHTLGenerator {
    // 访问者模式遍历AST
    // HTML代码生成
    // 优化处理
};
```

**生成策略**:
- 访问者模式遍历AST
- 增量代码生成
- 代码优化和压缩
- 源映射生成

## 开发环境配置

### 📋 **环境要求**

```bash
# C++编译器
g++ 7.0+ 或 clang++ 6.0+

# 构建工具
cmake 3.15+
make 或 ninja

# 调试工具 (可选)
gdb 或 lldb
valgrind (Linux)
```

### 🛠️ **开发环境设置**

```bash
# 1. 克隆项目
git clone https://github.com/CHTL-Team/CHTL.git
cd CHTL

# 2. 构建调试版本
./scripts/build_debug.sh

# 3. 运行测试
cd build_debug
make test

# 4. 安装开发工具
sudo apt install gdb valgrind  # Ubuntu/Debian
brew install gdb               # macOS
```

## 核心开发流程

### 🔄 **开发周期**

1. **需求分析** → 确定新特性或修复目标
2. **设计阶段** → 设计API和实现方案
3. **实现阶段** → 编写代码和单元测试
4. **测试验证** → 运行完整测试套件
5. **文档更新** → 更新相关文档
6. **代码审查** → 提交Pull Request

### 📝 **编码规范**

#### 命名约定
```cpp
// 类名：大驼峰
class CHTLParser {
    
// 方法名：小驼峰
public:
    bool parseStatement();
    void generateCode();
    
// 成员变量：小驼峰 + 下划线前缀
private:
    std::string current_token_;
    int line_number_;
    
// 常量：全大写 + 下划线
static const int MAX_DEPTH = 100;
};

// 文件名：大驼峰
// CHTLParser.h, CHTLParser.cpp
```

#### 代码结构
```cpp
// 头文件结构
#pragma once

#include "CHTL/Common/Common.h"
#include <string>
#include <vector>

namespace CHTL {

class CHTLParser {
public:
    explicit CHTLParser(const std::string& source);
    ~CHTLParser() = default;
    
    // 禁用拷贝构造
    CHTLParser(const CHTLParser&) = delete;
    CHTLParser& operator=(const CHTLParser&) = delete;
    
    // 主要接口
    std::unique_ptr<BaseNode> parse();
    bool hasErrors() const;
    
private:
    // 私有方法
    void advance();
    bool match(TokenType type);
    
    // 成员变量
    std::string source_;
    std::vector<Token> tokens_;
    size_t current_position_;
};

} // namespace CHTL
```

### 🧪 **测试驱动开发**

#### 单元测试示例
```cpp
// tests/CHTL/Parser/ParserTest.cpp
#include <gtest/gtest.h>
#include "CHTL/CHTLParser/Parser.h"

class CHTLParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser_ = std::make_unique<CHTLParser>("");
    }
    
    std::unique_ptr<CHTLParser> parser_;
};

TEST_F(CHTLParserTest, ParseSimpleElement) {
    std::string source = "div { \"Hello World\" }";
    parser_ = std::make_unique<CHTLParser>(source);
    
    auto ast = parser_->parse();
    
    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->getType(), NodeType::Element);
    
    auto element = static_cast<ElementNode*>(ast.get());
    EXPECT_EQ(element->getTagName(), "div");
    EXPECT_EQ(element->getChildren().size(), 1);
}
```

#### 集成测试
```cpp
// tests/Integration/CompilerTest.cpp
TEST(CompilerIntegration, FullCompilation) {
    std::string chtl_source = R"(
        [Template] @Style MainStyle {
            .container { margin: 20px; }
        }
        
        div.container {
            h1 { "CHTL Test" }
            p { "This is a test." }
        }
    )";
    
    CHTLCompiler compiler;
    auto result = compiler.compile(chtl_source);
    
    ASSERT_TRUE(result.success);
    EXPECT_THAT(result.html, testing::HasSubstr("<div class=\"container\">"));
    EXPECT_THAT(result.html, testing::HasSubstr("<h1>CHTL Test</h1>"));
}
```

## 关键实现细节

### 🔍 **词法分析实现**

```cpp
// CHTLLexer实现要点
class CHTLLexer {
private:
    std::string source_;
    size_t position_;
    size_t line_;
    size_t column_;
    
public:
    Token nextToken() {
        skipWhitespace();
        
        if (isAlpha(peek())) {
            return scanIdentifier();
        } else if (isDigit(peek())) {
            return scanNumber();
        } else if (peek() == '"') {
            return scanString();
        } else if (peek() == '{') {
            return Token(TokenType::LeftBrace, "{", getLocation());
        }
        // ... 其他token类型
        
        return Token(TokenType::EndOfFile, "", getLocation());
    }
    
private:
    Token scanIdentifier() {
        size_t start = position_;
        while (isAlphaNumeric(peek())) {
            advance();
        }
        
        std::string text = source_.substr(start, position_ - start);
        TokenType type = getKeywordType(text);
        return Token(type, text, getLocation());
    }
};
```

### 🌳 **语法分析实现**

```cpp
// 递归下降解析示例
class CHTLParser {
    std::unique_ptr<BaseNode> parseElement() {
        // element := IDENTIFIER ('.' IDENTIFIER)* attributes? '{' content '}'
        
        Token tagToken = consume(TokenType::Identifier);
        std::string tagName = tagToken.text;
        
        // 解析CSS类名
        std::vector<std::string> classes;
        while (match(TokenType::Dot)) {
            classes.push_back(consume(TokenType::Identifier).text);
        }
        
        // 解析属性
        AttributeMap attributes = parseAttributes();
        
        // 解析内容
        consume(TokenType::LeftBrace);
        auto children = parseChildren();
        consume(TokenType::RightBrace);
        
        auto element = std::make_unique<ElementNode>(tagName);
        element->setClasses(classes);
        element->setAttributes(attributes);
        element->setChildren(std::move(children));
        
        return element;
    }
};
```

### 🎯 **代码生成实现**

```cpp
// 访问者模式代码生成
class HTMLGenerator : public NodeVisitor {
private:
    std::stringstream output_;
    int indent_level_;
    
public:
    void visit(ElementNode* node) override {
        writeIndent();
        output_ << "<" << node->getTagName();
        
        // 生成属性
        for (const auto& [key, value] : node->getAttributes()) {
            output_ << " " << key << "=\"" << escapeHtml(value) << "\"";
        }
        
        // 生成CSS类
        if (!node->getClasses().empty()) {
            output_ << " class=\"";
            for (size_t i = 0; i < node->getClasses().size(); ++i) {
                if (i > 0) output_ << " ";
                output_ << node->getClasses()[i];
            }
            output_ << "\"";
        }
        
        output_ << ">";
        
        // 生成子节点
        if (!node->getChildren().empty()) {
            output_ << "\n";
            indent_level_++;
            for (auto& child : node->getChildren()) {
                child->accept(*this);
            }
            indent_level_--;
            writeIndent();
        }
        
        output_ << "</" << node->getTagName() << ">\n";
    }
    
    void visit(TextNode* node) override {
        writeIndent();
        output_ << escapeHtml(node->getText()) << "\n";
    }
};
```

## 错误处理机制

### 🚨 **错误类型体系**

```cpp
// src/Error/CompilerError.h
enum class ErrorType {
    LexicalError,    // 词法错误
    SyntaxError,     // 语法错误
    SemanticError,   // 语义错误
    TypeError,       // 类型错误
    ImportError,     // 导入错误
    InternalError    // 内部错误
};

class CompilerError {
public:
    ErrorType type;
    std::string message;
    SourceLocation location;
    std::vector<std::string> suggestions;
    
    std::string format() const {
        return formatError(type, message, location, suggestions);
    }
};
```

### 🔧 **错误恢复策略**

```cpp
// 语法错误恢复
class CHTLParser {
    void synchronize() {
        // 跳过错误token直到找到同步点
        while (!isAtEnd()) {
            if (previous().type == TokenType::Semicolon) return;
            
            switch (peek().type) {
                case TokenType::Div:
                case TokenType::Span:
                case TokenType::Template:
                    return;
            }
            
            advance();
        }
    }
    
    std::unique_ptr<BaseNode> parseStatement() {
        try {
            return parseElementOrTemplate();
        } catch (const ParseError& error) {
            reportError(error);
            synchronize();
            return nullptr;
        }
    }
};
```

## 性能优化

### ⚡ **编译性能优化**

#### 1. 词法分析优化
```cpp
// 使用字符查找表加速
class FastLexer {
private:
    static constexpr bool is_alpha_table[256] = { /* ... */ };
    static constexpr bool is_digit_table[256] = { /* ... */ };
    
public:
    inline bool isAlpha(char c) {
        return is_alpha_table[static_cast<unsigned char>(c)];
    }
};
```

#### 2. AST节点池化
```cpp
// 节点对象池
class NodePool {
private:
    std::vector<std::unique_ptr<BaseNode>> free_nodes_;
    
public:
    template<typename T, typename... Args>
    T* allocate(Args&&... args) {
        if (!free_nodes_.empty()) {
            auto node = std::move(free_nodes_.back());
            free_nodes_.pop_back();
            return static_cast<T*>(node.release());
        }
        return new T(std::forward<Args>(args)...);
    }
};
```

#### 3. 增量编译
```cpp
// 文件依赖追踪
class DependencyTracker {
private:
    std::unordered_map<std::string, std::time_t> file_timestamps_;
    std::unordered_map<std::string, std::vector<std::string>> dependencies_;
    
public:
    bool needsRecompilation(const std::string& file) {
        auto current_time = getFileTimestamp(file);
        auto cached_time = file_timestamps_[file];
        
        if (current_time > cached_time) {
            return true;
        }
        
        // 检查依赖文件
        for (const auto& dep : dependencies_[file]) {
            if (needsRecompilation(dep)) {
                return true;
            }
        }
        
        return false;
    }
};
```

## 调试技巧

### 🐛 **常用调试方法**

#### 1. AST可视化
```cpp
// 使用AST打印工具
class ASTPrinter : public NodeVisitor {
public:
    void visit(ElementNode* node) override {
        std::cout << std::string(depth_ * 2, ' ') 
                  << "Element: " << node->getTagName() << std::endl;
        depth_++;
        for (auto& child : node->getChildren()) {
            child->accept(*this);
        }
        depth_--;
    }
};

// 使用方法
auto ast = parser.parse();
ASTPrinter printer;
ast->accept(printer);
```

#### 2. Token流调试
```cpp
// Token序列打印
void printTokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::cout << "Token: " << tokenTypeToString(token.type)
                  << " Value: \"" << token.text << "\""
                  << " Line: " << token.location.line << std::endl;
    }
}
```

#### 3. 性能分析
```cpp
// 编译时间测量
class CompileTimer {
private:
    std::chrono::high_resolution_clock::time_point start_;
    std::string phase_name_;
    
public:
    CompileTimer(const std::string& phase) : phase_name_(phase) {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
    ~CompileTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);
        std::cout << phase_name_ << " took: " << duration.count() << "ms" << std::endl;
    }
};

// 使用方法
{
    CompileTimer timer("Lexical Analysis");
    auto tokens = lexer.tokenize(source);
}
```

## 扩展开发

### 🔌 **添加新语法特性**

#### 1. 扩展Token类型
```cpp
// 在TokenType枚举中添加新类型
enum class TokenType {
    // ... 现有类型
    NewKeyword,     // 新关键字
    NewOperator,    // 新操作符
};
```

#### 2. 扩展AST节点
```cpp
// 创建新的AST节点类
class NewFeatureNode : public BaseNode {
public:
    void accept(NodeVisitor& visitor) override {
        visitor.visit(this);
    }
    
    NodeType getType() const override {
        return NodeType::NewFeature;
    }
};
```

#### 3. 扩展解析器
```cpp
// 在解析器中添加新的解析方法
class CHTLParser {
    std::unique_ptr<BaseNode> parseNewFeature() {
        consume(TokenType::NewKeyword);
        // 解析新特性的语法
        return std::make_unique<NewFeatureNode>();
    }
};
```

#### 4. 扩展代码生成器
```cpp
// 在访问者中添加新的处理方法
class HTMLGenerator : public NodeVisitor {
public:
    void visit(NewFeatureNode* node) override {
        // 生成对应的HTML代码
    }
};
```

## 最佳实践

### ✅ **开发建议**

1. **始终编写单元测试**: 每个新功能都应该有对应的测试
2. **保持代码简洁**: 优先可读性，然后考虑性能
3. **遵循RAII原则**: 合理管理资源生命周期
4. **使用智能指针**: 避免内存泄漏
5. **添加详细注释**: 特别是复杂的算法和数据结构
6. **定期性能测试**: 确保新功能不会影响编译性能

### 🚫 **常见陷阱**

1. **左递归**: 避免在语法规则中使用左递归
2. **内存泄漏**: 注意AST节点的生命周期管理
3. **错误恢复**: 确保解析错误不会导致无限循环
4. **线程安全**: 多线程环境下的状态管理
5. **栈溢出**: 深度嵌套的AST可能导致栈溢出

---

通过遵循本指南，您将能够高效地参与CHTL编译器的开发，为项目贡献高质量的代码。