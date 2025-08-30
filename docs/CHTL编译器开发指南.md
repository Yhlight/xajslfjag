# CHTL编译器开发指南

## 🎯 开发环境搭建

### 系统要求
- **操作系统** - Windows 10+, Ubuntu 18.04+, macOS 10.15+
- **编译器** - GCC 9+, Clang 10+, MSVC 2019+
- **CMake** - 3.16或更高版本
- **内存** - 建议4GB以上
- **磁盘空间** - 建议2GB以上

### 依赖库
- **ANTLR4运行时** - 已包含在ThirdParty目录
- **标准库** - C++17标准库
- **文件系统库** - std::filesystem

## 🏗️ 构建步骤

### Linux/macOS构建
```bash
# 1. 克隆仓库
git clone <chtl-repo>
cd chtl-compiler

# 2. 进入源码目录
cd src_new

# 3. 创建构建目录
mkdir build && cd build

# 4. 配置CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# 5. 编译
make -j$(nproc)

# 6. 运行测试
./test_suite
```

### Windows构建
```cmd
REM 1. 克隆仓库
git clone <chtl-repo>
cd chtl-compiler

REM 2. 进入源码目录
cd src_new

REM 3. 创建构建目录
mkdir build && cd build

REM 4. 配置CMake
cmake .. -G "Visual Studio 16 2019" -A x64

REM 5. 编译
cmake --build . --config Release

REM 6. 运行测试
Release\test_suite.exe
```

## 🔧 核心组件开发

### 统一扫描器 (UnifiedScanner)

**职责** - 精准代码切割，智能片段识别

**关键特性**：
- 可变长度切片
- 前向扩增检测
- 最小单元切割
- 上下文连续性保持

**开发要点**：
```cpp
// 扫描器核心接口
class UnifiedScanner {
public:
    std::vector<CodeFragment> scanCode(const std::string& code);
    
private:
    // 可变长度切片
    std::vector<CodeFragment> variableLengthSlicing(const std::string& code);
    
    // 前向扩增检测
    bool needsForwardExpansion(const CodeFragment& fragment, const std::string& fullCode);
    
    // 最小单元切割
    std::vector<CodeFragment> minimalUnitCutting(const CodeFragment& fragment);
};
```

### 编译器调度器 (CompilerDispatcher)

**职责** - 协调多个编译器，正确的编译顺序

**编译顺序**：
1. CHTL片段（最高优先级）
2. CHTL JS片段
3. 合并CHTL和CHTL JS输出
4. CSS片段（接收完整代码）
5. JavaScript片段（接收完整代码）

**开发要点**：
```cpp
class CompilerDispatcher {
    // 分阶段编译
    std::vector<CompilationResult> compileFragments(const std::vector<CodeFragment>& fragments);
    
    // 优化编译顺序
    void optimizeCompilationOrder(std::vector<CodeFragment>& fragments);
    
    // 结果合并
    std::string mergeCSS(const std::vector<CompilationResult>& chtlResults);
    std::string mergeJavaScript(const std::vector<CompilationResult>& chtlJSResults);
};
```

### CHTL编译器

**职责** - 解析CHTL语法，生成HTML和CSS

**核心功能**：
- 词法分析 (Lexer)
- 语法分析 (Parser)
- AST构建 (BaseNode)
- 代码生成 (Generator)

**开发要点**：
```cpp
// 解析器核心
class CHTLParser {
    std::shared_ptr<BaseNode> parseTemplate();
    std::shared_ptr<BaseNode> parseCustom();
    std::shared_ptr<BaseNode> parseElement();
    // ... 其他解析方法
};

// 生成器核心
class CHTLGenerator {
    std::string generateHTML(std::shared_ptr<BaseNode> rootNode);
    std::string generateCSS(std::shared_ptr<BaseNode> rootNode);
    // ... 其他生成方法
};
```

### CHTL JS编译器

**职责** - 解析CHTL JS增强语法，生成JavaScript

**增强语法转换**：
- `{{.selector}}` → `document.querySelector('.selector')`
- `&-> click {}` → `addEventListener('click', function() {})`
- `animate {}` → `requestAnimationFrame(...)`
- `vir obj = listen {}` → `const obj = { ... }`

