# CHTL JS编译器开发指南

## 概述

CHTL JS编译器是CHTL项目的重要组成部分，专门负责处理CHTL语法中的JavaScript相关部分，包括CHTL JS语法糖、事件绑定、以及通过CJMOD扩展的自定义语法。本指南详细介绍了CHTL JS编译器的架构、开发流程和扩展机制。

## 编译器架构

### 🏗️ **整体架构**

```
CHTL JS源代码 → CHTLJSLexer → CHTLJSParser → CHTLJSGenerator
                     ↓              ↓            ↓
                 Token流        CHTL JS AST   JavaScript代码
                     ↑              ↑            ↑
                CJMODSystem ←→ CJMODScanner → 扩展语法支持
```

### 🧩 **核心组件**

#### 1. CHTLJSContext (JS编译上下文)
```cpp
// src/CHTLJS/CHTLJSContext/Context.h
class CHTLJSContext {
    // JS编译会话状态管理
    // 作用域链和变量绑定
    // CJMOD扩展注册管理
    // 异步语法状态追踪
};
```

**主要职责**:
- 管理JavaScript编译上下文
- 处理作用域链和变量绑定
- 管理CJMOD扩展的注册和调用
- 追踪异步语法的状态转换
- 提供错误上下文和调试信息

#### 2. CHTLJSLexer (JS词法分析器)
```cpp
// src/CHTLJS/CHTLJSLexer/Lexer.h
class CHTLJSLexer {
    // CHTL JS特殊Token识别
    // JavaScript关键字扩展
    // 操作符优先级管理
    // 字面量类型推断
};
```

**核心文件**:
- `Lexer.h/cpp`: JS词法分析器主体
- `Token.h/cpp`: CHTL JS Token定义
- `GlobalMap.h/cpp`: JS全局符号映射

**特殊Token类型**:
```cpp
enum class CHTLJSTokenType {
    // 基础JS Token
    JSKeyword, JSIdentifier, JSString, JSNumber,
    
    // CHTL JS 扩展Token
    Listen, Delegate, Animate, VirtualObject,
    
    // 事件绑定操作符
    EventBind,      // &->
    
    // 异步语法糖
    AsyncChain,     // await>>
    Parallel,       // parallel>>
    Timeout,        // timeout>>
    Retry,          // retry>>
    
    // 响应式绑定
    TwoWayBind,     // <->
    Computed,       // =>
    Conditional,    // ?:
    ListRender,     // *:
    Watcher,        // ~>
    EventStream,    // |>
    
    // CJMOD扩展Token
    CJMODKeyword,   // 动态关键字
};
```

#### 3. CHTLJSParser (JS语法分析器)
```cpp
// src/CHTLJS/CHTLJSParser/Parser.h
class CHTLJSParser {
    // CHTL JS语法解析
    // JavaScript表达式解析
    // CJMOD扩展语法解析
    // 错误恢复和同步
};
```

**解析策略**:
- 混合递归下降和运算符优先级解析
- JavaScript兼容性优先
- CHTL JS语法糖展开
- CJMOD动态语法注入

#### 4. CHTLJSNode (JS AST节点系统)
```
CHTLJSNode/
├── BaseJSNode.h            # JS基础节点
├── JSExpressionNode.h      # JS表达式节点
├── JSStatementNode.h       # JS语句节点
├── ListenNode.h            # listen语法节点
├── DelegateNode.h          # delegate语法节点
├── AnimateNode.h           # animate语法节点
├── VirtualObjectNode.h     # 虚对象节点
├── EventBindNode.h         # 事件绑定节点
├── AsyncNode.h             # 异步语法节点
├── ReactiveNode.h          # 响应式绑定节点
└── CJMODNode.h             # CJMOD扩展节点
```

**节点继承体系**:
```cpp
class BaseJSNode {
public:
    virtual void accept(JSVisitor& visitor) = 0;
    virtual std::string generateJS() const = 0;
    virtual JSNodeType getType() const = 0;
    virtual bool isAsync() const { return false; }
    
protected:
    SourceLocation location_;
    std::vector<std::unique_ptr<BaseJSNode>> children_;
};

class ListenNode : public BaseJSNode {
private:
    std::string variable_name_;
    std::unordered_map<std::string, std::unique_ptr<BaseJSNode>> event_handlers_;
    bool is_virtual_;
    
public:
    std::string generateJS() const override;
    JSNodeType getType() const override { return JSNodeType::Listen; }
};
```

