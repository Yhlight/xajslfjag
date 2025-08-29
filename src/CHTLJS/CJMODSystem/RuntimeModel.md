# CJMOD运行时模型

## 核心问题
CJMOD扩展需要在CHTL JS编译时处理语法扩展，但C++扩展是编译后的动态库。

## 解决方案：两阶段处理

### 第一阶段：语法定义（编译时）
- 使用JSON格式的语法定义文件
- 在编译时由CHTL JS编译器读取和解析
- 不需要执行C++代码

### 第二阶段：代码生成（运行时）
- 当匹配到自定义语法时，调用C++扩展
- 扩展返回生成的JavaScript代码
- 插入到最终输出中

## 实现方式

```cpp
// CJMOD扩展接口
class CJMODExtension {
public:
    // 编译时调用：返回语法定义
    virtual std::string getSyntaxDefinition() = 0;
    
    // 运行时调用：处理匹配的代码片段
    virtual std::string processFragment(
        const std::string& fragment,
        const std::map<std::string, std::string>& params
    ) = 0;
};
```

## 工作流程

1. **加载阶段**
   - 加载.cjmod文件
   - 提取syntax/*.json语法定义
   - 加载extension.so/dll但不执行

2. **编译阶段**
   - CHTL JS扫描器识别代码
   - 匹配JSON中定义的语法模式
   - 标记需要扩展处理的片段

3. **处理阶段**
   - 对每个标记的片段
   - 动态调用对应的C++扩展
   - 获取生成的代码
   - 替换原始片段

## 示例

### syntax/animate.json
```json
{
    "name": "animate",
    "pattern": "animate\\s*\\{([^}]*)\\}",
    "processor": "processAnimate",
    "parameters": {
        "captureGroups": ["body"]
    }
}
```

### extension.cpp
```cpp
class AnimateExtension : public CJMODExtension {
public:
    std::string processFragment(
        const std::string& fragment,
        const std::map<std::string, std::string>& params
    ) override {
        // 解析animate块
        auto body = params.at("body");
        
        // 生成JavaScript代码
        return generateAnimationCode(body);
    }
};
```