# CHTLLexer 词法分析器

这是CHTL项目的核心词法分析器，负责将CHTL源代码转换为Token流。

## 功能特性

### 完整的Token支持
- **基础Token**: 标识符、数字、字符串等
- **关键字Token**: 模板、自定义、原始嵌入、导入等系统关键字
- **操作符Token**: 冒号、等号、点号等操作符
- **分隔符Token**: 大括号、中括号、小括号等
- **注释Token**: 单行注释、多行注释、生成器注释
- **特殊符号Token**: 与号、@符号、井号等

### 严格的语法遵循
- 严格按照CHTL语法文档定义所有Token类型
- 支持CE对等式（冒号与等号完全等价）
- 支持无修饰字面量
- 支持UTF-8编码和中文

### 完整的错误处理
- 详细的错误位置信息（行号、列号）
- 多种错误类型支持
- 错误恢复机制

## 使用方法

### 基本用法

```cpp
#include "CHTLLexer.h"

// 从字符串创建词法分析器
std::string source = "div { text { Hello } }";
CHTLLexer lexer(source);

// 执行词法分析
auto tokens = lexer.tokenize();

// 检查是否有错误
if (lexer.hasErrors()) {
    for (const auto& error : lexer.getErrors()) {
        std::cout << error << std::endl;
    }
}
```

### 流式访问

```cpp
// 重置分析器状态
lexer.reset();

// 逐个获取Token
std::shared_ptr<Token> token;
while ((token = lexer.getNextToken())) {
    std::cout << token->toString() << std::endl;
}
```

### 位置信息

```cpp
// 获取当前分析位置
size_t line = lexer.getCurrentLine();
size_t column = lexer.getCurrentColumn();
size_t position = lexer.getCurrentPosition();
```

## Token类型

### 基础Token
- `UNKNOWN` - 未知Token
- `END_OF_FILE` - 文件结束
- `WHITESPACE` - 空白字符
- `NEWLINE` - 换行符

### 注释Token
- `SINGLE_LINE_COMMENT` - // 单行注释
- `MULTI_LINE_COMMENT` - /**/ 多行注释
- `GENERATOR_COMMENT` - -- 生成器注释

### 字面量Token
- `STRING_LITERAL` - 字符串字面量
- `UNQUOTED_LITERAL` - 无修饰字面量
- `NUMBER_LITERAL` - 数字字面量

### 关键字Token
- `TEMPLATE` - [Template]
- `CUSTOM` - [Custom]
- `ORIGIN` - [Origin]
- `IMPORT` - [Import]
- `CONFIGURATION` - [Configuration]
- `NAMESPACE` - [Namespace]
- `TEXT` - text
- `STYLE` - style
- `SCRIPT` - script

### 操作符Token
- `COLON` - : (与=等价)
- `EQUALS` - = (与:等价)
- `DOT` - .
- `SLASH` - /
- `COMMA` - ,
- `SEMICOLON` - ;

### 分隔符Token
- `LEFT_BRACE` - {
- `RIGHT_BRACE` - }
- `LEFT_BRACKET` - [
- `RIGHT_BRACKET` - ]
- `LEFT_PAREN` - (
- `RIGHT_PAREN` - )

### 特殊符号Token
- `AMPERSAND` - & (上下文推导)
- `AT_SYMBOL` - @ (模板前缀)
- `HASH_SYMBOL` - # (ID选择器)
- `PERIOD` - . (类选择器)

## GlobalMap功能

GlobalMap提供了全局的静态映射表，支持：

- 关键字映射
- 操作符映射
- 分隔符映射
- 特殊符号映射
- 选择器映射
- 配置映射
- 路径映射
- 通配符映射

```cpp
auto& globalMap = GlobalMap::getInstance();

// 检查是否为关键字
if (globalMap.isKeyword("template")) {
    auto type = globalMap.getKeywordType("template");
    std::cout << "Token类型: " << static_cast<int>(type) << std::endl;
}

// 获取统计信息
std::cout << "关键字数量: " << globalMap.getKeywordCount() << std::endl;
std::cout << "操作符数量: " << globalMap.getOperatorCount() << std::endl;
```

## 测试

运行测试程序：

```bash
cd src/CHTL/CHTLLexer
g++ -std=c++17 -I../../.. test_lexer.cpp CHTLLexer.cpp Token.cpp GlobalMap.cpp -o test_lexer
./test_lexer
```

## 设计原则

1. **严格遵循语法文档**: 不私自扩展，不偏离规范
2. **完全模块化**: 独立的Token、GlobalMap、Lexer实现
3. **高质量代码**: 使用C++17，RAII管理模式
4. **完整错误处理**: 详细的错误信息和恢复机制
5. **高性能**: 高效的字符处理和Token识别

## 注意事项

1. 词法分析器只负责Token识别，不进行语法分析
2. 支持UTF-8编码，确保中文支持完整
3. 错误处理采用非中断模式，尽可能识别更多Token
4. GlobalMap采用单例模式，确保全局唯一性
5. 所有Token都包含完整的位置信息（行号、列号）