#### 5. CHTLJSGenerator (JS代码生成器)
```cpp
// src/CHTLJS/CHTLJSGenerator/Generator.h
class CHTLJSGenerator {
    // JavaScript代码生成
    // ES6+特性支持
    // 代码优化和压缩
    // Source Map生成
};
```

**生成策略**:
- 模块化代码生成
- ES6+语法优先
- 异步代码优化
- 响应式框架集成

## CJMOD扩展系统

### 🔌 **CJMOD架构**

CJMOD (C++ JavaScript Module) 是CHTL JS编译器的扩展机制，允许开发者通过C++代码扩展CHTL JS语法。

#### 核心组件

##### 1. CJMODSystem (扩展系统核心)
```cpp
// src/CHTLJS/CJMODSystem/CJMODSystem.h
class CJMODSystem {
public:
    // 扩展注册和管理
    bool registerExtension(const std::string& name, std::unique_ptr<CJMODExtension> ext);
    CJMODExtension* getExtension(const std::string& name);
    
    // 语法扩展处理
    bool canHandle(const std::string& keyword);
    std::unique_ptr<BaseJSNode> parseExtension(const std::string& keyword, CHTLJSParser& parser);
};
```

##### 2. Syntax类 (语法分析)
```cpp
// src/CHTLJS/CJMODSystem/Syntax.h
class Syntax {
public:
    // 语法模式分析
    std::vector<AtomArg> analyze(const std::string& pattern);
    
    // 内容类型验证
    bool isObject(const std::string& content);
    bool isFunction(const std::string& content);
    bool isArray(const std::string& content);
    bool isCHTLJSFunction(const std::string& content);
    
private:
    std::regex object_pattern_;
    std::regex function_pattern_;
    std::regex array_pattern_;
    std::regex chtljs_function_pattern_;
};
```

##### 3. CJMODScanner (动态扫描器)
```cpp
// src/CHTLJS/CJMODSystem/CJMODScanner.h
class CJMODScanner {
public:
    // 静态扫描方法
    static DualPointerScanResult scan(const Arg& args, const CHTL::String& keyword);
    
private:
    // 双指针扫描算法
    static std::pair<size_t, size_t> findKeywordRange(const std::string& content, const std::string& keyword);
    
    // 预截断优化
    static std::string preTruncate(const std::string& content, const std::string& keyword);
};
```

### 🛠️ **CJMOD开发流程**

#### 1. 创建CJMOD扩展

```cpp
// MyExtension.h
#include "CHTLJS/CJMODSystem/CJMODExtension.h"

class MyAwaitExtension : public CJMODExtension {
public:
    std::string getName() const override { return "await_enhanced"; }
    
    std::vector<std::string> getKeywords() const override {
        return {"await>>", "parallel>>", "timeout>>", "retry>>"};
    }
    
    std::unique_ptr<BaseJSNode> parse(const std::string& keyword, 
                                      const std::vector<Arg>& args,
                                      CHTLJSParser& parser) override;
    
    std::string generateJS(const BaseJSNode* node) const override;
    
private:
    // 扩展特定的解析逻辑
    std::unique_ptr<BaseJSNode> parseAwaitChain(const std::vector<Arg>& args);
    std::unique_ptr<BaseJSNode> parseParallel(const std::vector<Arg>& args);
};
```

#### 2. 实现解析逻辑

```cpp
// MyExtension.cpp
std::unique_ptr<BaseJSNode> MyAwaitExtension::parse(const std::string& keyword, 
                                                    const std::vector<Arg>& args,
                                                    CHTLJSParser& parser) {
    if (keyword == "await>>") {
        return parseAwaitChain(args);
    } else if (keyword == "parallel>>") {
        return parseParallel(args);
    } else if (keyword == "timeout>>") {
        return parseTimeout(args);
    } else if (keyword == "retry>>") {
        return parseRetry(args);
    }
    
    return nullptr;
}

std::unique_ptr<BaseJSNode> MyAwaitExtension::parseAwaitChain(const std::vector<Arg>& args) {
    auto node = std::make_unique<AsyncChainNode>();
    
    // 使用CJMODScanner动态扫描参数
    for (const auto& arg : args) {
        auto scan_result = CJMODScanner::scan(arg, "then");
        if (scan_result.found) {
            // 处理链式调用
            node->addChainCall(scan_result.before, scan_result.after);
        }
    }
    
    return node;
}
```

