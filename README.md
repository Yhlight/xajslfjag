# CHTL - C++ HyperText Language

CHTL是一个基于C++的超文本语言，提供了一种新的方式来编写结构化的网页内容。它包含两个主要组件：

- **CHTL**: 核心标记语言，支持模板、自定义元素、样式块、导入系统等特性
- **CHTL JS**: 独立的编程语言，编译到JavaScript，提供增强的选择器、事件处理和动画支持

## 特性

### CHTL语言特性
- 多种注释风格（`//`, `/* */`, `--`）
- 文本节点和字面量（无引号、单引号、双引号）
- CE等价性（`:` 和 `=`）
- HTML元素支持
- 属性系统
- 局部样式块（内联样式、自动类/ID、上下文推断`&`）
- 模板系统（`[Template] @Style`, `@Element`, `@Var`，支持继承）
- 自定义元素（`[Custom]`，支持特化、索引访问、插入/删除操作）
- 完全限定名
- 源嵌入（`[Origin] @Html`, `@Style`, `@JavaScript`，自定义类型）
- 配置组（`[Configuration]`，命名配置，关键字重定义，源类型定义）
- 导入系统（`[Import]`，支持各种文件类型和CHTL组件，详细的路径解析和通配符支持）
- `use`语句（HTML5，命名配置）
- 命名空间（`[Namespace]`，支持嵌套、合并、冲突检测、默认命名空间）
- 约束系统（`except`）

### CHTL JS语言特性
- `.cjjs`文件扩展名
- AMD风格模块导入（`module { load: ... }`）
- 局部脚本块
- 增强选择器（`{{CSS selector}}`, `{{.class}}`, `{{#id}}`, `{{tag[index]}}`）
- `->`操作符用于CHTL JS语法
- 增强监听器（`listen {}`）
- 事件委托（`delegate {}`）
- 动画（`animate {}`）
- 虚拟对象（`vir`）
- 事件绑定操作符（`&->`）

### 系统特性
- 统一扫描器：精确代码切割，变长切片
- 编译器调度器：协调不同编译器
- CMOD系统：CHTL的模块化系统
- CJMOD系统：CHTL JS的扩展系统，允许C++开发者通过API扩展语法
- 错误报告系统：统一的错误处理和报告
- 编译监视器：检测无限循环，监控资源使用
- 完整的测试套件

## 构建要求

- C++17兼容的编译器（GCC 7+, Clang 5+, MSVC 2017+）
- CMake 3.10+
- 线程支持
- （可选）ANTLR4运行时（用于CSS/JS编译器）
- （可选）libzip（用于CMOD/CJMOD打包）

## 构建步骤

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake .. -DCMAKE_BUILD_TYPE=Release

# 构建
make -j$(nproc)

# 运行测试
make test
```

### 构建选项

- `BUILD_TESTS`: 构建测试套件（默认：ON）
- `USE_ANTLR`: 启用ANTLR4支持（默认：OFF）
- `CMAKE_BUILD_TYPE`: 构建类型（Debug/Release）

## 使用方法

### CHTL编译器

```bash
# 编译CHTL文件
chtlc input.chtl -o output.html

# 指定输出目录
chtlc input.chtl -d output_dir/

# 启用调试输出
chtlc input.chtl --debug

# 监视模式（自动重新编译）
chtlc input.chtl --watch

# 压缩输出
chtlc input.chtl --minify

# 美化输出
chtlc input.chtl --prettify
```

### CMOD打包工具

```bash
# 打包CMOD模块
cmod_pack pack module_dir/ output.cmod

# 解包CMOD模块
cmod_pack unpack module.cmod output_dir/

# 查看CMOD信息
cmod_pack info module.cmod

# 验证目录结构
cmod_pack validate module_dir/
```

### CJMOD打包工具

```bash
# 打包CJMOD扩展
cjmod_pack pack extension_dir/ output.cjmod

