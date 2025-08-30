# CMOD开发指南

## 概述

CMOD (CHTL Module) 是CHTL提供的模块化开发系统，允许开发者创建可复用的CHTL组件、模板和样式。本指南详细介绍了如何开发、打包和发布CMOD模块。

## CMOD基础概念

### 🧩 **什么是CMOD**

CMOD是CHTL的模块打包格式，包含：
- **CHTL组件**: 可复用的UI组件
- **模板系统**: 样式组、元素组、变量组模板
- **配置文件**: 模块信息和导出定义
- **依赖管理**: 模块间的依赖关系

### 📦 **CMOD文件结构**

```
ModuleName/
├── src/                    # 源代码目录
│   ├── ModuleName.chtl    # 主模块文件 (可选)
│   ├── Component1.chtl    # 子组件
│   ├── Component2.chtl    # 子组件
│   └── SubModule/         # 子模块 (可选)
│       ├── src/
│       │   └── SubModule.chtl
│       └── info/
│           └── SubModule.chtl
└── info/                  # 模块信息目录
    └── ModuleName.chtl    # 模块信息文件
```

## 创建CMOD模块

### 🛠️ **初始化新模块**

```bash
# 使用CHTL工具创建新模块
chtl create-module MyUIKit

# 或手动创建目录结构
mkdir MyUIKit
mkdir MyUIKit/src
mkdir MyUIKit/info
```

### 📝 **模块信息文件**

每个CMOD都必须包含模块信息文件 `info/ModuleName.chtl`：

```chtl
// info/MyUIKit.chtl
[Info]
{
    name = "MyUIKit";
    version = "1.0.0";
    description = "现代化UI组件库 - 提供完整的Web组件解决方案";
    author = "Your Name";
    license = "MIT";
    dependencies = "CHTL.Core@1.0.0, CHTL.Icons@0.5.0";
    category = "ui-components";
    minCHTLVersion = "1.0.0";
    maxCHTLVersion = "2.0.0";
    homepage = "https://github.com/yourname/myuikit";
    repository = "https://github.com/yourname/myuikit.git";
    keywords = "ui, components, design-system, chtl";
}

// 外部查询表 (系统生成，通常不需要手动编写)
[Export]
{
    [Custom] @Style ButtonStyle, CardStyle, ModalStyle;
    [Custom] @Element Button, Card, Modal, Navigation;
    [Custom] @Var Colors, Typography, Spacing;
    [Template] @Style ResponsiveGrid, FlexLayout;
    [Template] @Element Container, Section;
    [Configuration] @Config ThemeConfig, LayoutConfig;
}
```

### 🎨 **创建组件**

#### 样式组件
```chtl
// src/ButtonStyle.chtl
[Custom] @Style ButtonStyle {
    .btn {
        display: inline-block;
        padding: 12px 24px;
        border: none;
        border-radius: 6px;
        font-size: 14px;
        font-weight: 500;
        text-align: center;
        text-decoration: none;
        cursor: pointer;
        transition: all 0.3s ease;
        
        &:hover {
            transform: translateY(-2px);
            box-shadow: 0 4px 8px rgba(0,0,0,0.2);
        }
        
        &.primary {
            background-color: #007bff;
            color: white;
            
            &:hover {
                background-color: #0056b3;
            }
        }
        
        &.secondary {
            background-color: #6c757d;
            color: white;
            
            &:hover {
                background-color: #545b62;
            }
        }
        
        &.danger {
            background-color: #dc3545;
            color: white;
            
            &:hover {
                background-color: #c82333;
            }
        }
        
        &:disabled {
            opacity: 0.6;
            cursor: not-allowed;
            transform: none;
            box-shadow: none;
        }
    }
}
```

#### 元素组件
```chtl
// src/Button.chtl
[Custom] @Element Button {
    // 使用样式组
    use ButtonStyle;
    
    // 默认属性
    [Default] {
        type = "button";
        class = "btn primary";
    }
    
    // 特例化操作
    delete type when role="link";
    insert data-component="button";
    
    // 条件样式
    [Condition] {
        if (disabled) {
            class += " disabled";
            aria-disabled = "true";
        }
        
        if (size) {
            class += " btn-" + size;
        }
    }
}
```

