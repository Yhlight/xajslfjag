# CJMOD开发指南

## 概述

CJMOD (C++ JavaScript Module) 是CHTL提供的强大扩展机制，允许开发者使用C++代码扩展CHTL JS语法。通过CJMOD，您可以创建全新的语法特性、优化性能关键部分，或集成第三方C++库。

## CJMOD基础概念

### 🔌 **什么是CJMOD**

CJMOD允许您：
- **扩展CHTL JS语法**: 创建新的关键字和语法结构
- **性能优化**: 将性能关键代码用C++实现
- **库集成**: 集成现有的C++库和框架
- **自定义功能**: 实现CHTL JS无法直接支持的功能

### 🏗️ **CJMOD架构**

```
CHTL JS源码 → CJMODScanner → Syntax分析 → C++扩展处理 → JavaScript代码生成
                    ↓              ↓           ↓
               关键字识别      语法解析    自定义逻辑处理
```

### 📦 **CJMOD文件结构**

```
CJModuleName/
├── src/                    # C++源代码
│   ├── main.cpp           # 主扩展实现
│   ├── main.h             # 头文件
│   ├── parser.cpp         # 语法解析器
│   ├── generator.cpp      # 代码生成器
│   └── SubModule/         # 子模块 (可选)
│       ├── src/
│       │   ├── sub.cpp
│       │   └── sub.h
│       └── info/
│           └── SubModule.chtl
└── info/                  # 模块信息
    └── CJModuleName.chtl  # 模块信息文件
```

## 创建CJMOD模块

### 🛠️ **初始化新模块**

```bash
# 使用CHTL工具创建新CJMOD
chtl create-cjmod MyAsyncExtension

# 或手动创建目录结构
mkdir MyAsyncExtension
mkdir MyAsyncExtension/src
mkdir MyAsyncExtension/info
```

### 📝 **模块信息文件**

```chtl
// info/MyAsyncExtension.chtl
[Info]
{
    name = "MyAsyncExtension";
    version = "1.0.0";
    description = "高级异步语法扩展 - 提供链式调用、并行处理等功能";
    author = "Your Name";
    license = "MIT";
    cppStandard = "17";
    dependencies = "CHTL.JS.Core@1.0.0";
    chtlJSVersion = "1.0.0";
    repository = "https://github.com/yourname/myasyncextension.git";
    keywords = "async, await, promise, performance";
    
    // 扩展的关键字
    keywords_provided = "await>>, parallel>>, timeout>>, retry>>";
    
    // 兼容性
    platforms = "linux, windows, macos";
    compilers = "gcc>=7, clang>=6, msvc>=2017";
}

[Export]
{
    // 导出的扩展功能
    keywords: ["await>>", "parallel>>", "timeout>>", "retry>>"];
    functions: ["chainAsync", "parallelAsync", "timeoutAsync", "retryAsync"];
}
```

## 核心API详解

### 🔍 **Syntax类**

Syntax类负责语法模式分析和内容验证：

```cpp
// src/main.h
#include "CHTLJS/CJMODSystem/Syntax.h"
#include "CHTLJS/CJMODSystem/CJMODExtension.h"

class MyAsyncExtension : public CJMODExtension {
private:
    CJMOD::Syntax syntax_analyzer_;
    
public:
    std::string getName() const override {
        return "MyAsyncExtension";
    }
    
    std::vector<std::string> getKeywords() const override {
        return {"await>>", "parallel>>", "timeout>>", "retry>>"};
    }
    
    std::unique_ptr<BaseJSNode> parse(const std::string& keyword, 
                                      const std::vector<Arg>& args,
                                      CHTLJSParser& parser) override;
    
    std::string generateJS(const BaseJSNode* node) const override;
    
private:
    // 解析方法
    std::unique_ptr<BaseJSNode> parseAwaitChain(const std::vector<Arg>& args);
    std::unique_ptr<BaseJSNode> parseParallel(const std::vector<Arg>& args);
    std::unique_ptr<BaseJSNode> parseTimeout(const std::vector<Arg>& args);
    std::unique_ptr<BaseJSNode> parseRetry(const std::vector<Arg>& args);
    
    // 辅助方法
    bool validateAsyncExpression(const std::string& expr);
    std::string optimizeAsyncCode(const std::string& code);
};
```

