# CHTL编译器项目

CHTL是基于C++17实现的超文本语言编译器，提供了模块化的架构设计。

## 项目结构

```
CHTL/
├── CHTL/               # CHTL核心编译器
│   ├── CHTLLexer/      # 词法分析器
│   │   ├── Token.h/cpp # Token定义
│   │   ├── GlobalMap.h/cpp # 全局映射表
│   │   └── Lexer.h/cpp # 词法分析器
│   ├── CHTLNode/       # AST节点定义
│   │   ├── BaseNode.h/cpp # 基础节点
│   │   └── ...         # 其他节点类型
│   ├── CHTLParser.cpp  # 语法分析器
│   ├── CHTLGenerator.cpp # 代码生成器
│   └── ...
├── CHTLJS/             # CHTL JS编译器
│   ├── CHTLJSLexer/    # CHTL JS词法分析器
│   ├── CHTLJSNode.cpp  # CHTL JS节点
│   └── ...
├── CSS/                # CSS编译器（基于ANTLR4）
├── JS/                 # JavaScript编译器（基于ANTLR4）
├── Scanner/            # 统一扫描器
├── CompilerDispatcher/ # 编译器调度器
├── Util/               # 工具类
├── ThirdParty/         # 第三方库
├── Test/               # 测试代码
└── Module/             # 模块目录
```

## 编译方法

```bash
mkdir build
cd build
cmake ..
make
```

## 使用方法

```bash
./bin/chtl_compiler input.chtl -o output.html
```

## 特性

- 完全模块化设计，CHTL和CHTL JS完全独立
- 支持CMOD和CJMOD模块系统
- 统一扫描器实现精准代码切割
- 支持局部样式和局部脚本
- 自动化选择器管理
- 完整的错误处理机制

## 开发状态

- [x] 项目基础结构
- [ ] 统一扫描器实现
- [ ] CHTL词法分析器
- [ ] CHTL语法分析器
- [ ] CHTL代码生成器
- [ ] CHTL JS编译器
- [ ] CMOD/CJMOD模块系统
- [ ] 测试用例