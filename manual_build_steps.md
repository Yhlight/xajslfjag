# CHTL Windows手动构建步骤

## 方法1: 使用修正的批处理脚本 (推荐)

```cmd
# 直接运行修正版脚本
build_windows_correct.bat
```

## 方法2: 手动步骤

### 步骤1: 替换配置文件
```cmd
# 备份原始配置
copy CMakeLists.txt CMakeLists_backup.txt

# 使用Windows专用配置
copy CMakeLists_Windows_Fixed.txt CMakeLists.txt
```

### 步骤2: 创建构建目录
```cmd
mkdir build_windows
cd build_windows
```

### 步骤3: 配置CMake
```cmd
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
```

### 步骤4: 构建项目
```cmd
cmake --build . --config Release
```

### 步骤5: 恢复原始配置 (可选)
```cmd
cd ..
copy CMakeLists_backup.txt CMakeLists.txt
```

## 方法3: 如果仍有问题，使用核心构建

如果ANTLR相关文件有问题，可以构建不含ANTLR的核心版本：

### 创建简化的CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.16)
project(CHTL_Core VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 只包含核心CHTL文件
set(CHTL_CORE_SOURCES
    src/CHTL/CHTLLexer/Token.cpp
    src/CHTL/CHTLLexer/GlobalMap.cpp
    src/CHTL/CHTLLexer/Lexer.cpp
    src/CHTL/CHTLParser/Parser.cpp
    src/CHTL/CHTLParser/SafetyMethods.cpp
    src/CHTL/CHTLNode/BaseNode.cpp
    src/CHTL/CHTLNode/TemplateNode.cpp
    src/CHTL/CHTLNode/CustomNode.cpp
    src/CHTL/CHTLNode/OriginNode.cpp
    src/CHTL/CHTLNode/ConstraintNode.cpp
    src/CHTL/CHTLNode/SpecializationNode.cpp
    src/CHTL/CHTLNode/ImportNode.cpp
    src/CHTL/CHTLGenerator/Generator.cpp
    src/CHTL/CHTLGenerator/GeneratorMethods.cpp
    src/CHTL/CHTLContext/Context.cpp
    src/CHTL/CHTLConstraint/ConstraintSystem.cpp
    src/CHTL/CHTLNamespace/NamespaceManager.cpp
    src/CHTL/CHTLSelector/SelectorAutomation.cpp
    src/CHTLJS/CHTLJSLexer/Token.cpp
    src/CHTLJS/CHTLJSLexer/Lexer.cpp
    src/CHTLJS/CHTLJSParser/EnhancedParser.cpp
    src/CMODSystem/Syntax.cpp
    src/CMODSystem/CJMODGenerator.cpp
    src/Scanner/UnifiedScanner.cpp
    src/Util/FileSystem/FileSystem.cpp
    src/Error/ErrorManager.cpp
)

add_library(chtl_core STATIC ${CHTL_CORE_SOURCES})

target_include_directories(chtl_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(chtl_core PUBLIC cxx_std_17)
```

保存为 `CMakeLists_Core.txt`，然后：

```cmd
copy CMakeLists_Core.txt CMakeLists.txt
mkdir build_core
cd build_core
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

## 测试CHTL功能

构建完成后，编译测试程序：

```cmd
# 返回项目根目录
cd ..

# 编译测试程序
cl windows_test_fixed.cpp /I"src" /std:c++17 /EHsc build_windows\Release\chtl_core.lib

# 运行测试
windows_test_fixed.exe
```

## 预期输出

如果一切正常，你将看到：

```
=== CHTL Windows环境完整测试 ===

=== CJMOD功能专项测试 ===
【测试1】CJMOD基础功能
   ✓ CJMOD::Syntax初始化 成功

【测试2】AtomArg参数系统
   ✓ 字符串参数创建和赋值 成功
   ✓ 数字参数创建和赋值 成功
   ✓ 布尔参数创建和赋值 成功

... 更多测试输出 ...

🎉 所有测试通过！CHTL Windows环境完全可用
```

这样就能确认CJMOD和SPA功能都正常工作了！