## 📦 模块系统开发

### 模块管理器 (ModuleManager)

**支持的目录结构**：
- **乱序结构** - 文件混杂在一起
- **分类结构** - CMOD/CJMOD文件夹分类

**路径搜索策略**：
1. 官方模块目录 (chtl::前缀)
2. 当前目录module文件夹
3. 当前目录

### CMOD系统

**标准格式**：
```
ModuleName/
├── src/
│   ├── ModuleName.chtl    # 主模块文件
│   └── Other.chtl         # 其他源文件
└── info/
    └── ModuleName.chtl    # 模块信息文件
```

**info文件格式**：
```chtl
[Info]
{
    name = "module-name";
    version = "1.0.0";
    description = "模块描述";
    author = "作者";
    license = "MIT";
}

[Export]
{
    [Custom] @Style StyleName1, StyleName2;
    [Custom] @Element ElementName1, ElementName2;
    [Template] @Var VarGroupName;
}
```

### CJMOD系统

**标准格式**：
```
ModuleName/
├── src/
│   ├── implementation.cpp
│   └── implementation.h
└── info/
    └── ModuleName.chtl
```

**CJMOD API使用**：
```cpp
#include "CJMODApi.h"

// 语法分析
Arg args = Syntax::analyze("$ ** $");

// 参数绑定
args.bind("$", [](const std::string& value) {
    return value;
});

// 扫描填充
Arg result = CJMODScanner::scan(args, "**");
args.fillValue(result);

// 代码转换
args.transform("pow(" + args[0].value + ", " + args[2].value + ")");

// 导出结果
CJMODGenerator::exportResult(args);
```

## 🧪 测试开发

### 单元测试
- 每个组件都应有对应的单元测试
- 测试覆盖率应达到80%以上
- 使用断言验证功能正确性

### 集成测试
- 端到端编译流程测试
- 模块系统集成测试
- 官方模块功能测试

### 性能测试
- 编译速度基准测试
- 内存使用监控
- 大型项目压力测试

## 🔍 调试技巧

### 启用调试模式
```cpp
// 在代码中启用调试
scanner.enableDebugMode(true);
dispatcher.enableDebugMode(true);
```

### 日志输出
- 使用ErrorReporter统一错误处理
- 在关键节点添加调试输出
- 记录编译时间和内存使用

### 常见问题排查
1. **编译错误** - 检查C++17支持和依赖库
2. **链接错误** - 检查ANTLR4库路径配置
3. **运行时错误** - 检查文件路径和权限
4. **性能问题** - 使用性能分析工具

## 📚 代码规范

### C++代码风格
- 使用4空格缩进
- 类名使用PascalCase
- 方法名使用camelCase
- 常量使用UPPER_CASE
- 私有成员使用m_前缀

### 文件组织
- 头文件和源文件分离
- 每个类一个文件
- 相关功能分组到目录
- 使用命名空间CHTL

### 注释规范
```cpp
/**
 * @brief 简要描述
 * @param param1 参数1描述
 * @param param2 参数2描述
 * @return 返回值描述
 */
bool functionName(const std::string& param1, int param2);
```

## 🚀 发布流程

### 版本号规范
- 使用语义化版本号 (Semantic Versioning)
- 格式：主版本.次版本.修订版本
- 例如：1.0.0, 1.1.0, 1.1.1

### 发布检查清单
- [ ] 所有测试通过
- [ ] 文档更新完整
- [ ] 版本号更新
- [ ] 更新日志编写
- [ ] 跨平台构建测试
- [ ] 性能基准测试

### 分发包内容
- 编译器可执行文件
- 官方模块文件
- VSCode扩展包
- 完整文档
- 示例项目

## 🤝 贡献指南

### 代码贡献流程
1. Fork仓库
2. 创建功能分支
3. 编写代码和测试
4. 提交Pull Request
5. 代码审查
6. 合并到主分支

### 贡献类型
- **核心功能** - 编译器核心功能改进
- **模块开发** - 新的官方模块
- **工具改进** - 构建脚本和工具
- **文档完善** - 文档翻译和改进
- **测试增强** - 测试用例和工具

---

**欢迎加入CHTL编译器的开发！让我们一起打造更好的Web开发工具！**