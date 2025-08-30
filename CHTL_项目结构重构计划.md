# CHTL项目结构重构计划

## 当前结构 vs 标准结构对比

### ❌ 当前结构（不符合标准）
```
src/CHTL/
├── CHTLContext/
├── CHTLGenerator/
├── CHTLLexer/
├── CHTLManage/
├── CHTLNode/
├── CHTLParser/
├── CHTLState/
├── CHTLIOStream/
├── CHTLCompiler/        # 这个应该是CompilerDispatcher
├── CHTLScanner/         # 这个应该是Scanner
├── CJMOD/              # 这个应该在CHTL JS下面
├── Error/              # 位置正确
└── grammars/           # 这个应该在CSS/JS下面
```

### ✅ 标准结构（用户要求）
```
src/
├── CHTL/                           # CHTL编译器
│   ├── CHTLContext/
│   ├── CHTLGenerator/
│   ├── CHTLLexer/                  # 内含Lexer，GlobalMap，Token文件
│   ├── CHTLLoader/
│   ├── CHTLManage/
│   ├── CHTLNode/                   # 内含BaseNode等所有节点文件
│   ├── CHTLParser/
│   ├── CHTLState/
│   ├── CHTLIOStream/
│   └── CMODSystem/                 # CMOD系统
│
├── CHTL JS/                        # CHTL JS编译器（独立）
│   ├── CHTLJSContext/
│   ├── CHTLJSGenerator/
│   ├── CHTLJSLexer/               # 内含Lexer，GlobalMap，Token文件
│   ├── CHTLJSLoader/
│   ├── CHTLJSManage/
│   ├── CHTLJSNode/                # 内含很多独立节点
│   ├── CHTLJSParser/
│   ├── CHTLJSState/
│   ├── CHTLJSIOStream/
│   └── CJMODSystem/               # CJMOD系统
│
├── CSS/                           # CSS编译器（独立）
├── JS/                            # JS编译器（独立）
├── Scanner/                       # 统一扫描器（独立）
├── CompilerDispatcher/            # 编译调度器（独立）
├── ThirdParty/                    # 第三方库
│
├── Util/                          # 工具类
│   ├── FileSystem/
│   └── ZIPUtil/
│
├── Test/                          # 测试系统
│   ├── TokenTest/                 # TokenPrint，TokenTable
│   ├── ASTTest/                   # ASTPrint，ASTGraph
│   ├── CompilerTimeMonitor/       # 编译时间监控，内存监视
│   └── UtilTest/                  # 单元测试
│
├── Module/                        # 官方CMOD和CJMOD源码
└── Error/                         # 统一错误处理
```

## 重构必要性

### 🎯 为什么需要重构

1. **独立性原则**：CHTL和CHTL JS应该是两个独立的编译器
2. **模块化设计**：每个编译器有自己完整的组件集合
3. **清晰的职责分离**：Scanner、CompilerDispatcher作为独立模块
4. **标准化结构**：符合大型编译器项目的标准组织方式

### 🎯 当前结构的问题

1. **CHTL JS组件缺失**：没有独立的CHTL JS编译器结构
2. **混合在一起**：CJMOD放在了CHTL下面，应该在CHTL JS下面
3. **缺少独立模块**：CSS、JS、Scanner、CompilerDispatcher应该独立
4. **测试结构不规范**：测试文件散落在各处，应该统一在Test下

## 重构计划

### 阶段1：创建标准目录结构
1. 创建`CHTL JS/`独立目录及其子目录
2. 创建`CSS/`、`JS/`、`Scanner/`、`CompilerDispatcher/`独立目录
3. 重组`Test/`目录结构
4. 创建`Module/`、`ThirdParty/`目录

### 阶段2：迁移现有代码
1. 将相关代码迁移到正确的目录
2. 复制CHTL的结构到CHTL JS（并调整为CHTL JS特定）
3. 移动测试文件到统一的Test目录
4. 更新所有的include路径

### 阶段3：更新构建系统
1. 重写CMakeLists.txt以适应新结构
2. 确保各个独立编译器能够正确构建
3. 更新测试构建配置

### 阶段4：验证重构结果
1. 确保所有功能正常工作
2. 验证编译顺序正确
3. 确保测试通过

## 重构的好处

### ✅ 符合标准结构
- 大型编译器项目的标准组织方式
- 清晰的模块边界和职责分离
- 便于维护和扩展

### ✅ 独立编译器设计
- CHTL编译器：完整独立的组件集合
- CHTL JS编译器：完整独立的组件集合
- CSS/JS编译器：独立的第三方编译器集成

### ✅ 更好的可维护性
- 每个编译器有自己的Context、Lexer、Parser等
- 测试统一管理，便于测试和调试
- 工具类统一放置，便于复用

## 确认重构

**是否需要按照标准结构进行重构？**

这将是一个较大的重构工作，但会让项目结构更加标准化和清晰化，符合大型编译器项目的最佳实践。