#### 3. 实现代码生成

```cpp
std::string MyAwaitExtension::generateJS(const BaseJSNode* node) const {
    if (auto chain_node = dynamic_cast<const AsyncChainNode*>(node)) {
        std::stringstream js;
        
        js << "(async () => {\n";
        js << "  try {\n";
        
        for (const auto& call : chain_node->getChainCalls()) {
            js << "    const result_" << call.id << " = await " << call.expression << ";\n";
        }
        
        js << "    return result_" << chain_node->getLastCallId() << ";\n";
        js << "  } catch (error) {\n";
        js << "    console.error('Async chain error:', error);\n";
        js << "    throw error;\n";
        js << "  }\n";
        js << "})()";
        
        return js.str();
    }
    
    return "";
}
```

## 核心语法实现

### 🎭 **Listen语法**

```cpp
// ListenNode实现
class ListenNode : public BaseJSNode {
public:
    std::string generateJS() const override {
        std::stringstream js;
        
        if (is_virtual_) {
            // 虚对象模式：编译时处理，运行时不存在
            return "";
        }
        
        js << "const " << variable_name_ << " = {\n";
        
        for (const auto& [event, handler] : event_handlers_) {
            js << "  " << event << ": " << handler->generateJS() << ",\n";
        }
        
        js << "};\n";
        
        return js.str();
    }
};

// 解析listen语法
std::unique_ptr<BaseJSNode> CHTLJSParser::parseListen() {
    // listen variable_name = { event_handlers }
    consume(TokenType::Listen);
    
    std::string var_name = consume(TokenType::Identifier).text;
    consume(TokenType::Assign);
    
    auto listen_node = std::make_unique<ListenNode>(var_name);
    
    consume(TokenType::LeftBrace);
    
    while (!check(TokenType::RightBrace)) {
        std::string event_name = consume(TokenType::Identifier).text;
        consume(TokenType::Colon);
        
        auto handler = parseJSExpression();
        listen_node->addEventHandler(event_name, std::move(handler));
        
        if (!check(TokenType::RightBrace)) {
            consume(TokenType::Comma);
        }
    }
    
    consume(TokenType::RightBrace);
    
    return listen_node;
}
```

### 🎯 **事件绑定操作符 (&->)**

```cpp
// EventBindNode实现
class EventBindNode : public BaseJSNode {
private:
    std::string selector_;
    std::string event_type_;
    std::unique_ptr<BaseJSNode> handler_;
    
public:
    std::string generateJS() const override {
        std::stringstream js;
        
        js << "document.addEventListener('DOMContentLoaded', () => {\n";
        js << "  const elements = document.querySelectorAll('" << selector_ << "');\n";
        js << "  elements.forEach(element => {\n";
        js << "    element.addEventListener('" << event_type_ << "', ";
        js << handler_->generateJS() << ");\n";
        js << "  });\n";
        js << "});\n";
        
        return js.str();
    }
};

// 解析事件绑定
std::unique_ptr<BaseJSNode> CHTLJSParser::parseEventBinding() {
    // {{selector}} &-> event_type { handler }
    
    consume(TokenType::DoubleBrace);
    std::string selector = consume(TokenType::String).text;
    consume(TokenType::DoubleBrace);
    
    consume(TokenType::EventBind);  // &->
    
    std::string event_type = consume(TokenType::Identifier).text;
    
    consume(TokenType::LeftBrace);
    auto handler = parseJSStatement();
    consume(TokenType::RightBrace);
    
    return std::make_unique<EventBindNode>(selector, event_type, std::move(handler));
}
```

### ⚡ **异步语法糖**

#### await>> (链式异步调用)
```cpp
class AsyncChainNode : public BaseJSNode {
public:
    std::string generateJS() const override {
        std::stringstream js;
        
        js << "(async () => {\n";
        
        // 生成链式await调用
        std::string result_var = "result_0";
        for (size_t i = 0; i < chain_calls_.size(); ++i) {
            if (i == 0) {
                js << "  let " << result_var << " = await " << chain_calls_[i] << ";\n";
            } else {
                std::string next_var = "result_" + std::to_string(i);
                js << "  let " << next_var << " = await " << chain_calls_[i] 
                   << "(" << result_var << ");\n";
                result_var = next_var;
            }
        }
        
        js << "  return " << result_var << ";\n";
        js << "})()";
        
        return js.str();
    }
};
```

