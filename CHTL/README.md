# CHTL 编译器项目

## 项目简介

CHTL（Custom HyperText Language）是一个基于C++17实现的超文本语言编译器，旨在提供更符合开发者习惯的HTML代码编写方式。本项目采用MIT开源协议，致力于打造清晰的架构设计、严格的语法定义以及完善的测试用例。

## 项目特点

- **完全模块化架构**: CHTL和CHTL JS编译器完全独立，各自维护独立的系统
- **严格语法遵循**: 严格按照CHTL语法文档进行实现，确保语法的完整性和准确性
- **面向社区开源**: 清晰的架构设计，便于社区贡献和维护
- **互不干涉设计**: 每个编译器只为自己的内容负责，不插手其他模块的事务

## 项目架构

```
CHTL/
├── CHTL/                     # CHTL主编译器
│   ├── CHTLContext/          # 编译上下文管理
│   ├── CHTLGenerator/        # 代码生成器
│   ├── CHTLLexer/           # 词法分析器
│   │   ├── Lexer.h/cpp      # 主词法分析器
│   │   ├── Token.h/cpp      # Token定义
│   │   └── GlobalMap.h/cpp  # 全局映射表
│   ├── CHTLLoader/          # 模块加载器
│   ├── CHTLManage/          # 编译管理器
│   ├── CHTLNode/            # AST节点系统
│   │   ├── BaseNode.h/cpp   # 基础节点类
│   │   ├── CommentNode.h/cpp # 注释节点
│   │   ├── CustomNode.h/cpp # 自定义节点
│   │   ├── OriginNode.h/cpp # 原始嵌入节点
│   │   ├── StyleNode.h/cpp  # 样式节点
│   │   ├── TemplateNode.h/cpp # 模板节点
│   │   └── NodeVisitor.h    # 访问者模式接口
│   ├── CHTLParser/          # 语法分析器
│   ├── CHTLState/           # 状态管理
│   └── CMODSystem/          # CMOD模块系统
├── CHTL JS/                 # CHTL JS编译器
│   ├── CHTLJSContext/       # CHTL JS编译上下文
│   ├── CHTLJSGenerator/     # CHTL JS代码生成器
│   ├── CHTLJSLexer/         # CHTL JS词法分析器
│   │   ├── Lexer.h/cpp      # CHTL JS词法分析器
│   │   ├── Token.h/cpp      # CHTL JS Token定义
│   │   └── GlobalMap.h/cpp  # CHTL JS全局映射
│   ├── CHTLJSLoader/        # CHTL JS加载器
│   ├── CHTLJSManage/        # CHTL JS管理器
│   ├── CHTLJSNode/          # CHTL JS节点系统
│   ├── CHTLJSParser/        # CHTL JS语法分析器
│   ├── CHTLJSState/         # CHTL JS状态管理
│   └── CJMODSystem/         # CJMOD模块系统
├── Scanner/                 # 统一代码扫描器
│   ├── Scanner.h/cpp        # 代码片段切割器
├── CSS/                     # CSS编译器模块
├── JS/                      # JavaScript编译器模块
├── CompilerDispatcher/      # 编译器调度器
├── ThirdParty/              # 第三方依赖
├── Util/                    # 工具模块
│   └── FileSystem/          # 文件系统工具
├── Test/                    # 测试模块
│   └── test_main.cpp        # 主测试程序
├── Module/                  # 模块存放目录
├── CMakeLists.txt           # CMake构建文件
└── README.md                # 项目说明文档
```

## 核心功能

### 1. 统一代码扫描器 (Scanner)
- 将CHTL源代码精确切割为不同类型的代码片段
- 支持CHTL、CHTL JS、CSS、JS等多种代码类型的识别
- 基于可变长度切片技术，确保代码片段的完整性

### 2. CHTL词法分析器 (CHTL Lexer)
- 支持完整的CHTL语法规范
- 包含所有CHTL关键字、操作符和特殊语法的识别
- 提供准确的位置信息和错误报告