#### 变量组
```chtl
// src/Colors.chtl
[Custom] @Var Colors {
    // 主色彩
    primary: #007bff;
    secondary: #6c757d;
    success: #28a745;
    warning: #ffc107;
    danger: #dc3545;
    info: #17a2b8;
    light: #f8f9fa;
    dark: #343a40;
    
    // 灰度色彩
    gray-100: #f8f9fa;
    gray-200: #e9ecef;
    gray-300: #dee2e6;
    gray-400: #ced4da;
    gray-500: #adb5bd;
    gray-600: #6c757d;
    gray-700: #495057;
    gray-800: #343a40;
    gray-900: #212529;
    
    // 语义化颜色
    text-primary: #{primary};
    text-secondary: #{gray-600};
    text-muted: #{gray-500};
    
    // 背景颜色
    bg-primary: #{primary};
    bg-secondary: #{secondary};
    bg-light: #{light};
    bg-dark: #{dark};
}
```

#### 复杂组件示例
```chtl
// src/Modal.chtl
[Custom] @Element Modal {
    use ModalStyle;
    
    [Default] {
        class = "modal";
        role = "dialog";
        aria-hidden = "true";
        tabindex = "-1";
    }
    
    [Structure] {
        div.modal-overlay {
            div.modal-container {
                div.modal-header {
                    h3.modal-title { {title} }
                    button.modal-close[aria-label="关闭"] {
                        span[aria-hidden="true"] { "×" }
                    }
                }
                
                div.modal-body {
                    {content}
                }
                
                div.modal-footer when {hasFooter} {
                    {footer}
                }
            }
        }
    }
    
    [Script] {
        listen modalController = {
            show: () => {
                this.classList.add('show');
                this.setAttribute('aria-hidden', 'false');
                document.body.classList.add('modal-open');
            },
            
            hide: () => {
                this.classList.remove('show');
                this.setAttribute('aria-hidden', 'true');
                document.body.classList.remove('modal-open');
            },
            
            toggle: () => {
                this.classList.contains('show') ? 
                    this.modalController.hide() : 
                    this.modalController.show();
            }
        };
        
        // 事件绑定
        {{.modal-close}} &-> click {
            modalController->hide();
        };
        
        {{.modal-overlay}} &-> click {
            if (event.target === this) {
                modalController->hide();
            }
        };
        
        // 键盘事件
        {{document}} &-> keydown {
            if (event.key === 'Escape' && this.classList.contains('show')) {
                modalController->hide();
            }
        };
    }
}
```

### 🎭 **模板系统**

#### 创建模板
```chtl
// src/GridTemplate.chtl
[Template] @Style GridTemplate {
    .grid-container {
        display: grid;
        gap: var(--grid-gap, 20px);
        
        &.cols-1 { grid-template-columns: 1fr; }
        &.cols-2 { grid-template-columns: repeat(2, 1fr); }
        &.cols-3 { grid-template-columns: repeat(3, 1fr); }
        &.cols-4 { grid-template-columns: repeat(4, 1fr); }
        &.cols-auto { grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); }
        
        @media (max-width: 768px) {
            &.cols-2, &.cols-3, &.cols-4 {
                grid-template-columns: 1fr;
            }
        }
    }
    
    .grid-item {
        &.span-2 { grid-column: span 2; }
        &.span-3 { grid-column: span 3; }
        &.span-all { grid-column: 1 / -1; }
    }
}

[Template] @Element GridContainer {
    use GridTemplate;
    
    [Default] {
        class = "grid-container cols-3";
    }
    
    [Parameters] {
        columns?: number = 3;
        gap?: string = "20px";
        responsive?: boolean = true;
    }
    
    [Render] {
        div.grid-container[
            class="cols-{columns}"
            style="--grid-gap: {gap}"
        ] {
            {children}
        }
    }
}
```

#### 使用模板
```chtl
// 在其他模块中使用模板
use GridTemplate from "MyUIKit";

div {
    include GridContainer(columns: 4, gap: "30px") {
        div.grid-item { "Item 1" }
        div.grid-item.span-2 { "Item 2 (spans 2 columns)" }
        div.grid-item { "Item 3" }
    }
}
```

## 高级特性

### 🔧 **配置系统**

```chtl
// src/ThemeConfig.chtl
[Configuration] @Config ThemeConfig {
    [Schema] {
        primaryColor: Color = "#007bff";
        secondaryColor: Color = "#6c757d";
        fontFamily: String = "system-ui, sans-serif";
        borderRadius: Number = 6;
        spacing: {
            small: Number = 8;
            medium: Number = 16;
            large: Number = 24;
        };
        breakpoints: {
            mobile: Number = 576;
            tablet: Number = 768;
            desktop: Number = 992;
            wide: Number = 1200;
        };
    }
    
    [Validation] {
        primaryColor: /^#[0-9A-Fa-f]{6}$/;
        borderRadius: value >= 0 && value <= 50;
        spacing.small: value > 0;
        spacing.medium: value > spacing.small;
        spacing.large: value > spacing.medium;
    }
    
    [Default] {
        // 使用默认值
    }
}
```

