# CHTL Windows环境使用指南

## 概述

本指南专门针对Windows用户，详细介绍了如何在Windows环境中设置、构建和使用CHTL项目。CHTL为Windows用户提供了完整的PowerShell脚本支持，让开发过程更加便捷。

## 🚀 **快速开始 (Windows)**

### **第一步：系统要求检查**

确保您的Windows系统满足以下要求：

- **Windows 10** (1903+) 或 **Windows 11**
- **PowerShell 5.0+** (Windows 10自带)
- **管理员权限** (用于安装依赖)
- **至少8GB内存** (推荐16GB+)
- **至少10GB可用磁盘空间**

### **第二步：一键环境设置**

```powershell
# 1. 以管理员身份运行PowerShell
# 右键点击"开始"按钮 -> "Windows PowerShell (管理员)"

# 2. 克隆项目
git clone https://github.com/CHTL-Team/CHTL.git
cd CHTL

# 3. 检查当前环境
.\scripts\check_environment.ps1

# 4. 自动安装依赖 (如果需要)
.\scripts\install_dependencies.ps1 -All

# 5. 验证安装
.\scripts\check_environment.ps1 -Detailed
```

### **第三步：构建CHTL**

```powershell
# Release构建 (推荐用于日常使用)
.\scripts\build_release.ps1

# 或者Debug构建 (用于开发调试)
.\scripts\build_debug.ps1

# 构建完成后测试
cd build_release
.\src\Release\chtl.exe --version
```

## 🛠️ **开发环境选择**

### **选项1：Visual Studio (推荐)**

**优势**：
- 最佳的Windows C++开发体验
- 优秀的调试器和分析工具
- IntelliSense代码补全
- 集成的包管理器(vcpkg)

**安装**：
```powershell
# 使用Chocolatey安装
choco install visualstudio2022community --params "--add Microsoft.VisualStudio.Workload.NativeDesktop"

# 或手动下载
# https://visualstudio.microsoft.com/vs/community/
```

**使用**：
```powershell
# 生成Visual Studio项目
.\scripts\build_debug.ps1 -Generator "Visual Studio 17 2022"

# 在Visual Studio中打开
cd build_debug
start CHTL.sln
```

### **选项2：MinGW-w64**

**优势**：
- 轻量级，安装快速
- 与Linux环境更相似
- 支持最新C++标准

**安装**：
```powershell
# 使用Chocolatey
choco install mingw

# 或使用MSYS2
choco install msys2
```

**使用**：
```powershell
# 使用MinGW构建
.\scripts\build_release.ps1 -Generator "MinGW Makefiles"
```

### **选项3：WSL (Windows Subsystem for Linux)**

**优势**：
- 完整的Linux环境
- 可以使用Linux版构建脚本
- 适合熟悉Linux的开发者

**设置**：
```powershell
# 启用WSL
wsl --install

# 在WSL中使用Linux构建脚本
wsl
cd /mnt/c/path/to/CHTL
./scripts/build_release.sh
```

## 📜 **PowerShell脚本详解**

### **🔍 check_environment.ps1**

检查Windows开发环境是否配置正确：

```powershell
# 基础检查
.\scripts\check_environment.ps1

# 详细信息检查
.\scripts\check_environment.ps1 -Detailed
```

**检查项目**：
- PowerShell版本
- Windows版本和架构
- Visual Studio / MinGW编译器
- CMake版本
- Git可用性
- 系统资源(内存、磁盘空间)
- 可选调试工具

### **⚙️ install_dependencies.ps1**

自动安装CHTL开发所需的依赖：

```powershell
# 安装所有依赖 (推荐)
.\scripts\install_dependencies.ps1 -All

# 仅使用Chocolatey安装
.\scripts\install_dependencies.ps1 -Chocolatey

# 安装C++库 (vcpkg)
.\scripts\install_dependencies.ps1 -Vcpkg

# 显示手动安装说明
.\scripts\install_dependencies.ps1 -Manual

# 跳过Visual Studio安装
.\scripts\install_dependencies.ps1 -All -SkipVS
```