### 🔎 **CJMODScanner使用**

CJMODScanner提供强大的动态扫描功能：

```cpp
// src/main.cpp
#include "CHTLJS/CJMODSystem/CJMODScanner.h"

std::unique_ptr<BaseJSNode> MyAsyncExtension::parseAwaitChain(const std::vector<Arg>& args) {
    auto chain_node = std::make_unique<AsyncChainNode>();
    
    for (const auto& arg : args) {
        // 使用CJMODScanner动态扫描关键字
        auto then_result = CJMODScanner::scan(arg, "then");
        auto catch_result = CJMODScanner::scan(arg, "catch");
        auto finally_result = CJMODScanner::scan(arg, "finally");
        
        if (then_result.found) {
            // 处理then链
            chain_node->addThenClause(then_result.before, then_result.after);
        }
        
        if (catch_result.found) {
            // 处理catch块
            chain_node->addCatchClause(catch_result.after);
        }
        
        if (finally_result.found) {
            // 处理finally块
            chain_node->addFinallyClause(finally_result.after);
        }
        
        // 分析语法模式
        auto atoms = syntax_analyzer_.analyze(arg.content);
        for (const auto& atom : atoms) {
            if (atom.type == AtomArg::Type::Function) {
                chain_node->addAsyncCall(atom.content);
            }
        }
    }
    
    return chain_node;
}
```

### 🎯 **语法分析实现**

```cpp
// src/parser.cpp
bool MyAsyncExtension::validateAsyncExpression(const std::string& expr) {
    // 检查是否为函数调用
    if (syntax_analyzer_.isFunction(expr)) {
        return true;
    }
    
    // 检查是否为Promise表达式
    if (expr.find("Promise") != std::string::npos) {
        return true;
    }
    
    // 检查是否为async函数
    if (expr.find("async") != std::string::npos) {
        return true;
    }
    
    // 检查CHTL JS函数语法
    if (syntax_analyzer_.isCHTLJSFunction(expr)) {
        return true;
    }
    
    return false;
}

std::unique_ptr<BaseJSNode> MyAsyncExtension::parse(const std::string& keyword, 
                                                    const std::vector<Arg>& args,
                                                    CHTLJSParser& parser) {
    // 根据关键字分发到不同的解析器
    if (keyword == "await>>") {
        return parseAwaitChain(args);
    } else if (keyword == "parallel>>") {
        return parseParallel(args);
    } else if (keyword == "timeout>>") {
        return parseTimeout(args);
    } else if (keyword == "retry>>") {
        return parseRetry(args);
    }
    
    // 未知关键字
    throw CJMODParseException("Unknown keyword: " + keyword);
}
```

## 高级语法扩展示例

### ⚡ **异步链式调用 (await>>)**

