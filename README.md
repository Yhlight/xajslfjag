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
