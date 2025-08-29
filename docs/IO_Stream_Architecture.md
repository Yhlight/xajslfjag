# CHTL IO流架构设计

## 问题背景

在CHTL的设计中，CSS和JavaScript编译器（基于ANTLR）需要接收**完整的代码字符串**进行解析，而不是片段。但统一扫描器会将源代码切割成多个片段，这就需要一个IO流系统来处理。

## 解决方案：FragmentCollector

### 架构图

```
源代码
   │
   ▼
┌─────────────────────┐
│  统一扫描器         │
│ (CHTLUnifiedScanner)│
└──────────┬──────────┘
           │ 生成片段列表
           ▼
┌─────────────────────┐
│  片段收集器         │
│ (FragmentCollector) │
└──────────┬──────────┘
           │ 按类型分组
           ▼
┌─────────────────────────────────────┐
│       四个独立的片段流              │
│  ┌─────────┐ ┌─────────┐          │
│  │CHTL流   │ │CHTLJS流 │          │
│  └─────────┘ └─────────┘          │
│  ┌─────────┐ ┌─────────┐          │
│  │CSS流    │ │JS流     │          │
│  └─────────┘ └─────────┘          │
└──────────┬──────────────────────────┘
           │ 完整代码字符串
           ▼
┌─────────────────────────────────────┐
│        各编译器                     │
│  ┌─────────┐ ┌─────────┐          │
│  │CSS编译器│ │JS编译器 │          │
│  │(ANTLR)  │ │(ANTLR)  │          │
│  └─────────┘ └─────────┘          │
└─────────────────────────────────────┘
```

### 工作流程

1. **扫描阶段**
   ```cpp
   // 统一扫描器切割代码
   auto fragments = scanner->scan(sourceCode);
   // fragments = [
   //   {type: "css", content: "body { color: red; }"},
   //   {type: "chtl", content: "div.container { ... }"},
   //   {type: "javascript", content: "console.log('hello');"},
   //   {type: "css", content: ".button { ... }"}
   // ]
   ```

2. **收集阶段**
   ```cpp
   // FragmentCollector收集同类型片段
   FragmentCollector collector;
   collector.processFragments(fragments);
   
   // 内部维护四个流
   // CSS流: "body { color: red; }\n.button { ... }"
   // JS流: "console.log('hello');"
   // CHTL流: "div.container { ... }"
   // CHTLJS流: ""
   ```

3. **编译阶段**
   ```cpp
   // CSS编译器接收完整的CSS代码
   if (collector.hasContent(FragmentType::CSS)) {
       std::string completeCSSCode = collector.getCompleteCSS();
       // completeCSSCode包含所有CSS片段，用换行符连接
       auto result = cssCompiler->compile(completeCSSCode);
   }
   
   // JS编译器接收完整的JS代码
   if (collector.hasContent(FragmentType::JavaScript)) {
       std::string completeJSCode = collector.getCompleteJavaScript();
       // completeJSCode包含所有JS片段
       auto result = jsCompiler->compile(completeJSCode);
   }
   ```

### FragmentCollector实现细节

```cpp
class FragmentCollector {
    class FragmentStream {
        FragmentType type_;
        std::vector<CodeFragment> fragments_;
        std::string content_;  // 累积的完整内容
        
        void addFragment(const CodeFragment& fragment) {
            fragments_.push_back(fragment);
            
            // 对于CSS和JS，片段之间添加换行以保持结构
            if (!content_.empty() && needsNewline(fragment)) {
                content_ += "\n";
            }
            content_ += fragment.getContent();
        }
    };
    
    std::map<FragmentType, std::unique_ptr<FragmentStream>> streams_;
};
```

### 关键优势

1. **保持代码完整性**：CSS和JS编译器接收到的是完整的代码，而不是片段
2. **保留结构信息**：通过适当的换行符保持代码结构
3. **类型隔离**：不同类型的代码完全分离，避免混淆
4. **效率优化**：只对有内容的类型调用相应的编译器

## ANTLR集成示例

### CSS编译器
```cpp
class CSSCompilerImpl : public CSSCompiler {
    CompileResult compile(const std::string& code, const CompileOptions& options) {
        // code是完整的CSS代码，包含所有CSS片段
        
        // 使用ANTLR解析
        ANTLRInputStream input(code);
        CSSLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        CSSParser parser(&tokens);
        
        // 解析完整的样式表
        auto stylesheet = parser.stylesheet();
        
        // 生成输出...
    }
};
```

### JavaScript编译器
```cpp
class JavaScriptCompilerImpl : public JavaScriptCompiler {
    CompileResult compile(const std::string& code, const CompileOptions& options) {
        // code是完整的JavaScript代码
        
        // 使用ANTLR解析
        ANTLRInputStream input(code);
        JavaScriptLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        JavaScriptParser parser(&tokens);
        
        // 解析完整的程序
        auto program = parser.program();
        
        // 处理AST...
    }
};
```

## 实际例子

### 输入CHTL文件
```chtl
[Style] {
    body {
        background: white;
    }
}

div.container {
    h1: "Hello"
    
    [Script] {
        console.log("Page loaded");
    }
    
    [Style] {
        .local {
            color: blue;
        }
    }
}

[Script] {
    function init() {
        alert("Init");
    }
}
```

### 处理流程

1. **扫描器输出片段**：
   - Fragment 1: CSS - "body { background: white; }"
   - Fragment 2: CHTL - "div.container { h1: \"Hello\" ... }"
   - Fragment 3: JS - "console.log(\"Page loaded\");"
   - Fragment 4: CSS - ".local { color: blue; }"
   - Fragment 5: JS - "function init() { alert(\"Init\"); }"

2. **FragmentCollector整合**：
   - CSS流: "body { background: white; }\n.local { color: blue; }"
   - JS流: "console.log(\"Page loaded\");\nfunction init() { alert(\"Init\"); }"

3. **编译器接收**：
   - CSS编译器接收完整CSS代码进行解析
   - JS编译器接收完整JS代码进行解析

## 总结

通过FragmentCollector这个中间层，我们成功解决了片段化扫描与完整代码编译之间的矛盾。这种设计既保持了统一扫描器的灵活性，又满足了ANTLR编译器对完整代码的需求。