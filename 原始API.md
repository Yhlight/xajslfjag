## CJMOD
### 简介
CJMOD是CHTL项目CHTL JS模块的扩展组件，用于提供模块化分发CHTL JS代码  
CHTL提供了CJMOD API，CJMOD API极其强大，能够高效创建CHTL JS语法  

### CJMOD API
#### 使用案例
```cpp
Arg args = Syntax::analyze("$ ** $");  // 语法分析
args.print();  // 输出-> ["$", "**", "$"]

args.bind("$", [](const std::string& value) {
    return value;
});

args.bind("**", [](const std::string& value) {
    return value;
});

args.bind("$", [](const std::string& value) {
    return value;
});

Arg result = CJMODScanner::scan(args, "**");
result.print();  // 输出-> ["3", "**", "4"]

args.fillValue(result);
std::cout << arg[0].value << std::endl;  // 输出-> 3
std::cout << arg[1].value << std::endl;  // 输出-> **
std::cout << arg[2].value << std::endl;  // 输出-> 4

args.transform("pow(" + arg[0].value + arg[2].value + ")");

CJMODGenerator::exportResult(args);
```

```chtl
script
{
    console.log(3 ** 4);
}

->

<script>
    console.log(pow(3, 4));
</script>
```

##### Syntax
语法分析类，这个类负责对语法进行解析  

###### analyze
分析语法，返回一个Arg对象，Arg对象包含了解析出的参数列表  

```cpp
Arg args = Syntax::analyze("$ ** $");  // 语法分析
args.print();  // 输出-> ["$", "**", "$"]
```

###### isObject
判断是否是JS对象  

```cpp
Syntax::isObject("{b: 1}");  // 输出-> true
```

###### isFunction
判断是否是JS函数  

```cpp
Syntax::isFunction("function a(){}");  // 输出-> true
```

###### isArray
判断是否是JS数组  

```cpp
Syntax::isArray("[1, 2, 3]");  // 输出-> true
```

###### isCHTLJSFunction
判断是否是CHTL JS函数  

```cpp
Syntax::isCHTLJSFunction("test {test: 1, test2: 2};");  // 输出-> true
```

##### Arg
参数列表类，这个类包含了解析出的参数列表，并且提供了参数的绑定、填充、转换等操作  

###### bind
让一个原子Arg绑定获取值的函数  

```cpp
Arg args = Syntax::analyze("$ ** $");  // 语法分析
args.bind("$", [](const std::string& value) {
    return value;
});

args.bind("**", [](const std::string& value) {
    return Syntax::isCHTLJSFunction(value) ? "" : value;
});
```

###### fillValue
填充参数列表的值  

```cpp
Arg result = CJMODScanner::scan(args);
args.fillValue(result);
args.fillValue(Arg(["3", "**", "4"]));
```

###### transform
参数最终输出什么JS代码  

```cpp
args.transform("pow(" + arg[0].value + arg[2].value + ")");
```

##### CJMODScanner
统一扫描器用于CJMOD API的接口  

###### scan
扫描语法片段，第二参数为扫描的关键字  

```cpp
Arg result = CJMODScanner::scan(args, "**");
```

#### CJMODGenerator
生成器用于CMJMOD API的接口  

###### exportResult
导出最终的JS代码  

```cpp
CJMODGenerator::exportResult(args);
```

#### AtomArg
原子参数，Arg封装此    

##### $
占位符  

##### $?
可选占位符  

##### $!
必须占位符  

##### $_
无序占位符  

上述占位符可以组合  
例如$!_  

##### ...
不定参数占位符  

##### bind
绑定获取值的函数  

```cpp
args[0].bind([](const std::string& value) {return value;});
```

##### fillValue
填充参数值  

```cpp
args[0].fillValue("3");
args[0].fillValue(3);
```

#### CHTLJSFunction
CHTL JS函数，用于CJMOD API的接口  

##### CreateCHTLJSFunction
封装了原始API构建语法的流程，能够快速构建CHTL JS函数，这些CHTL JS函数天然支持虚对象vir以及无修饰字符串    

```cpp
CHTLJSFunction func;
CHTLJSFunction::CreateCHTLJSFunction("printMyLove {url: $!_, mode: $?_}");
```

```chtl
script
{
    printMyLove({url: "https://www.baidu.com", mode: "auto"});
}

天然支持vir  
script
{
    vir test = printMyLove({url: "https://www.baidu.com", mode: "auto"});
}
```

##### bindVirtualObject
绑定虚对象vir  
对于不使用CreateCHTLJSFunction创建的，但是符合CHTL JS函数的语法  
可以使用bindVirtualObject手动绑定虚对象vir，获得虚对象的支持  

```cpp
Syntax::isCHTLJSFunction("printMyLove {url: $!_, mode: $?_}");  // 输出-> true
CHTLJSFunction::bindVirtualObject("printMyLove");  // 写函数名称 
```