```cpp
// src/async_chain.cpp
class AsyncChainNode : public BaseJSNode {
private:
    struct ChainCall {
        std::string expression;
        std::string then_handler;
        std::string catch_handler;
        std::string finally_handler;
        bool has_timeout;
        int timeout_ms;
    };
    
    std::vector<ChainCall> chain_calls_;
    std::string error_handler_;
    std::string finally_handler_;
    
public:
    void addAsyncCall(const std::string& expr) {
        ChainCall call;
        call.expression = expr;
        call.has_timeout = false;
        chain_calls_.push_back(call);
    }
    
    void addThenClause(const std::string& expr, const std::string& handler) {
        if (!chain_calls_.empty()) {
            chain_calls_.back().then_handler = handler;
        }
    }
    
    std::string generateJS() const override {
        std::stringstream js;
        
        js << "(async () => {\n";
        js << "  try {\n";
        
        // 生成链式调用
        for (size_t i = 0; i < chain_calls_.size(); ++i) {
            const auto& call = chain_calls_[i];
            
            if (i == 0) {
                js << "    let result = await " << call.expression << ";\n";
            } else {
                js << "    result = await " << call.expression;
                if (!call.then_handler.empty()) {
                    js << ".then(" << call.then_handler << ")";
                }
                js << ";\n";
            }
            
            // 处理超时
            if (call.has_timeout) {
                js << "    result = await Promise.race([\n";
                js << "      result,\n";
                js << "      new Promise((_, reject) => setTimeout(() => \n";
                js << "        reject(new Error('Timeout')), " << call.timeout_ms << "))\n";
                js << "    ]);\n";
            }
        }
        
        js << "    return result;\n";
        js << "  } catch (error) {\n";
        
        if (!error_handler_.empty()) {
            js << "    return " << error_handler_ << "(error);\n";
        } else {
            js << "    console.error('Async chain error:', error);\n";
            js << "    throw error;\n";
        }
        
        js << "  }";
        
        if (!finally_handler_.empty()) {
            js << " finally {\n";
            js << "    " << finally_handler_ << "();\n";
            js << "  }";
        }
        
        js << "\n})()";
        
        return js.str();
    }
};
```

### 🔀 **并行异步处理 (parallel>>)**

```cpp
// src/parallel_async.cpp
std::unique_ptr<BaseJSNode> MyAsyncExtension::parseParallel(const std::vector<Arg>& args) {
    auto parallel_node = std::make_unique<ParallelAsyncNode>();
    
    for (const auto& arg : args) {
        // 扫描并行任务
        auto task_result = CJMODScanner::scan(arg, "task");
        if (task_result.found) {
            parallel_node->addTask(task_result.after);
        }
        
        // 扫描结果处理
        auto then_result = CJMODScanner::scan(arg, "then");
        if (then_result.found) {
            parallel_node->setResultHandler(then_result.after);
        }
        
        // 扫描错误处理
        auto catch_result = CJMODScanner::scan(arg, "catch");
        if (catch_result.found) {
            parallel_node->setErrorHandler(catch_result.after);
        }
        
        // 检查是否为数组形式
        if (syntax_analyzer_.isArray(arg.content)) {
            // 解析数组中的任务
            parseArrayTasks(arg.content, parallel_node.get());
        }
    }
    
    return parallel_node;
}

class ParallelAsyncNode : public BaseJSNode {
private:
    std::vector<std::string> tasks_;
    std::string result_handler_;
    std::string error_handler_;
    bool fail_fast_ = true;
    
public:
    void addTask(const std::string& task) {
        tasks_.push_back(task);
    }
    
    void setFailFast(bool fail_fast) {
        fail_fast_ = fail_fast;
    }
    
    std::string generateJS() const override {
        std::stringstream js;
        
        if (fail_fast_) {
            js << "Promise.all([\n";
        } else {
            js << "Promise.allSettled([\n";
        }
        
        for (size_t i = 0; i < tasks_.size(); ++i) {
            if (i > 0) js << ",\n";
            js << "  " << tasks_[i];
        }
        
        js << "\n])";
        
        if (!result_handler_.empty()) {
            js << ".then(" << result_handler_ << ")";
        }
        
        if (!error_handler_.empty()) {
            js << ".catch(" << error_handler_ << ")";
        }
        
        return js.str();
    }
};
```

### ⏱️ **超时控制 (timeout>>)**

