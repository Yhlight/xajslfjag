# CHTL - 超文本语言编译器

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/CHTL-Team/CHTL)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Version](https://img.shields.io/badge/version-1.0.0-green.svg)](https://github.com/CHTL-Team/CHTL/releases)

CHTL是基于C++17实现的超文本语言，其本质是为了提供一种更符合开发者编写HTML代码的方式。CHTL使用MIT开源协议，致力于简化Web开发流程，提供强大的模板系统、自定义组件、以及先进的CHTL JS语法。

## 特性

### 🚀 核心特性
- **现代语法**: 更简洁、更直观的HTML编写方式
- **强大的模板系统**: 支持样式组模板、元素模板、变量组模板
- **自定义组件**: 支持特例化操作的自定义样式组、元素、变量组
- **原始嵌入**: 灵活的HTML、CSS、JavaScript原始代码嵌入
- **智能导入系统**: 强大的模块导入和依赖管理
- **CHTL JS**: 独特的JavaScript扩展语法

### 🛠️ 技术特性
- **高性能编译**: 基于C++17的高性能编译器
- **并行处理**: 支持多线程并行编译
- **智能缓存**: 增量编译和智能缓存系统
- **模块化设计**: CMOD和CJMOD模块系统
- **扩展性**: 支持CJMOD API扩展CHTL JS语法

### 📦 开发特性
- **完整的工具链**: 从编译到打包的完整解决方案
- **开发者友好**: 详细的错误信息和调试支持
- **跨平台**: 支持Windows、Linux、macOS
- **集成简单**: 可与现有构建系统集成

## 快速开始

### 系统要求

- C++17兼容编译器 (GCC 7+, Clang 6+, MSVC 2017+)
- CMake 3.15+
- 可选: ANTLR4 (用于CSS和JavaScript解析)

### 构建安装

```bash
# 克隆仓库
git clone https://github.com/CHTL-Team/CHTL.git
cd CHTL

# 创建构建目录
mkdir build && cd build

# 配置构建
cmake ..

# 编译
cmake --build . --config Release

# 可选: 安装
cmake --install .
```

### 基础使用

创建一个简单的CHTL文件 `hello.chtl`:

```chtl
html
{
    head
    {
        text
        {
            "Hello CHTL"
        }
    }

    body
    {
        div
        {
            style
            {
                .container
                {
                    width: 800px;
                    margin: 0 auto;
                    padding: 20px;
                }
            }

            text
            {
                "欢迎使用CHTL!"
            }
        }
    }
}
```

编译文件:

```bash
./chtl hello.chtl -o hello.html
```

## 语法文档

### 基础语法

#### 文本节点
```chtl
text
{
    "这是一段文本"
}
```

#### 元素节点
```chtl
div
{
    id: myDiv;
    class: container;
    
    text
    {
        Hello World
    }
}
```

#### 样式块
```chtl
div
{
    style
    {
        .box  // 自动添加类名
        {
            width: 300px;
            height: 200px;
            background-color: #f0f0f0;
        }
        
        &:hover  // 伪类选择器
        {
            background-color: #e0e0e0;
        }
    }
}
```

### 模板系统

#### 样式组模板
```chtl
[Template] @Style ButtonStyle
{
    padding: 10px 20px;
    border: none;
    border-radius: 4px;
    cursor: pointer;
}

button
{
    style
    {
        @Style ButtonStyle;
        background-color: #007bff;
        color: white;
    }
}
```

#### 元素模板
```chtl
[Template] @Element Card
{
    div
    {
        style
        {
            .card
            {
                border: 1px solid #ddd;
                border-radius: 8px;
                padding: 16px;
                box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            }
        }
    }
}

section
{
    @Element Card;
}
```

### CHTL JS

#### 选择器语法
```chtl
script
{
    {{.button}}->addEventListener('click', () => {
        console.log('按钮被点击!');
    });
    
    // 事件绑定操作符
    {{#submit-btn}} &-> click {
        console.log('提交按钮点击');
    }
}
```

#### 监听器
```chtl
script
{
    {{.menu}}->listen {
        click: handleMenuClick,
        mouseenter: () => {
            console.log('鼠标进入菜单');
        }
    };
}
```

#### 动画
```chtl
script
{
    const slideIn = animate {
        target: {{.sidebar}},
        duration: 300,
        easing: ease-in-out,
        
        begin: {
            transform: translateX(-100%);
        }
        
        end: {
            transform: translateX(0);
        }
    };
}
```

### 导入系统

```chtl
// 导入CHTL模块
[Import] @Chtl from "./components/Header"

// 导入样式文件
[Import] @Style from "styles.css" as MainStyles

// 导入CMOD包
[Import] @Chtl from "chtl::UI.Button"

// 导入自定义组件
[Import] [Custom] @Element Button from "./ui/Button.chtl"
```

## 架构设计

CHTL编译器采用模块化架构设计：

```
┌─────────────────────────────────────────────────────────────────┐
│                         CHTL源代码                               │
└─────────────────────────────┬───────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                    CHTLUnifiedScanner                           │
│                   (精准代码切割器)                               │
└─────────────────────────────┬───────────────────────────────────┘
                              │
                              ▼
        ┌──────────────┬──────────────┬──────────────┬────────────┐
        │   CHTL片段   │ CHTL JS片段  │   CSS片段    │   JS片段   │
        └──────┬───────┴──────┬───────┴──────┬───────┴──────┬─────┘
               │              │              │              │
               ▼              ▼              ▼              ▼
┌─────────────────────────────────────────────────────────────────┐
│                    CompilerDispatcher                           │
│                   (编译器调度器)                                 │
└──────┬──────────────┬──────────────┬──────────────┬────────────┘
       │              │              │              │
       ▼              ▼              ▼              ▼
┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐
│    CHTL     │ │   CHTL JS   │ │     CSS     │ │JavaScript   │
│  Compiler   │ │  Compiler   │ │  Compiler   │ │  Compiler   │
│  (手写)     │ │  (手写)     │ │  (ANTLR)    │ │  (ANTLR)    │
└─────────────┘ └─────────────┘ └─────────────┘ └─────────────┘
       │              │              │              │
       └──────────────┴──────────────┴──────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                      编译结果合并                                │
│                     (HTML输出)                                   │
└─────────────────────────────────────────────────────────────────┘
```

### 核心组件

- **CHTLUnifiedScanner**: 统一扫描器，实现精准的代码片段切割
- **CompilerDispatcher**: 编译器调度器，协调各个编译器的工作
- **CHTL Compiler**: 手写的CHTL编译器，处理核心语法
- **CHTL JS Compiler**: 手写的CHTL JS编译器，处理JavaScript扩展
- **CSS/JS Compiler**: 基于ANTLR4的CSS和JavaScript编译器

## 模块系统

### CMOD包

CMOD是CHTL模块的打包格式：

```
MyModule/
├── src/
│   ├── MyModule.chtl
│   ├── components/
│   └── styles/
├── info/
│   └── MyModule.chtl
└── submodules/
    └── SubModule/
        ├── src/
        └── info/
```

### CJMOD包

CJMOD允许使用C++扩展CHTL JS语法：

```cpp
#include "CJMODApi.h"

// 使用CJMOD API扩展语法
Arg args = Syntax::analyze("$ ** $");
args.bind("$", [](const std::string& value) {
    return value;
});
args.bind("**", [](const std::string& value) {
    return "Math.pow";
});
```

## 性能特性

- **并行编译**: 多线程处理大型项目
- **智能缓存**: 避免重复编译
- **增量构建**: 只编译修改的文件
- **内存优化**: 高效的AST管理
- **编译时优化**: 代码压缩和优化

## 开发工具

### 编译选项

```bash
# 基础编译
chtl input.chtl -o output.html

# 压缩输出
chtl input.chtl --minify -o output.html

# 调试模式
chtl input.chtl --debug --verbose

# 并行编译
chtl input.chtl -j 4

# 分离输出
chtl input.chtl --separate -d ./build
```

### 配置文件

支持配置文件自定义编译行为：

```ini
[Compilation]
EnableOptimizations=true
EnableCaching=true
ThreadCount=0

[Output]
MinifyOutput=false
GenerateSourceMaps=false
SeparateFiles=false

[Paths]
ModulePaths=./modules,./node_modules
OutputDirectory=./dist
```

## 贡献指南

我们欢迎社区贡献！请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解详细信息。

### 开发环境设置

```bash
# 克隆开发分支
git clone -b develop https://github.com/CHTL-Team/CHTL.git

# 安装开发依赖
cmake -B build -DBUILD_TESTS=ON -DBUILD_DOCS=ON

# 运行测试
cmake --build build --target test

# 生成文档
cmake --build build --target docs
```

## 许可证

本项目使用 [MIT许可证](LICENSE)。

## 支持与社区

- 📖 [完整文档](https://chtl-lang.org/docs)
- 🐛 [问题报告](https://github.com/CHTL-Team/CHTL/issues)
- 💬 [讨论社区](https://github.com/CHTL-Team/CHTL/discussions)
- 📧 联系我们: chtl-team@example.com

## 更新日志

### v1.0.0 (2024-01-01)
- 🎉 初始发布
- ✨ 完整的CHTL语法支持
- ✨ CHTL JS扩展语法
- ✨ 模板和自定义系统
- ✨ 模块导入系统
- ✨ CMOD/CJMOD包管理
- 🚀 高性能编译器
- 📦 跨平台支持

---

**CHTL - 让Web开发更简单、更强大！** 🚀