### 3. CHTL JS词法分析器 (CHTL JS Lexer)
- 独立的CHTL JS语言词法分析
- 支持增强选择器 `{{.selector}}`
- 支持CHTL JS特有语法如 `listen`, `delegate`, `animate`, `vir` 等

### 4. AST节点系统 (Node System)
- 完整的抽象语法树节点层次结构
- 支持访问者模式进行节点遍历和处理
- 包含元素节点、文本节点、样式节点、模板节点等

### 5. 编译上下文管理 (Context)
- 作用域管理和符号表维护
- 模板、自定义内容的注册和查找
- 错误和警告信息的收集和管理

## 编译和使用

### 环境要求
- C++17 或更高版本
- CMake 3.16 或更高版本
- 支持的编译器：GCC, Clang, MSVC

### 编译步骤
```bash
# 进入项目目录
cd CHTL

# 创建构建目录
mkdir build && cd build

# 配置CMake
cmake ..

# 编译项目
make -j4

# 运行测试
./chtl_test
```

### 编译选项
- `BUILD_TESTS`: 是否构建测试程序（默认：ON）
- `CMAKE_BUILD_TYPE`: 构建类型（Debug/Release）

## 测试结果

项目包含完整的测试程序，验证各个核心模块的功能：

```
=== CHTL 编译器测试程序 ===
欢迎使用CHTL语言编译器！

1. 测试 Scanner（代码片段切割器）
扫描结果: 成功
发现 1 个代码片段

2. 测试 CHTL Lexer（词法分析器）
CHTL词法分析结果: 生成了 265 个Token

3. 测试 CHTL JS Lexer（CHTL JS词法分析器）
CHTL JS词法分析完成

4. 测试 CHTL Context（编译上下文）
编译上下文管理正常

=== 测试完成 ===
CHTL编译器核心模块运行正常！
```

## CHTL语法示例

```chtl
// 配置块
[Configuration]
{
    DEBUG_MODE = true;
}

// HTML结构
html
{
    body
    {
        div
        {
            id: main-container;
            class: container;

            // 局部样式块
            style
            {
                .container
                {
                    width: 100%;
                    height: 100vh;
                    background-color: #f0f0f0;
                }

                &:hover
                {
                    background-color: #e0e0e0;
                }
            }

            // CHTL JS脚本块
            script
            {
                {{.container}}->addEventListener('click', () => {
                    console.log('容器被点击了！');
                });

                const anim = animate {
                    target: {{.container}},
                    duration: 1000,
                    begin: { opacity: 0 },
                    end: { opacity: 1 }
                };
            }

            text
            {
                欢迎使用CHTL语言！
            }
        }
    }
}

// 模板定义
[Template] @Style ButtonStyle
{
    padding: 10px 20px;
    border: none;
    border-radius: 5px;
    background-color: #007bff;
    color: white;
    cursor: pointer;
}

// 自定义组件
[Custom] @Element Button
{
    button
    {
        style
        {
            @Style ButtonStyle;
        }
    }
}
```

## 开发理念

作为CHTL项目的开发者，我们：

- 将CHTL语法文档视为最高标准，严格按照语法文档进行实现
- 追求语法的完整性与绝对准确性
- 坚持完全分离的模块化开发方式
- 每个编译器都有自己独立的系统，互不干涉
- CHTL和CHTL JS完全独立，各自负责自己的领域

## 项目状态

✅ 项目架构完成  
✅ CHTL编译器核心模块实现  
✅ CHTL JS编译器基础模块实现  
✅ 统一代码扫描器实现  
✅ AST节点系统完成  
✅ 编译上下文管理完成  
✅ 测试程序验证通过  

## 下一步计划

- [ ] 完善CHTL语法分析器实现
- [ ] 实现代码生成器模块
- [ ] 完善CHTL JS语法分析器
- [ ] 实现CMOD和CJMOD模块系统
- [ ] 添加更多测试用例
- [ ] 完善文档和示例

## 许可证

本项目采用MIT开源许可证。详见LICENSE文件。

## 贡献

欢迎社区贡献！请确保遵循项目的模块化原则和代码规范。

---

*CHTL - 让HTML编写更加优雅*