#### parallel>> (并行异步调用)
```cpp
class ParallelNode : public BaseJSNode {
public:
    std::string generateJS() const override {
        std::stringstream js;
        
        js << "Promise.all([\n";
        
        for (size_t i = 0; i < parallel_calls_.size(); ++i) {
            if (i > 0) js << ",\n";
            js << "  " << parallel_calls_[i];
        }
        
        js << "\n])";
        
        return js.str();
    }
};
```

### 🔄 **响应式绑定语法**

#### 双向绑定 (<->)
```cpp
class TwoWayBindNode : public BaseJSNode {
public:
    std::string generateJS() const override {
        std::stringstream js;
        
        js << "{\n";
        js << "  let _value = " << initial_value_ << ";\n";
        js << "  Object.defineProperty(" << target_object_ << ", '" << property_ << "', {\n";
        js << "    get() { return _value; },\n";
        js << "    set(newValue) {\n";
        js << "      _value = newValue;\n";
        js << "      // 触发UI更新\n";
        js << "      document.querySelectorAll('[data-bind=\"" << property_ << "\"]').forEach(el => {\n";
        js << "        el.textContent = newValue;\n";
        js << "      });\n";
        js << "    }\n";
        js << "  });\n";
        js << "}";
        
        return js.str();
    }
};
```

#### 计算属性 (=>)
```cpp
class ComputedNode : public BaseJSNode {
public:
    std::string generateJS() const override {
        std::stringstream js;
        
        js << "Object.defineProperty(" << target_object_ << ", '" << property_ << "', {\n";
        js << "  get() {\n";
        js << "    return " << computation_expression_ << ";\n";
        js << "  },\n";
        js << "  enumerable: true,\n";
        js << "  configurable: true\n";
        js << "})";
        
        return js.str();
    }
};
```

## 开发工具和调试

### 🧪 **单元测试**

```cpp
// tests/CHTLJS/Parser/CHTLJSParserTest.cpp
class CHTLJSParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        context_ = std::make_unique<CHTLJSContext>();
        parser_ = std::make_unique<CHTLJSParser>("", *context_);
    }
    
    std::unique_ptr<CHTLJSContext> context_;
    std::unique_ptr<CHTLJSParser> parser_;
};

TEST_F(CHTLJSParserTest, ParseListenSyntax) {
    std::string source = R"(
        listen myHandler = {
            click: () => { console.log('clicked'); },
            hover: (event) => { console.log('hovered'); }
        }
    )";
    
    parser_ = std::make_unique<CHTLJSParser>(source, *context_);
    auto ast = parser_->parse();
    
    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->getType(), JSNodeType::Listen);
    
    auto listen_node = static_cast<ListenNode*>(ast.get());
    EXPECT_EQ(listen_node->getVariableName(), "myHandler");
    EXPECT_EQ(listen_node->getEventHandlers().size(), 2);
}

TEST_F(CHTLJSParserTest, ParseEventBinding) {
    std::string source = R"(
        {{.button}} &-> click {
            console.log('Button clicked!');
        }
    )";
    
    parser_ = std::make_unique<CHTLJSParser>(source, *context_);
    auto ast = parser_->parse();
    
    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->getType(), JSNodeType::EventBind);
}
```

### 🔍 **AST调试工具**

```cpp
// JSASTPrinter.h
class JSASTPrinter : public JSNodeVisitor {
private:
    int depth_ = 0;
    std::ostream& output_;
    
public:
    explicit JSASTPrinter(std::ostream& os) : output_(os) {}
    
    void visit(ListenNode* node) override {
        printIndent();
        output_ << "Listen: " << node->getVariableName() << std::endl;
        
        depth_++;
        for (const auto& [event, handler] : node->getEventHandlers()) {
            printIndent();
            output_ << "Event: " << event << std::endl;
            depth_++;
            handler->accept(*this);
            depth_--;
        }
        depth_--;
    }
    
    void visit(EventBindNode* node) override {
        printIndent();
        output_ << "EventBind: " << node->getSelector() 
                << " -> " << node->getEventType() << std::endl;
        
        depth_++;
        node->getHandler()->accept(*this);
        depth_--;
    }
    
private:
    void printIndent() {
        output_ << std::string(depth_ * 2, ' ');
    }
};
```

### 📊 **性能分析**