```cpp
// src/timeout_async.cpp
std::unique_ptr<BaseJSNode> MyAsyncExtension::parseTimeout(const std::vector<Arg>& args) {
    auto timeout_node = std::make_unique<TimeoutAsyncNode>();
    
    for (const auto& arg : args) {
        // 扫描within关键字
        auto within_result = CJMODScanner::scan(arg, "within");
        if (within_result.found) {
            timeout_node->setExpression(within_result.before);
            timeout_node->setTimeout(std::stoi(within_result.after));
        }
        
        // 扫描or关键字 (默认值)
        auto or_result = CJMODScanner::scan(arg, "or");
        if (or_result.found) {
            timeout_node->setDefaultValue(or_result.after);
        }
        
        // 扫描catch关键字 (超时处理)
        auto catch_result = CJMODScanner::scan(arg, "catch timeout");
        if (catch_result.found) {
            timeout_node->setTimeoutHandler(catch_result.after);
        }
    }
    
    return timeout_node;
}

class TimeoutAsyncNode : public BaseJSNode {
private:
    std::string expression_;
    int timeout_ms_;
    std::string default_value_;
    std::string timeout_handler_;
    
public:
    std::string generateJS() const override {
        std::stringstream js;
        
        js << "Promise.race([\n";
        js << "  " << expression_ << ",\n";
        js << "  new Promise((resolve, reject) => {\n";
        js << "    setTimeout(() => {\n";
        
        if (!timeout_handler_.empty()) {
            js << "      resolve(" << timeout_handler_ << "());\n";
        } else if (!default_value_.empty()) {
            js << "      resolve(" << default_value_ << ");\n";
        } else {
            js << "      reject(new Error('Operation timed out after " 
               << timeout_ms_ << "ms'));\n";
        }
        
        js << "    }, " << timeout_ms_ << ");\n";
        js << "  })\n";
        js << "])";
        
        return js.str();
    }
};
```

### 🔄 **重试机制 (retry>>)**

```cpp
// src/retry_async.cpp
std::unique_ptr<BaseJSNode> MyAsyncExtension::parseRetry(const std::vector<Arg>& args) {
    auto retry_node = std::make_unique<RetryAsyncNode>();
    
    for (const auto& arg : args) {
        // 扫描times关键字
        auto times_result = CJMODScanner::scan(arg, "times");
        if (times_result.found) {
            retry_node->setExpression(times_result.before);
            retry_node->setMaxRetries(std::stoi(times_result.after));
        }
        
        // 扫描delay关键字
        auto delay_result = CJMODScanner::scan(arg, "delay");
        if (delay_result.found) {
            retry_node->setDelay(std::stoi(delay_result.after));
        }
        
        // 扫描when条件
        auto when_result = CJMODScanner::scan(arg, "when");
        if (when_result.found) {
            retry_node->setRetryCondition(when_result.after);
        }
        
        // 扫描backoff策略
        auto backoff_result = CJMODScanner::scan(arg, "backoff");
        if (backoff_result.found) {
            retry_node->setBackoffStrategy(backoff_result.after);
        }
    }
    
    return retry_node;
}

class RetryAsyncNode : public BaseJSNode {
private:
    std::string expression_;
    int max_retries_ = 3;
    int delay_ms_ = 1000;
    std::string retry_condition_;
    std::string backoff_strategy_ = "linear";
    
public:
    std::string generateJS() const override {
        std::stringstream js;
        
        js << "(async function retryOperation() {\n";
        js << "  const operation = () => " << expression_ << ";\n";
        js << "  const maxRetries = " << max_retries_ << ";\n";
        js << "  const baseDelay = " << delay_ms_ << ";\n";
        js << "  \n";
        js << "  for (let attempt = 0; attempt < maxRetries; attempt++) {\n";
        js << "    try {\n";
        js << "      return await operation();\n";
        js << "    } catch (error) {\n";
        js << "      if (attempt === maxRetries - 1) {\n";
        js << "        throw error;\n";
        js << "      }\n";
        
        // 重试条件检查
        if (!retry_condition_.empty()) {
            js << "      \n";
            js << "      const shouldRetry = " << retry_condition_ << ";\n";
            js << "      if (!shouldRetry) {\n";
            js << "        throw error;\n";
            js << "      }\n";
        }
        
        // 延迟策略
        js << "      \n";
        js << "      let delay = baseDelay;\n";
        
        if (backoff_strategy_ == "exponential") {
            js << "      delay = baseDelay * Math.pow(2, attempt);\n";
        } else if (backoff_strategy_ == "linear") {
            js << "      delay = baseDelay * (attempt + 1);\n";
        } else if (backoff_strategy_ == "random") {
            js << "      delay = baseDelay + Math.random() * baseDelay;\n";
        }
        
        js << "      \n";
        js << "      await new Promise(resolve => setTimeout(resolve, delay));\n";
        js << "    }\n";
        js << "  }\n";
        js << "})()";
        
        return js.str();
    }
};
```

