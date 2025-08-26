# CHTL 编译器项目

CHTL（C++ HTML Template Language）是一个基于C++实现的超文本语言编译器。

## 当前进度

### 已完成的基础架构

1. **Token系统** (`CHTLLexer/CHTLToken.h/cpp`)
   - 完整的Token类型定义，覆盖所有CHTL语法
   - 关键字映射表
   - Token辅助函数

2. **状态管理** (`CHTLState/CHTLState.h/cpp`)
   - RAII状态管理器
   - 状态栈管理
   - 状态查询和验证功能
   - 支持所有CHTL状态类型（全局、命名空间、元素、样式、脚本等）

3. **全局符号表** (`CHTLManage/CHTLGlobalMap.h/cpp`)
   - 符号管理（模板变量、自定义变量、样式组等）
   - 命名空间管理（支持同名命名空间合并）
   - 导入管理
   - 配置管理
   - 类名和ID使用统计

4. **上下文管理** (`CHTLContext/CHTLContext.h/cpp`)
   - RAII上下文管理器
   - 文件和目录管理
   - 局部符号表
   - 错误和警告管理
   - 编译选项
   - 模块路径解析

5. **词法分析器框架** (`CHTLLexer/CHTLLexer.h`)
   - Lexer类定义（待实现）
   - 多种扫描模式支持

## 测试

- `test_basic.cpp` - Token和关键字映射测试
- `test_state_context.cpp` - State和Context综合测试

运行测试：
```bash
make test
```

## 下一步计划

根据目标规划文件，接下来需要：
1. 实现CHTLLexer的具体功能
2. 创建AST节点定义
3. 实现Parser
4. 实现Generator
5. 实现统一扫描器（CHTLUnifiedScanner）

## 编译

使用C++17标准：
```bash
make all
```

## 注意事项

- 严格遵循CHTL语法文档，不私自扩展
- 使用RAII管理资源
- 保持代码的可测试性