**安装的组件**：
- Chocolatey包管理器
- CMake构建系统
- Git版本控制
- MinGW编译器 (可选)
- Visual Studio Community (可选)
- Visual Studio Code (可选)
- vcpkg包管理器
- Google Test测试框架
- ANTLR4运行时库

### **🏗️ build_release.ps1**

构建优化的Release版本：

```powershell
# 基础使用
.\scripts\build_release.ps1

# 指定Visual Studio版本
.\scripts\build_release.ps1 -Generator "Visual Studio 17 2022"

# 使用MinGW
.\scripts\build_release.ps1 -Generator "MinGW Makefiles"

# 自定义构建目录
.\scripts\build_release.ps1 -BuildDir "my_build"

# 显示帮助
.\scripts\build_release.ps1 -Help
```

**特性**：
- 自动检测可用编译器
- 支持多种CMake生成器
- Windows特定优化(`/O2`, `/MP`)
- 自动运行基础测试
- 生成详细构建报告

### **🐛 build_debug.ps1**

构建带调试信息的Debug版本：

```powershell
# 基础调试构建
.\scripts\build_debug.ps1

# 启用AddressSanitizer (VS 2019+)
.\scripts\build_debug.ps1 -EnableSanitizers

# 启用代码覆盖率
.\scripts\build_debug.ps1 -EnableCoverage

# 使用所有调试功能
.\scripts\build_debug.ps1 -EnableSanitizers -EnableCoverage
```

**生成的调试工具**：
- `debug_with_vs.bat` - Visual Studio调试
- `debug_with_cdb.bat` - 控制台调试器
- `debug_session.ps1` - PowerShell调试助手
- `enable_app_verifier.bat` - 应用程序验证器

### **🧹 clean_build.ps1**

清理构建文件和临时文件：

```powershell
# 清理所有内容
.\scripts\clean_build.ps1 -All

# 仅清理构建目录
.\scripts\clean_build.ps1 -Build

# 清理临时文件
.\scripts\clean_build.ps1 -Temp

# 预览清理内容 (不实际删除)
.\scripts\clean_build.ps1 -All -DryRun

# 显示帮助
.\scripts\clean_build.ps1 -Help
```

**清理类别**：
- **构建目录**: `build*`, `bin`, `lib`, `obj`, `.vs`
- **临时文件**: `*.tmp`, `*.log`, `*.pdb`, `*.ilk`
- **包文件**: `*.cmod`, `*.cjmod`, `*.chtlpkg`
- **CMake缓存**: `CMakeCache.txt`, `CMakeFiles`
- **IDE文件**: `.vscode`, `.idea`, `*.sln`, `*.vcxproj`

## 🔧 **常见问题解决**

### **问题1：PowerShell执行策略限制**

**错误信息**：
```
.\scripts\build_release.ps1 : 无法加载文件，因为在此系统上禁止运行脚本
```

**解决方案**：
```powershell
# 以管理员身份运行PowerShell，然后执行：
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser

# 或者临时允许：
Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process
```

### **问题2：Visual Studio找不到**

**错误信息**：
```
✗ No suitable C++ compiler found
```

**解决方案**：
```powershell
# 检查Visual Studio安装
.\scripts\check_environment.ps1 -Detailed

# 安装Visual Studio
.\scripts\install_dependencies.ps1 -Chocolatey

# 或手动安装Visual Studio Community
# https://visualstudio.microsoft.com/vs/community/
```

### **问题3：CMake版本过旧**

**错误信息**：
```
CMake Error: CMake 3.15 or higher is required
```

**解决方案**：
```powershell
# 更新CMake
choco upgrade cmake

# 或手动下载最新版本
# https://cmake.org/download/
```

### **问题4：构建过程中内存不足**

**错误信息**：
```
fatal error C1060: compiler is out of heap space
```