## 响应式绑定扩展

### 🔗 **双向绑定实现**

```cpp
// src/reactive_binding.cpp
class ReactiveExtension : public CJMODExtension {
public:
    std::vector<std::string> getKeywords() const override {
        return {"<->", "=>", "?:", "*:", "~>", "|>"};
    }
    
    std::unique_ptr<BaseJSNode> parse(const std::string& keyword, 
                                      const std::vector<Arg>& args,
                                      CHTLJSParser& parser) override {
        if (keyword == "<->") {
            return parseTwoWayBinding(args);
        } else if (keyword == "=>") {
            return parseComputedProperty(args);
        } else if (keyword == "?:") {
            return parseConditionalRender(args);
        } else if (keyword == "*:") {
            return parseListRender(args);
        } else if (keyword == "~>") {
            return parseWatcher(args);
        } else if (keyword == "|>") {
            return parseEventStream(args);
        }
        
        return nullptr;
    }
};

class TwoWayBindingNode : public BaseJSNode {
private:
    std::string object_property_;
    std::string element_selector_;
    std::string element_property_;
    std::string transformer_;
    
public:
    std::string generateJS() const override {
        std::stringstream js;
        
        js << "{\n";
        js << "  // 双向绑定: " << object_property_ << " <-> " 
           << element_selector_ << "." << element_property_ << "\n";
        js << "  const element = document.querySelector('" << element_selector_ << "');\n";
        js << "  let _internalValue = " << object_property_ << ";\n";
        js << "  \n";
        
        // 创建属性描述符
        js << "  Object.defineProperty(window, '" << getPropertyName() << "', {\n";
        js << "    get() { return _internalValue; },\n";
        js << "    set(newValue) {\n";
        js << "      _internalValue = newValue;\n";
        
        if (!transformer_.empty()) {
            js << "      const transformedValue = " << transformer_ << "(newValue);\n";
            js << "      element." << element_property_ << " = transformedValue;\n";
        } else {
            js << "      element." << element_property_ << " = newValue;\n";
        }
        
        js << "      // 触发变更事件\n";
        js << "      element.dispatchEvent(new CustomEvent('chtl:valueChanged', {\n";
        js << "        detail: { oldValue: _internalValue, newValue }\n";
        js << "      }));\n";
        js << "    }\n";
        js << "  });\n";
        js << "  \n";
        
        // 监听元素变化
        js << "  element.addEventListener('" << getChangeEvent() << "', (event) => {\n";
        js << "    " << object_property_ << " = event.target." << element_property_ << ";\n";
        js << "  });\n";
        js << "  \n";
        
        // 初始同步
        js << "  element." << element_property_ << " = " << object_property_ << ";\n";
        js << "}";
        
        return js.str();
    }
};
```

## 构建和测试

### 🔨 **CMakeLists.txt配置**

```cmake
# CMakeLists.txt (由打包脚本自动生成)
cmake_minimum_required(VERSION 3.15)
project(MyAsyncExtension VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找CHTL JS头文件
find_path(CHTLJS_INCLUDE_DIR 
    NAMES CHTLJS/CJMODSystem/Syntax.h
    PATHS ${CMAKE_INSTALL_PREFIX}/include /usr/local/include /usr/include
)

if(NOT CHTLJS_INCLUDE_DIR)
    message(FATAL_ERROR "CHTL JS headers not found. Please install CHTL first.")
endif()

include_directories(${CHTLJS_INCLUDE_DIR})

# 收集源文件
file(GLOB_RECURSE CJMOD_SOURCES "src/*.cpp")
file(GLOB_RECURSE CJMOD_HEADERS "src/*.h" "src/*.hpp")

# 创建动态库
add_library(MyAsyncExtension SHARED ${CJMOD_SOURCES})

# 设置输出目录
set_target_properties(MyAsyncExtension PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/cjmod"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/cjmod"
)

# 安装规则
install(TARGETS MyAsyncExtension
    LIBRARY DESTINATION lib/chtl/cjmod
    RUNTIME DESTINATION lib/chtl/cjmod
)

install(FILES info/MyAsyncExtension.chtl
    DESTINATION share/chtl/cjmod/info
)
```

