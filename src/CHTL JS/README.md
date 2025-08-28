# CHTL JS编译器

这是CHTL项目的CHTL JS编译器，负责解析和编译CHTL JS语法。

## 目录结构

- **CHTLJSContext/** - CHTL JS上下文管理，包含解析状态和符号表
- **CHTLJSGenerator/** - CHTL JS代码生成器，负责生成JavaScript输出
- **CHTLJSLexer/** - CHTL JS词法分析器，包含Lexer、GlobalMap、Token定义
- **CHTLJSLoader/** - CHTL JS文件加载器，负责文件读取和预处理
- **CHTLJSManage/** - CHTL JS管理器，协调各个组件的工作
- **CHTLJSNode/** - CHTL JS AST节点定义，包含各种语法节点类型
- **CHTLJSParser/** - CHTL JS语法分析器，构建AST树
- **CHTLJSState/** - CHTL JS状态机，管理解析过程中的状态转换
- **CJMODSystem/** - CJMOD模块系统，处理CHTL JS模块的导入和导出

## 核心特性

- 完全独立的CHTL JS语法编译器
- 与CHTL编译器完全分离，互不干涉
- 支持增强选择器、事件绑定、动画系统等特性
- 支持虚对象、事件委托等高级功能
- 支持module{}模块导入系统
- 支持.cjjs文件后缀

## 重要说明

**CHTL JS不是JavaScript的超集，而是一门完全独立的编程语言**
- 与JavaScript语法不兼容
- 最终转换为JavaScript代码
- 拥有自己的语法规则和特性
- 支持无修饰字面量

## 设计原则

- 完全独立的模块化设计
- 不共用CHTL编译器的任何组件
- 使用RAII管理模式
- 大驼峰命名法
- 完整的错误处理和恢复机制