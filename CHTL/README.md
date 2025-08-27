# CHTL - 基于C++的超文本语言

CHTL（C++ HyperText Language）是一种新型的超文本语言，旨在提供更优雅、模块化的HTML编写方式。

## 特性

### CHTL核心特性
- **模块化设计**：支持模板（Template）和自定义（Custom）组件
- **局部样式块**：在元素内部直接编写样式，自动处理作用域
- **增强的导入系统**：支持导入HTML、CSS、JS、CHTL文件和模块
- **命名空间**：避免组件命名冲突
- **配置系统**：灵活的配置选项，支持自定义关键字
- **原始嵌入**：支持嵌入原始HTML、CSS、JavaScript代码

### CHTL JS特性
- **增强选择器**：使用`{{}}`语法快速选择DOM元素
- **虚对象（vir）**：编译时的语法糖，提供元信息访问能力
- **简化的事件绑定**：`listen`和`&->`语法
- **内置动画系统**：`animate`表达式，支持复杂动画定义
- **事件委托**：`delegate`表达式，解决动态元素事件绑定问题

## 项目结构

```
CHTL/
├── CHTL/                    # CHTL核心编译器
│   ├── CHTLContext/         # 上下文管理
│   ├── CHTLGenerator/       # 代码生成器
│   ├── CHTLLexer/          # 词法分析器
│   ├── CHTLLoader/         # 文件加载器
│   ├── CHTLManage/         # 管理器
│   ├── CHTLNode/           # AST节点定义
│   ├── CHTLParser/         # 语法分析器
│   ├── CHTLState/          # 状态管理
│   └── CMODSystem/         # CMOD模块系统
├── CHTLJS/                 # CHTL JS编译器
│   ├── CHTLJSContext/      # JS上下文
│   ├── CHTLJSGenerator/    # JS代码生成
│   ├── CHTLJSLexer/        # JS词法分析
│   ├── CHTLJSLoader/       # JS加载器
│   ├── CHTLJSManage/       # JS管理器
│   ├── CHTLJSNode/         # JS AST节点
│   ├── CHTLJSParser/       # JS语法分析
│   ├── CHTLJSState/        # JS状态管理
│   └── CJMODSystem/        # CJMOD模块系统
├── CSS/                    # CSS处理器
├── JS/                     # JavaScript处理器
├── Scanner/                # 扫描器
├── CompilerDispatcher/     # 编译调度器
├── ThirdParty/            # 第三方库
├── Util/                  # 工具类
│   └── FileSystem/        # 文件系统工具
├── Test/                  # 测试
└── Module/                # 模块目录
```

## 编译和安装

### 依赖
- C++17或更高版本
- CMake 3.16或更高版本

### 编译步骤
```bash
mkdir build
cd build
cmake ..
make
```

### 运行测试
```bash
make test
```

## 使用示例

### 基础CHTL代码
```chtl
div {
    style {
        .box {
            width: 100px;
            height: 100px;
            background-color: red;
        }
    }
    
    text {
        "Hello, CHTL!"
    }
}
```

### CHTL JS示例
```chtl
script {
    {{.box}}->listen {
        click: () => {
            console.log('Box clicked!');
        }
    };
}
```

## 语法文档

详细的语法说明请参考 [CHTL语法文档.md](CHTL语法文档.md)

## 开发状态

CHTL项目目前处于活跃开发阶段，已完成：
- ✅ 基础项目结构
- ✅ CHTL词法分析器
- ✅ CHTL JS词法分析器
- ✅ AST节点定义
- ✅ 文件系统工具

正在开发：
- 🚧 语法分析器
- 🚧 代码生成器
- 🚧 模块系统
- 🚧 标准库

## 贡献

欢迎提交Issue和Pull Request！

## 许可证

MIT License