### 🎯 **条件编译**

```chtl
// src/ResponsiveComponents.chtl
[Custom] @Element ResponsiveCard {
    [Condition] {
        if (CHTL.config.target === "mobile") {
            use MobileCardStyle;
        } else if (CHTL.config.target === "desktop") {
            use DesktopCardStyle;
        } else {
            use ResponsiveCardStyle;
        }
    }
    
    [Feature-Detection] {
        if (supports("display: grid")) {
            use GridCardLayout;
        } else {
            use FlexCardLayout;
        }
    }
}
```

### 🧪 **测试支持**

```chtl
// src/Button.test.chtl
[Test] @Component Button {
    [TestCase] "默认按钮渲染" {
        [Given] {
            Button { "Click me" }
        }
        
        [Then] {
            should.exist(".btn");
            should.contain.text("Click me");
            should.have.attribute("type", "button");
        }
    }
    
    [TestCase] "禁用状态" {
        [Given] {
            Button[disabled] { "Disabled" }
        }
        
        [Then] {
            should.have.class("disabled");
            should.have.attribute("aria-disabled", "true");
        }
    }
    
    [TestCase] "点击事件" {
        [Given] {
            Button[onClick=mockHandler] { "Test" }
        }
        
        [When] {
            click(".btn");
        }
        
        [Then] {
            should.have.called(mockHandler);
        }
    }
}
```

## 子模块系统

### 📁 **创建子模块**

```
MyUIKit/
├── src/
│   ├── MyUIKit.chtl      # 主模块 (可选)
│   ├── Icons/            # 子模块
│   │   ├── src/
│   │   │   ├── IconFont.chtl
│   │   │   ├── SvgIcons.chtl
│   │   │   └── IconButton.chtl
│   │   └── info/
│   │       └── Icons.chtl
│   ├── Forms/            # 子模块
│   │   ├── src/
│   │   │   ├── Input.chtl
│   │   │   ├── Select.chtl
│   │   │   └── Checkbox.chtl
│   │   └── info/
│   │       └── Forms.chtl
│   └── Layout/           # 子模块
│       ├── src/
│       │   ├── Container.chtl
│       │   ├── Grid.chtl
│       │   └── Flex.chtl
│       └── info/
│           └── Layout.chtl
└── info/
    └── MyUIKit.chtl
```

### 🔗 **子模块信息**

```chtl
// src/Icons/info/Icons.chtl
[Info]
{
    name = "Icons";
    version = "1.0.0";
    description = "图标组件子模块";
    author = "Your Name";
    license = "MIT";
    parent = "MyUIKit";
    dependencies = "";
}

[Export]
{
    [Custom] @Style IconStyle, IconButtonStyle;
    [Custom] @Element Icon, IconButton;
    [Custom] @Var IconSizes, IconColors;
}
```

### 📥 **导入和使用**

```chtl
// 导入整个模块
Import @Chtl from MyUIKit;

// 导入特定子模块
Import @Chtl from MyUIKit.Icons;
Import @Chtl from MyUIKit.Forms;

// 精确导入
Import @Chtl {
    Button, Card from MyUIKit,
    Icon, IconButton from MyUIKit.Icons,
    Input, Select from MyUIKit.Forms
};

// 使用别名
Import @Chtl from MyUIKit.Icons as UIIcons;
```

## 构建和打包

### 📦 **使用构建脚本**

```bash
# 验证模块结构
chtl validate MyUIKit

# 构建CMOD包
./scripts/package_cmod.sh MyUIKit output/

# 输出文件:
# output/MyUIKit-1.0.0.cmod
# output/MyUIKit-1.0.0.cmod.info
```

### 🔍 **手动验证**

```bash
# 检查必需文件
ls MyUIKit/src/
ls MyUIKit/info/

# 验证信息文件语法
chtl check-syntax MyUIKit/info/MyUIKit.chtl

# 检查依赖
chtl check-dependencies MyUIKit
```

### 📋 **打包清单**

打包前请确认：

- [ ] 模块信息文件完整且正确
- [ ] 所有源文件语法正确
- [ ] 依赖关系明确
- [ ] 版本号符合语义化版本规范
- [ ] 许可证信息正确
- [ ] 文档和示例完整

## 发布和分发

### 🌐 **发布到仓库**

```bash
# 发布到CHTL官方仓库
chtl publish MyUIKit-1.0.0.cmod --registry=official

# 发布到私有仓库
chtl publish MyUIKit-1.0.0.cmod --registry=https://myregistry.com

# 发布到GitHub Packages
chtl publish MyUIKit-1.0.0.cmod --registry=github
```

