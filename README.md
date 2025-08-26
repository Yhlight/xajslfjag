# CHTL 编译器项目

CHTL（Chtholly Hyper Text Language）是一个基于 C++ 17 实现的超文本语言编译器，旨在提供更符合开发者习惯的 HTML 编写方式。

## 项目架构

```
CHTL源代码
    ↓
CHTLUnifiedScanner (精准代码切割器)
    ↓
CompilerDispatcher (编译器调度器)
    ↓
CHTL/CHTL JS/CSS/JS 编译器
    ↓
编译结果合并 (HTML输出)
```

## 当前进展

### ✅ 已完成

1. **CHTLUnifiedScanner 统一扫描器**
   - 实现可变长度切片机制
   - 支持精准代码片段切割
   - 提供 CJMOD 特殊扫描器接口
   - 实现扫描器政策（NORMAL、COLLECT、SKIP）

2. **CompilerDispatcher 编译器调度器**
   - 实现代码片段分发机制
   - 支持并行/串行编译模式
   - 提供编译结果缓存
   - 实现错误处理和结果合并

3. **CHTL 编译器核心结构**
   - Token 定义：支持所有 CHTL 语法元素
   - GlobalMap：全局符号表管理
   - State：状态管理和 RAII 状态守卫
   - Context：编译上下文管理
   - Lexer：词法分析器框架

4. **CHTL JS 编译器核心结构**
   - CHTLJSToken：扩展 Token 支持 CHTL JS 特性
   - Syntax/Arg：实现 syntaxAnalys API
   - CHTLJSContext：支持虚对象、事件委托、动画管理
   - CHTLJSLexer：CHTL JS 词法分析器

### 🚧 待实现

5. CHTL AST 节点体系
6. CHTL JS AST 节点体系
7. CHTL 解析器 (Parser)
8. CHTL JS 解析器 (Parser)
9. CHTL 生成器 (Generator)
10. CHTL JS 生成器 (Generator)
11. RAII 状态机实现
12. Import 功能增强
13. 命名空间功能
14. Cmod 体系
15. CJmod 体系完整实现
16. 官方模块前缀 chtl::
17. 选择器自动化规则
18. 约束器实现
19. CHTL 语法细节
20. CHTL JS 语法细节
21. 配置组功能
22. use 语法
23. except 约束语法
24. 编译结果合并模块
25. ANTLR4 集成
26. 扫描器政策机制完善
27. 双指针扫描和前置截取
28. Chtholly 珂朵莉模块
29. Yuigahama 由比滨结衣模块
30. 测试体系

## 构建方法

```bash
mkdir build
cd build
cmake ..
make
```

## 使用方法

```bash
./chtl [options] <input-file>

Options:
  -o <file>    Output file (default: output.html)
  -m <path>    Module path (default: ./modules)
  --debug      Enable debug mode
  --strict     Enable strict mode
  --help       Show this help message
```

## 核心特性

### CHTL 语法特性
- 元素和属性的简洁语法
- 模板系统（@Style、@Element、@Var）
- 自定义系统（[Custom]）
- 原始嵌入（[Origin]）
- 命名空间支持
- 配置组系统
- CE 对等式（: 和 = 等价）
- 无修饰字面量

### CHTL JS 特性
- 增强选择器 {{}}
- 箭头操作符 ->
- 虚对象 vir
- 事件监听器 listen
- 事件委托 delegate
- 动画系统 animate
- CJmod 扩展机制

## 开发规范

1. 使用 C++ 17 标准
2. 严格遵循 CHTL 语法文档
3. 模块化设计，每个编译器独立文件
4. 使用命名空间 `chtl` 和 `chtl::js`
5. RAII 原则管理资源

## 许可证

MIT License