### 🧪 **单元测试**

```cpp
// tests/test_async_extension.cpp
#include <gtest/gtest.h>
#include "MyAsyncExtension.h"

class AsyncExtensionTest : public ::testing::Test {
protected:
    void SetUp() override {
        extension_ = std::make_unique<MyAsyncExtension>();
    }
    
    std::unique_ptr<MyAsyncExtension> extension_;
};

TEST_F(AsyncExtensionTest, ParseAwaitChain) {
    std::vector<Arg> args = {
        Arg{"fetchData() >> processData >> saveData"}
    };
    
    auto node = extension_->parse("await>>", args, mock_parser);
    
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->getType(), JSNodeType::AsyncChain);
    
    auto js_code = extension_->generateJS(node.get());
    EXPECT_THAT(js_code, testing::HasSubstr("async ()"));
    EXPECT_THAT(js_code, testing::HasSubstr("await fetchData()"));
}

TEST_F(AsyncExtensionTest, ParseParallelExecution) {
    std::vector<Arg> args = {
        Arg{"[task1(), task2(), task3()]"}
    };
    
    auto node = extension_->parse("parallel>>", args, mock_parser);
    
    ASSERT_NE(node, nullptr);
    auto js_code = extension_->generateJS(node.get());
    EXPECT_THAT(js_code, testing::HasSubstr("Promise.all"));
}

TEST_F(AsyncExtensionTest, ParseTimeoutControl) {
    std::vector<Arg> args = {
        Arg{"slowOperation() within 5000"}
    };
    
    auto node = extension_->parse("timeout>>", args, mock_parser);
    
    ASSERT_NE(node, nullptr);
    auto js_code = extension_->generateJS(node.get());
    EXPECT_THAT(js_code, testing::HasSubstr("Promise.race"));
    EXPECT_THAT(js_code, testing::HasSubstr("5000"));
}
```

### 🔍 **集成测试**

```cpp
// tests/integration_test.cpp
TEST(IntegrationTest, FullAsyncWorkflow) {
    std::string chtl_js_source = R"(
        listen apiController = {
            loadUserData: async (userId) => {
                await>> fetchUser(userId) 
                    >> enrichUserData 
                    >> cacheUserData
                    >> updateUI;
            },
            
            loadDashboard: async () => {
                parallel>> [
                    fetchUserProfile(),
                    fetchUserPosts(),
                    fetchUserNotifications()
                ] then (results) => {
                    const [profile, posts, notifications] = results;
                    renderDashboard({ profile, posts, notifications });
                };
            }
        };
    )";
    
    CHTLJSCompiler compiler;
    compiler.registerExtension(std::make_unique<MyAsyncExtension>());
    
    auto result = compiler.compile(chtl_js_source);
    
    ASSERT_TRUE(result.success);
    EXPECT_THAT(result.javascript, testing::HasSubstr("async ()"));
    EXPECT_THAT(result.javascript, testing::HasSubstr("Promise.all"));
}
```

## 性能优化

### ⚡ **编译时优化**

```cpp
// src/optimizer.cpp
class AsyncCodeOptimizer {
public:
    std::string optimizeAsyncChain(const std::string& code) {
        std::string optimized = code;
        
        // 移除不必要的await
        optimized = removeRedundantAwaits(optimized);
        
        // 内联简单函数
        optimized = inlineSimpleFunctions(optimized);
        
        // 合并相邻的Promise.then调用
        optimized = mergeThenCalls(optimized);
        
        return optimized;
    }
    
private:
    std::string removeRedundantAwaits(const std::string& code) {
        // 实现去除冗余await的逻辑
        return code;
    }
    
    std::string inlineSimpleFunctions(const std::string& code) {
        // 实现简单函数内联的逻辑
        return code;
    }
    
    std::string mergeThenCalls(const std::string& code) {
        // 实现合并then调用的逻辑
        return code;
    }
};
```