**解决方案**：
```powershell
# 减少并行编译进程
.\scripts\build_release.ps1 -Generator "Visual Studio 16 2019"
# 然后手动构建时限制并行度：
cd build_release
cmake --build . --config Release --parallel 2  # 仅使用2个线程
```

### **问题5：ANTLR4库找不到**

**错误信息**：
```
Could NOT find ANTLR4 (missing: ANTLR4_LIBRARIES ANTLR4_INCLUDE_DIRS)
```

**解决方案**：
```powershell
# 禁用ANTLR4 (可选功能)
.\scripts\build_release.ps1
# CMake会自动跳过ANTLR4

# 或安装ANTLR4
.\scripts\install_dependencies.ps1 -Vcpkg
```

## 🎯 **高级使用技巧**

### **技巧1：使用vcpkg管理C++库**

```powershell
# 安装vcpkg
.\scripts\install_dependencies.ps1 -Vcpkg

# 集成到Visual Studio
C:\vcpkg\vcpkg.exe integrate install

# 在CMake中使用
.\scripts\build_release.ps1 -Generator "Visual Studio 17 2022"
```

### **技巧2：自定义构建配置**

```powershell
# 创建自定义配置文件
$env:CHTL_CUSTOM_FLAGS = "/O2 /GL /LTCG"  # 自定义编译器标志
$env:CHTL_BUILD_TYPE = "RelWithDebInfo"   # 自定义构建类型

.\scripts\build_release.ps1
```

### **技巧3：使用Windows性能工具**

```powershell
# 安装Windows Performance Toolkit
choco install windows-adk-winpe

# 使用ETW进行性能分析
# (需要专业知识)
```

### **技巧4：集成CI/CD**

```yaml
# GitHub Actions workflow for Windows
name: Windows Build
on: [push, pull_request]

jobs:
  build:
    runs-on: windows-latest
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup environment
      run: .\scripts\check_environment.ps1
      shell: powershell
    
    - name: Install dependencies
      run: .\scripts\install_dependencies.ps1 -Chocolatey
      shell: powershell
    
    - name: Build Release
      run: .\scripts\build_release.ps1
      shell: powershell
    
    - name: Run tests
      run: cd build_release && ctest
      shell: powershell
```

## 📚 **Windows特定资源**

### **学习资源**

- [Visual Studio C++文档](https://docs.microsoft.com/cpp/)
- [PowerShell学习路径](https://docs.microsoft.com/powershell/)
- [Windows开发者中心](https://developer.microsoft.com/windows/)
- [vcpkg文档](https://vcpkg.io/)

### **调试工具**

- **Visual Studio Debugger**: 最强大的Windows调试器
- **Windows Debugger (WinDbg)**: 内核级调试
- **Application Verifier**: 堆损坏检测
- **Performance Toolkit**: 性能分析
- **Intel Inspector**: 内存和线程错误检测

### **开发工具推荐**

- **Visual Studio Code**: 轻量级代码编辑器
- **Visual Studio Community**: 完整的IDE
- **Windows Terminal**: 现代终端应用
- **PowerToys**: Windows增强工具
- **Git for Windows**: Git客户端

## 🆘 **获取帮助**

如果遇到Windows特定的问题：

1. **查看脚本帮助**：
   ```powershell
   .\scripts\build_release.ps1 -Help
   .\scripts\check_environment.ps1 -Help
   ```

2. **运行详细诊断**：
   ```powershell
   .\scripts\check_environment.ps1 -Detailed
   ```

3. **查看日志文件**：
   - 构建日志在构建目录中
   - Windows事件查看器
   - Visual Studio输出窗口

4. **社区支持**：
   - CHTL GitHub Issues (标注Windows)
   - Stack Overflow (标签: chtl, windows, cmake)
   - CHTL社区论坛

---

通过遵循本指南，Windows用户将能够轻松设置和使用CHTL开发环境，享受与Linux/macOS用户相同的优秀开发体验！🚀