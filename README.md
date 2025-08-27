# CHTL 编译器项目

CHTL是一个基于C++17实现的现代化超文本标记语言编译器，采用完全模块化的架构设计，支持CHTL和CHTL JS两种独立的编程语言。

## 项目特性

- **完全模块化设计**: CHTL和CHTL JS采用完全分离的架构，各自管理独立的文件体系
- **统一扫描器**: 核心的CHTLUnifiedScanner支持精准代码切割和可变长度切片处理
- **四编译器协调**: CompilerDispatcher管理CHTL、CHTL JS、CSS、JavaScript四种编译器的协调工作
- **高性能**: 支持并行编译和结果缓存
- **严格语法遵循**: 严格按照CHTL语法文档实现，不私自扩展或偏离语法规范
- **UTF-8支持**: 全程支持UTF-8编码，确保中文等多语言字符正确处理

## 架构设计

```
CHTL源代码
    ↓
CHTLUnifiedScanner (精准代码切割器)
    ↓
┌─────────────┬──────────────┬──────────────┬────────────┐
│   CHTL片段   │ CHTL JS片段  │   CSS片段    │   JS片段   │
└──────┬──────┴──────┬───────┴──────┬───────┴──────┬─────┘
       ▼              ▼              ▼              ▼
CompilerDispatcher (编译器调度器)
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
                    编译结果合并 (HTML输出)
```

## 项目结构

```
CHTL/
├── src/
│   ├── CHTL/                     # CHTL编译器模块
│   │   ├── CHTLContext/          # CHTL上下文管理
│   │   ├── CHTLGenerator/        # CHTL代码生成器
│   │   ├── CHTLLexer/           # CHTL词法分析器
│   │   ├── CHTLLoader/          # CHTL加载器
│   │   ├── CHTLManage/          # CHTL管理器
│   │   ├── CHTLNode/            # CHTL AST节点
│   │   ├── CHTLParser/          # CHTL语法分析器
│   │   ├── CHTLState/           # CHTL状态管理
│   │   └── CMODSystem/          # CMOD模块系统
│   ├── CHTLJS/                  # CHTL JS编译器模块 (完全独立)
│   │   ├── CHTLJSContext/       # CHTL JS上下文管理
│   │   ├── CHTLJSGenerator/     # CHTL JS代码生成器
│   │   ├── CHTLJSLexer/        # CHTL JS词法分析器
│   │   ├── CHTLJSLoader/       # CHTL JS加载器
│   │   ├── CHTLJSManage/       # CHTL JS管理器
│   │   ├── CHTLJSNode/         # CHTL JS AST节点
│   │   ├── CHTLJSParser/       # CHTL JS语法分析器
│   │   ├── CHTLJSState/        # CHTL JS状态管理
│   │   └── CJMODSystem/        # CJMOD模块系统
│   ├── CSS/                     # CSS编译器 (基于ANTLR)
│   ├── JS/                      # JavaScript编译器 (基于ANTLR)
│   ├── Scanner/                 # 统一扫描器
│   ├── CompilerDispatcher/      # 编译器调度器
│   ├── Util/                    # 工具模块
│   └── main.cpp                 # 主程序入口
├── ANTLR4/                      # ANTLR4运行时库
├── css/                         # CSS语法文件 (ANTLR生成)
├── js/                          # JavaScript语法文件 (ANTLR生成)
├── Grammars/                    # 语法定义文件
├── CMakeLists.txt               # 构建配置
└── README.md                    # 项目说明
```

## 构建要求

- C++17 或更高版本
- CMake 3.16 或更高版本
- ANTLR4 运行时库 (已包含)
- 支持UTF-8的编译器 (GCC, Clang, MSVC)

## 构建步骤

### Windows (Visual Studio)

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

### Linux/macOS

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## 使用方法

### 基本用法

```bash
# 编译单个文件
./chtl input.chtl -o output.html

# 启用调试模式
./chtl input.chtl --debug --stats

# 查看帮助
./chtl --help

# 查看版本信息
./chtl --version
```

### 命令行选项

- `-h, --help`: 显示帮助信息
- `-o, --output <文件>`: 指定输出文件
- `-d, --debug`: 启用调试模式
- `-p, --parallel`: 启用并行编译
- `-s, --stats`: 显示编译统计信息
- `-v, --verbose`: 详细输出
- `--version`: 显示版本信息

## CHTL语法示例

```chtl
use html5;

html
{
    head
    {
        text { "CHTL示例页面" }
    }

    body
    {
        div
        {
            id: container;
            class: main-content;

            style
            {
                .main-content
                {
                    width: 100%;
                    max-width: 800px;
                    margin: 0 auto;
                    padding: 20px;
                }
            }

            button
            {
                text { "点击我" }

                script
                {
                    {{button}}->listen {
                        click: () => {
                            console.log('按钮被点击了！');
                        }
                    };
                }
            }
        }
    }
}

[Template] @Style DefaultButton
{
    background-color: #007bff;
    color: white;
    border: none;
    padding: 10px 20px;
    border-radius: 5px;
}
```

## 核心特性

### 1. 模块化架构
- CHTL和CHTL JS完全独立，各自拥有独立的Token、GlobalMap、State、Context、Lexer系统
- 每个编译器只负责自己的语法，不插手其他编译器的工作

### 2. 统一扫描器
- 支持精准代码切割，基于可变长度切片
- 自动检测和扩展片段以确保语法完整性
- 支持CHTL和CHTL JS最小单元的二次切割

### 3. 编译器调度
- 智能分发编译任务到相应的编译器
- 支持并行编译以提高性能
- 结果缓存和优化

### 4. CHTL JS特性
- 虚对象(vir)支持
- 增强选择器({{selector}})
- 事件绑定操作符(&->)
- 声明式事件监听(listen)
- 事件委托(delegate)
- 动画支持(animate)

## 开发规范

- 严格遵循CHTL语法文档，不私自扩展语法
- 使用大驼峰命名法
- 完全分离的模块化开发
- 每个编译器独立管理自己的文件体系
- 高质量、高标准的代码实现

## 许可证

MIT License

## 贡献

欢迎提交Issue和Pull Request来改进CHTL编译器项目。

## 联系方式

CHTL Team - chtl@example.com