### 📄 **创建文档**

```markdown
# MyUIKit - 现代化UI组件库

## 安装

将 `MyUIKit-1.0.0.cmod` 文件放置到项目的 `module` 目录中。

## 使用

\```chtl
Import @Chtl from MyUIKit;

div {
    Button[type="primary"] { "主要按钮" }
    Card {
        "卡片内容"
    }
    Modal[title="确认"][show=true] {
        "是否确认此操作？"
    }
}
\```

## 组件列表

### 按钮组件
- `Button`: 基础按钮
- `IconButton`: 图标按钮

### 布局组件
- `Container`: 容器
- `Grid`: 网格布局
- `Card`: 卡片

### 表单组件
- `Input`: 输入框
- `Select`: 下拉选择
- `Checkbox`: 复选框
```

### 🔄 **版本管理**

```chtl
// 版本更新示例
[Info]
{
    name = "MyUIKit";
    version = "1.1.0";  // 新增功能
    // version = "1.0.1";  // Bug修复
    // version = "2.0.0";  // 破坏性更新
    
    changelog = "
        v1.1.0:
        - 新增 Modal 组件
        - 改进 Button 组件样式
        - 修复 Grid 响应式问题
        
        v1.0.1:
        - 修复 Button 禁用状态样式
        
        v1.0.0:
        - 初始版本发布
    ";
}
```

## 最佳实践

### ✅ **设计原则**

1. **组件化**: 将功能拆分为独立的、可复用的组件
2. **一致性**: 保持命名规范和代码风格一致
3. **可配置性**: 提供合理的配置选项和默认值
4. **可访问性**: 遵循Web可访问性标准
5. **性能优先**: 优化组件性能和包体积

### 🎯 **命名规范**

```chtl
// 推荐的命名规范

// 组件名：大驼峰 + 描述性
[Custom] @Element PrimaryButton { }
[Custom] @Element NavigationCard { }

// 样式名：描述性 + Style后缀
[Custom] @Style ButtonStyle { }
[Custom] @Style CardLayoutStyle { }

// 变量组：描述性 + 复数形式
[Custom] @Var Colors { }
[Custom] @Var Spacings { }
[Custom] @Var Breakpoints { }

// 配置：描述性 + Config后缀
[Configuration] @Config ThemeConfig { }
[Configuration] @Config LayoutConfig { }
```

### 🚫 **避免的反模式**

```chtl
// ❌ 避免：单体组件
[Custom] @Element MegaComponent {
    // 包含太多功能的大组件
}

// ✅ 推荐：拆分为小组件
[Custom] @Element Header { }
[Custom] @Element Navigation { }
[Custom] @Element Sidebar { }

// ❌ 避免：硬编码值
[Custom] @Style BadStyle {
    .element {
        color: #007bff;  // 硬编码
        padding: 20px;   // 硬编码
    }
}

// ✅ 推荐：使用变量
[Custom] @Style GoodStyle {
    use Colors, Spacings;
    
    .element {
        color: #{Colors.primary};
        padding: #{Spacings.medium};
    }
}
```

### 📊 **性能优化**

```chtl
// 延迟加载大组件
[Custom] @Element HeavyComponent {
    [Loading] {
        strategy = "lazy";
        placeholder = LoadingSpinner;
    }
}

// 条件样式加载
[Custom] @Style ConditionalStyle {
    [Condition] {
        if (CHTL.config.theme === "dark") {
            load "dark-theme.css";
        }
    }
}

// 压缩和优化
[Build] {
    minify = true;
    optimize = true;
    tree-shaking = true;
}
```

## 调试和测试

### 🐛 **调试技巧**

```bash
# 启用详细日志
chtl build MyUIKit --verbose --debug

# 检查生成的代码
chtl preview MyUIKit Button

# 分析依赖关系
chtl analyze MyUIKit --dependencies

# 性能分析
chtl profile MyUIKit --metrics
```

### 🧪 **测试策略**

```chtl
// 单元测试
[Test] @Unit Button {
    test "渲染测试";
    test "事件测试";
    test "样式测试";
}

// 集成测试
[Test] @Integration FormComponents {
    test "表单提交流程";
    test "验证显示";
}

// 视觉回归测试
[Test] @Visual AllComponents {
    screenshot "default-state";
    screenshot "hover-state";
    screenshot "disabled-state";
}
```

---

通过遵循本指南，您将能够创建高质量、可维护、可复用的CMOD模块，为CHTL生态系统贡献优秀的组件和工具。