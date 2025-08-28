# CHTL编译器

这是CHTL项目的核心编译器，负责解析和编译CHTL语法。

## 目录结构

- **CHTLContext/** - CHTL上下文管理，包含解析状态和符号表
- **CHTLGenerator/** - CHTL代码生成器，负责生成HTML输出
- **CHTLLexer/** - CHTL词法分析器，包含Lexer、GlobalMap、Token定义
- **CHTLLoader/** - CHTL文件加载器，负责文件读取和预处理
- **CHTLManage/** - CHTL管理器，协调各个组件的工作
- **CHTLNode/** - CHTL AST节点定义，包含各种语法节点类型
- **CHTLParser/** - CHTL语法分析器，构建AST树
- **CHTLState/** - CHTL状态机，管理解析过程中的状态转换
- **CMODSystem/** - CMOD模块系统，处理模块的导入和导出

## 核心特性

- 完全独立的CHTL语法编译器
- 不依赖其他编译器组件
- 严格遵循CHTL语法文档
- 支持模板、自定义、继承等高级特性
- 完整的命名空间和模块系统支持

## 设计原则

- 模块化设计，每个组件职责明确
- 使用RAII管理模式
- 大驼峰命名法
- 完整的错误处理和恢复机制