# 签名打包
cjmod_pack pack extension_dir/ output.cjmod --sign --key private.key

# 解包并验证签名
cjmod_pack unpack extension.cjmod output_dir/ --verify

# 测试扩展
cjmod_pack test extension.cjmod
```

## 项目结构

```
src/
├── CHTL/                    # CHTL编译器核心
│   ├── CHTLContext/         # 上下文管理
│   ├── CHTLGenerator/       # 代码生成器
│   ├── CHTLLexer/          # 词法分析器
│   ├── CHTLLoader/         # 加载器（导入解析）
│   ├── CHTLManage/         # 管理器（命名空间、选择器自动化、约束）
│   ├── CHTLNode/           # AST节点
│   ├── CHTLParser/         # 语法分析器
│   ├── CHTLState/          # 状态管理
│   ├── CHTLIOStream/       # 文件系统工具
│   └── CMODSystem/         # CMOD打包系统
├── CHTLJS/                 # CHTL JS编译器
│   ├── CHTLJSContext/      # 上下文管理
│   ├── CHTLJSGenerator/    # 代码生成器
│   ├── CHTLJSLexer/        # 词法分析器
│   ├── CHTLJSLoader/       # 加载器
│   ├── CHTLJSManage/       # 管理器
│   ├── CHTLJSNode/         # AST节点
│   ├── CHTLJSParser/       # 语法分析器
│   ├── CHTLJSState/        # 状态管理
│   ├── CHTLJSIOStream/     # 文件系统工具
│   └── CJMODSystem/        # CJMOD扩展系统
├── Scanner/                # 统一扫描器
├── CompilerDispatcher/     # 编译器调度器
├── CSS/                    # CSS编译器（ANTLR）
├── JS/                     # JavaScript编译器（ANTLR）
├── Error/                  # 错误报告系统
├── Test/                   # 测试套件
│   ├── TokenTestUtil/      # Token测试工具
│   ├── ASTTestUtil/        # AST测试工具
│   ├── CompilationMonitor/ # 编译监视器
│   └── UtilTest/          # 工具类测试
├── Tools/                  # 命令行工具
└── Util/                   # 通用工具类
```

## 示例

### 基本CHTL文件

```chtl
use html5

[Template] @Style {
    .container {
        max-width: 1200px;
        margin: 0 auto;
    }
}

[Template] @Element Header {
    header {
        h1: "Welcome to CHTL"
        nav {
            a[href="/home"]: "Home"
            a[href="/about"]: "About"
        }
    }
}

div.container {
    Header
    main {
        p: "CHTL makes web development easier!"
    }
}
```

### CHTL JS示例

```cjjs
module {
    load: ["jquery", "utils"]
}

listen {
    {{.button}} -> {
        click: (e) => {
            e.preventDefault()
            console.log("Button clicked!")
        }
    }
}

animate {
    {{#logo}} -> {
        duration: 1000,
        easing: "ease-in-out",
        from: { opacity: 0 },
        to: { opacity: 1 }
    }
}
```

## 开发指南

### 添加新的AST节点

1. 在相应的Node目录中创建新的节点类
2. 继承自BaseNode或特定的节点基类
3. 实现accept方法用于访问者模式
4. 更新Parser以识别新的语法结构

### 扩展CHTL JS

1. 创建CJMOD扩展目录结构
2. 实现C++ API接口
3. 定义语法规则（JSON格式）
4. 编译并打包为.cjmod文件

### 错误处理

使用统一的ErrorReport系统：

```cpp
ErrorBuilder(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR)
    .withMessage("Invalid syntax")
    .atLocation(file, line, column)
    .withSuggestion("Check your brackets")
    .report();
```

## 贡献

欢迎贡献代码！请确保：

1. 遵循现有的代码风格
2. 添加适当的测试
3. 更新文档
4. 所有测试通过

## 许可证

本项目采用MIT许可证。详见LICENSE文件。