### 📊 **运行时性能监控**

```cpp
// src/performance_monitor.cpp
class PerformanceMonitor {
private:
    std::unordered_map<std::string, std::chrono::milliseconds> operation_times_;
    
public:
    void startTimer(const std::string& operation) {
        start_times_[operation] = std::chrono::high_resolution_clock::now();
    }
    
    void endTimer(const std::string& operation) {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_times_[operation]);
        operation_times_[operation] = duration;
    }
    
    std::string generatePerformanceReport() const {
        std::stringstream js;
        
        js << "// CJMOD Performance Report\n";
        js << "const CJMOD_Performance = {\n";
        
        for (const auto& [operation, time] : operation_times_) {
            js << "  '" << operation << "': " << time.count() << ", // ms\n";
        }
        
        js << "};\n";
        
        return js.str();
    }
};
```

## 最佳实践

### ✅ **设计原则**

1. **性能优先**: 充分利用C++的性能优势
2. **类型安全**: 使用强类型和RAII原则
3. **错误处理**: 提供清晰的错误信息和恢复机制
4. **可扩展性**: 设计易于扩展的接口
5. **兼容性**: 确保与CHTL JS核心的兼容性

### 🎯 **代码规范**

```cpp
// 命名规范
class MyAsyncExtension;          // 类名：大驼峰
void parseAwaitChain();          // 方法名：小驼峰
std::string js_output_;          // 成员变量：小写+下划线
const int MAX_RETRIES = 5;       // 常量：全大写+下划线

// 错误处理
class CJMODException : public std::exception {
public:
    CJMODException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override { return message_.c_str(); }
private:
    std::string message_;
};

// 资源管理
std::unique_ptr<BaseJSNode> createNode() {
    return std::make_unique<AsyncChainNode>();  // 使用智能指针
}
```

### 🚫 **避免的反模式**

```cpp
// ❌ 避免：内存泄漏
BaseJSNode* createNode() {
    return new AsyncChainNode();  // 容易造成内存泄漏
}

// ✅ 推荐：智能指针
std::unique_ptr<BaseJSNode> createNode() {
    return std::make_unique<AsyncChainNode>();
}

// ❌ 避免：原始字符串拼接
std::string code = "async () => { return " + expr + "; }";

// ✅ 推荐：使用stringstream
std::stringstream js;
js << "async () => { return " << expr << "; }";
```

## 调试和诊断

### 🐛 **调试技巧**

```cpp
// 启用调试日志
#ifdef CJMOD_DEBUG
#define CJMOD_LOG(msg) std::cout << "[CJMOD] " << msg << std::endl
#else
#define CJMOD_LOG(msg)
#endif

// 在解析过程中添加日志
std::unique_ptr<BaseJSNode> MyAsyncExtension::parseAwaitChain(const std::vector<Arg>& args) {
    CJMOD_LOG("Parsing await chain with " << args.size() << " arguments");
    
    for (const auto& arg : args) {
        CJMOD_LOG("Processing argument: " << arg.content);
    }
    
    // ... 解析逻辑
}
```

### 📊 **性能分析**

```cpp
// 性能计时器
class ScopedTimer {
private:
    std::chrono::high_resolution_clock::time_point start_;
    std::string name_;
    
public:
    ScopedTimer(const std::string& name) : name_(name) {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_);
        std::cout << "[PERF] " << name_ << ": " << duration.count() << "μs" << std::endl;
    }
};

// 使用方法
std::unique_ptr<BaseJSNode> MyAsyncExtension::parse(...) {
    ScopedTimer timer("parse_await_chain");
    // ... 解析逻辑
}
```

---

通过本指南，您将能够创建强大的CJMOD扩展，为CHTL JS添加新的语法特性和功能。CJMOD的灵活性允许您实现几乎任何想要的JavaScript语法扩展。