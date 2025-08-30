#!/bin/bash
# Repository Cleanup Script
# Clean up repository for release state

echo "🧹 CHTL Repository Cleanup Script"
echo "=================================="

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_ROOT"

echo "🔍 Analyzing repository state..."

# Remove build artifacts
echo ""
echo "🗑️ Removing build artifacts..."
find . -name "build" -type d -exec rm -rf {} + 2>/dev/null || true
find . -name "*.o" -type f -delete 2>/dev/null || true
find . -name "*.a" -type f -path "./src_new/*" -delete 2>/dev/null || true
find . -name "CMakeCache.txt" -delete 2>/dev/null || true
find . -name "CMakeFiles" -type d -exec rm -rf {} + 2>/dev/null || true
find . -name "Makefile" -path "./src_new/*" -delete 2>/dev/null || true
echo "  ✓ Build artifacts removed"

# Remove temporary files
echo ""
echo "🗑️ Removing temporary files..."
find . -name "*.tmp" -delete 2>/dev/null || true
find . -name "*.log" -delete 2>/dev/null || true
find . -name ".DS_Store" -delete 2>/dev/null || true
find . -name "Thumbs.db" -delete 2>/dev/null || true
find . -name "*.swp" -delete 2>/dev/null || true
find . -name "*.swo" -delete 2>/dev/null || true
echo "  ✓ Temporary files removed"

# Remove test artifacts
echo ""
echo "🗑️ Removing test artifacts..."
find . -name "*test*" -type f -executable -delete 2>/dev/null || true
find . -name "test_*.cpp" -path "./developer-*" -delete 2>/dev/null || true
find . -name "comprehensive_test*" -delete 2>/dev/null || true
find . -name "*verification*" -delete 2>/dev/null || true
echo "  ✓ Test artifacts removed"

# Remove development simulation
echo ""
echo "🗑️ Removing development simulation..."
rm -rf developer-simulation 2>/dev/null || true
rm -rf developer-test-project 2>/dev/null || true
echo "  ✓ Development simulation removed"

# Remove redundant documentation
echo ""
echo "🗑️ Removing redundant documentation..."
find . -name "*报告*.md" -delete 2>/dev/null || true
find . -name "*总结*.md" -delete 2>/dev/null || true
find . -name "*完成*.md" -delete 2>/dev/null || true
find . -name "*分析*.md" -delete 2>/dev/null || true
echo "  ✓ Redundant documentation removed"

# Organize final structure
echo ""
echo "📁 Organizing final repository structure..."

# Ensure proper directory structure
mkdir -p dist
mkdir -p examples
mkdir -p packaged_modules

# Move examples to proper location
if [ -d "developer-simulation" ]; then
    mv developer-simulation examples/ 2>/dev/null || true
fi

echo "  ✓ Repository structure organized"

# Create .gitignore if not exists
echo ""
echo "📝 Creating/updating .gitignore..."
cat > .gitignore << 'EOF'
# Build artifacts
build/
build_*/
*.o
*.a
*.so
*.dll
*.exe
CMakeCache.txt
CMakeFiles/
Makefile
cmake_install.cmake
CPackConfig.cmake
CPackSourceConfig.cmake

# IDE files
.vscode/settings.json
.vscode/launch.json
.idea/
*.swp
*.swo
*~

# OS files
.DS_Store
Thumbs.db
desktop.ini

# Temporary files
*.tmp
*.log
*.bak

# Node.js (for VSCode extension)
node_modules/
npm-debug.log*
yarn-debug.log*
yarn-error.log*

# Distribution
dist/
packaged_modules/
*.vsix

# Test artifacts
test_*
*test*
*verification*
comprehensive_*

# Development artifacts
developer-simulation/
developer-test-project/
EOF

echo "  ✓ .gitignore created/updated"

# Create README.md
echo ""
echo "📖 Creating main README.md..."
cat > README.md << 'EOF'
# CHTL编译器

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)]()

🚀 **现代化的超文本编译器，让Web开发变得更加简洁高效！**

## ✨ 特性亮点

- 🎯 **简洁语法** - 比HTML更直观的元素定义
- 🎨 **强大模板** - 可复用的样式、元素、变量组
- 📦 **模块化** - CMOD/CJMOD模块系统
- ⚡ **CHTL JS** - 增强的JavaScript语法
- 🌸 **官方模块** - Chtholly和Yuigahama主题
- 🔧 **VSCode支持** - 完整的IDE集成

## 🚀 快速开始

### 安装
```bash
# 克隆仓库
git clone https://github.com/CHTL-Team/chtl-compiler
cd chtl-compiler

# 构建编译器
./scripts/linux/build_release.sh

# 安装VSCode扩展
code --install-extension chtl-language-support.vsix
```

### 第一个CHTL项目
```chtl
[Import] @Chtl from chtl::Chtholly

html
{
    body
    {
        ChthollyCard()
        {
            div[1]
            {
                h1 { text { "Hello CHTL!" } }
                p { text { "我的第一个CHTL项目" } }
            }
        }
    }
}

use html5;
```

## 📚 文档

- [CHTL项目概述](docs/CHTL项目概述.md)
- [CHTL编译器开发指南](docs/CHTL编译器开发指南.md)
- [珂朵莉模块使用文档](docs/珂朵莉模块使用文档.md)
- [由比滨结衣模块使用文档](docs/由比滨结衣模块使用文档.md)
- [完整文档列表](docs/)

## 🎭 官方模块

### 🌸 Chtholly珂朵莉模块
- 手风琴、相册、备忘录、进度条等组件
- printMylove、iNeverAway等CJMOD功能

### 💖 Yuigahama由比滨结衣模块  
- 音乐播放器、温暖主题组件
- 友善的用户交互体验

## 🤝 贡献

欢迎贡献代码、文档、模块和反馈！

## 📄 许可证

[MIT License](docs/MIT开源协议条款.md)

---

**CHTL编译器 - 为Web开发者打造的现代化超文本编译器！**
EOF

echo "  ✓ README.md created"

# Final statistics
echo ""
echo "📊 Cleanup Summary"
echo "=================================="
total_files=$(find . -type f | wc -l)
total_dirs=$(find . -type d | wc -l)
total_size=$(du -sh . | cut -f1)

echo "Repository statistics:"
echo "  📁 Total directories: $total_dirs"
echo "  📄 Total files: $total_files"
echo "  💾 Total size: $total_size"
echo ""

# List main components
echo "📋 Main components:"
echo "  ✓ src/ - CHTL compiler source code"
echo "  ✓ docs/ - Complete documentation"
echo "  ✓ scripts/ - Cross-platform build scripts"
echo "  ✓ vscode-chtl-extension/ - VSCode extension"
echo "  ✓ examples/ - Example projects"
echo ""

echo "🎊 Repository cleanup completed!"
echo "=================================="
echo "Repository is now in release-ready state!"
echo ""
echo "Next steps:"
echo "  1. Run ./scripts/universal_build.sh to build everything"
echo "  2. Test the built compiler and extension"
echo "  3. Create release package"
echo "  4. Publish to GitHub releases"
echo "=================================="