```cpp
// CHTLJSCompileTimer.h
class CHTLJSCompileTimer {
private:
    struct PhaseInfo {
        std::chrono::milliseconds duration;
        size_t node_count;
        size_t js_lines_generated;
    };
    
    std::unordered_map<std::string, PhaseInfo> phases_;
    
public:
    void startPhase(const std::string& phase_name) {
        // 开始计时
    }
    
    void endPhase(const std::string& phase_name, size_t node_count = 0, size_t js_lines = 0) {
        // 结束计时并记录统计信息
    }
    
    void printReport() const {
        std::cout << "CHTL JS Compilation Performance Report\n";
        std::cout << "=====================================\n";
        
        for (const auto& [phase, info] : phases_) {
            std::cout << phase << ": " << info.duration.count() << "ms";
            if (info.node_count > 0) {
                std::cout << " (" << info.node_count << " nodes)";
            }
            if (info.js_lines_generated > 0) {
                std::cout << " -> " << info.js_lines_generated << " JS lines";
            }
            std::cout << std::endl;
        }
    }
};
```

## 扩展开发最佳实践

### ✅ **CJMOD开发建议**

1. **保持语法简洁**: 设计易于理解和使用的语法
2. **提供完整错误信息**: 包括建议和修复提示
3. **支持增量编译**: 确保扩展不会影响编译性能
4. **编写详细文档**: 包括语法说明和使用示例
5. **提供单元测试**: 覆盖所有语法分支和边界情况

### 🚫 **常见陷阱**

1. **语法冲突**: 避免与现有CHTL JS语法冲突
2. **性能影响**: 注意扫描和解析的性能开销
3. **内存泄漏**: 正确管理AST节点的生命周期
4. **异步处理**: 确保异步代码的正确性和安全性
5. **跨平台兼容**: 考虑不同JavaScript引擎的差异

### 📝 **代码规范**

```cpp
// CJMOD扩展命名规范
class MyExtension : public CJMODExtension {
    // 使用描述性的类名
    // 遵循驼峰命名法
};

// 方法命名
std::unique_ptr<BaseJSNode> parseMyFeature();  // parse + 功能名
std::string generateMyFeatureJS();             // generate + 功能名 + JS

// 常量命名
static const std::string MY_KEYWORD = "my_keyword";
```

## 高级特性

### 🌟 **虚对象系统**

虚对象是CHTL JS的重要特性，允许在编译时处理对象访问：

```cpp
class VirtualObjectNode : public BaseJSNode {
private:
    std::string object_name_;
    std::unordered_map<std::string, std::unique_ptr<BaseJSNode>> properties_;
    bool is_virtual_;
    
public:
    std::string generateJS() const override {
        if (is_virtual_) {
            // 虚对象在运行时不存在，只在编译时处理
            return "";
        }
        
        // 生成实际的JavaScript对象
        std::stringstream js;
        js << "const " << object_name_ << " = {\n";
        
        for (const auto& [key, value] : properties_) {
            js << "  " << key << ": " << value->generateJS() << ",\n";
        }
        
        js << "};\n";
        return js.str();
    }
    
    // 编译时属性访问处理
    std::unique_ptr<BaseJSNode> resolvePropertyAccess(const std::string& property) {
        auto it = properties_.find(property);
        if (it != properties_.end()) {
            return it->second->clone();
        }
        return nullptr;
    }
};
```

### 🔄 **响应式系统**

CHTL JS提供了完整的响应式编程支持：

```cpp
// 响应式系统管理器
class ReactiveSystemManager {
private:
    std::unordered_map<std::string, std::vector<std::string>> dependencies_;
    std::unordered_map<std::string, std::unique_ptr<BaseJSNode>> computations_;
    
public:
    void addDependency(const std::string& property, const std::string& dependent) {
        dependencies_[property].push_back(dependent);
    }
    
    std::string generateReactiveSystem() const {
        std::stringstream js;
        
        js << "// CHTL JS 响应式系统\n";
        js << "const CHTL_Reactive = {\n";
        js << "  dependencies: " << serializeDependencies() << ",\n";
        js << "  notify(property) {\n";
        js << "    const dependents = this.dependencies[property] || [];\n";
        js << "    dependents.forEach(dep => {\n";
        js << "      if (typeof dep === 'function') dep();\n";
        js << "      else this.notify(dep);\n";
        js << "    });\n";
        js << "  }\n";
        js << "};\n";
        
        return js.str();
    }
};
```

---

通过本指南，您将能够深入理解CHTL JS编译器的工作原理，并能够开发强大的CJMOD扩展来增强CHTL JS的功能。