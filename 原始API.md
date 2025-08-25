class Arg
{
    // 需要重载
    operator()(std::function)
    {

    }
    
}

class Syntax
{
    std::vector<Arg> args;
    std::string result;

    Template<typename T>
    void bind(const std::string& name, std::function<void(T)> func)
    {
        args.find(name)->bind(func);
    }
}

Scanner scanner = 统一扫描器.CJMODScanner();

Syntax keyword = syntaxAnalys("
    arg ** arg2 
");

举个例子  

keyword.arg[0]([](int num) {  // 接受一个数字
    return std::string(num);
});

keyword.arg[1] = []() {
    return **;
}

keyword.arg[2] = [](int num) {  // 接受一个数字
    return std::string(num);
}

scanner.scanKeyword(keyword.arg[1], []() {
    keyword.arg[0](peekKeyword(-1));  // 把内容传递给参数
    keyword.arg[2](peekKeyword(1));
});  // 扫描器接口，意思就是扫描到这个关键字时，做什么

generateCode(keyword) {
    std::string result = keyword.result();
    return result;
}

// 举个例子
printMylove({
    url: ,
    mode: ,  // 模式可以选择ASCII或Pixel
    width: ,  // 宽度，支持的单位有CSS单位以及百分比，小数，纯数字(像素)
    height: ,  // 高度
    scale:  ,  // 缩放倍数，限定为等比缩放策略
});

std::string str = ",:{};()";   // 需要忽略的符号
Syntax keyword = syntaxAnalys("
    printMylove({
        url: $,
        mode: $,  // 模式可以选择ASCII或Pixel
        width: $,  // 宽度，支持的单位有CSS单位以及百分比，小数，纯数字(像素)
        height: $,  // 高度
        scale:  $,  // 缩放倍数，限定为等比缩放策略
    });
" , str);

args = ["printMylove", "url","$", "mode", "width", "height", "scale"]  // 其他"$"省略

keyword.args.bind("url", [](std::string str){
        return str;
})

scanner.scanKeyword(keyword.arg[0], []() {
    keyword.args.match("url", peekKeyword(1));
});


我的设计如下
syntaxAnalys()函数能够将输入的字符串分成多个Keyword
它支持两个参数，一个参数用于接收字符串，一个参数用于设置不进行收集的内容

syntaxAnalys会返回一个对象，这个对象内部维护着args，是syntaxAnalys分析的字符串的关键字
开发者只需要使用bind方法就能够绑定采集函数
keyword.args.bind("url", [](std::string str) {
        return str;
});  
可以使用

接着就是扫描器，接收一个扫描的关键字，扫描到他的时候执行一个函数，这个函数会维护一些扫描用的方法，开发者可以通过他来获取关键字  
scanner.scanKeyword(keyword.args[0], []() {
    keyword.args.match("url", peekKeyword(1));
});
match方法就是如果匹配到参数是这个时，就执行参数的绑定函数
此时arg内部的value就会被填充

generateCode(keyword) {  
    std::string result = keyword.result();
    return result;
}
result()方法会把参数拼接起来，然后返回一个字符串
generateCode方法会返回最终的结果

如果参数值是一个函数 / 对象会怎么样？
    printMylove({
        url: () => {},
        mode: {},  // 模式可以选择ASCII或Pixel
        width: () => {},  // 宽度，支持的单位有CSS单位以及百分比，小数，纯数字(像素)
        height: () => {},  // 高度
        scale:  () => {},  // 缩放倍数，限定为等比缩放策略
    });
占位符是获取两个关键字之间的内容，只要确保前后有关键字即可  
std::string str = ",:{};()";   // 需要忽略的符号
const keyword = syntaxAnalys("
    printMylove({
        url: $,
        mode: $,  // 模式可以选择ASCII或Pixel
        width: $,  // 宽度，支持的单位有CSS单位以及百分比，小数，纯数字(像素)
        height: $,  // 高度
        scale:  $,  // 缩放倍数，限定为等比缩放策略
    });
" , str);



Syntax keyword = syntaxAnalys("
    arg ** arg2 
");

Syntax keyword2 = syntaxAnalys("
    $ ** $ 
");

std::string str = ",:{};()";   // 需要忽略的符号
const keyword = syntaxAnalys("
    printMylove({
        url: $,
        mode: $,  // 模式可以选择ASCII或Pixel
        width: $,  // 宽度，支持的单位有CSS单位以及百分比，小数，纯数字(像素)
        height: $,  // 高度
        scale:  $,  // 缩放倍数，限定为等比缩放策略
    });
" , str);

args = ["printMylove", "url","$", "mode", "width", "height", "scale"]  // 其他"$"省略

keyword.args.bind("url", [](std::string str){
        return str;
})

scanner.scanKeyword(keyword.arg[0], []() {
    keyword.args.match("url", peekKeyword(1));
});

scanner.scanKeyword(keyword.arg[1], []() {
    keyword.arg[0](peekKeyword(-1));  // 把内容传递给参数
    keyword.arg[2](peekKeyword(1));
});  // 扫描器接口，意思就是扫描到这个关键字时，做什么

generateCode(keyword) {
    std::string result = keyword.result();
    return result;
}

注意！有一些内容是珂朵莉模块，例如iNeverAway和printMyLove，CJMOD的内容，如果你想提前完成CJMOD也可以，我会为你提供CJMOD的原始API

## CJMOD API
### syntaxAnalys
syntaxAnalys函数用于分析CHTL JS语法，并分解成多个arg  

```cpp
const keyword = syntaxAnalys("
    arg ** arg2 
");

std::cout << keyword.args[0] << std::endl;  // arg
std::cout << keyword.args[1] << std::endl;  // **
std::cout << keyword.args[2] << std::endl;  // arg
```

syntaxAnalys函数具有一个可选择值，可以填入一个字符数组 / string  
这个字符数组 / string包含的字符将不会被视为arg

```cpp
std::string str = ",:{};()";   // 需要忽略的符号
const keyword = syntaxAnalys("
    printMylove({
        url: $,
        mode: $,  // 模式可以选择ASCII或Pixel
        width: $,  // 宽度，支持的单位有CSS单位以及百分比，小数，纯数字(像素)
        height: $,  // 高度
        scale:  $,  // 缩放倍数，限定为等比缩放策略
    });
" , str);
```

### 占位符
你可以使用$表示占位符，使用占位符能够更好地处理参数问题  

```cpp
const keyword2 = syntaxAnalys("
    $ ** $ 
");

std::cout << keyword.args[0] << std::endl;  // $
std::cout << keyword.args[1] << std::endl;  // **
std::cout << keyword.args[2] << std::endl;  // $
```

### bind
要为这些参数传递值，需要使用bind函数，bind决定了参数将会接收什么值，怎么接收值  
bing函数会自动搜索相关的参数并绑定函数  
如果你不想使用自动搜索，你也可以通过索引去绑定  

```cpp
keyword.args.bind("url", [](const std::string& str) {  // 对于占位符，你需要多次重复绑定，bind函数会自动计数占位符，重复写的语句会自动绑定到下一个$
    return str;
});

keyword.args[1].bind([]() {
    return x;
});
```

### scanKeyword
scanKeyword函数是扫描器暴露的接口，是扩展CHTL JS的核心，你需要使用scanKeyword指示语法的核心关键字，这是CHTL JS语法得到支持的关键一步  

```cpp
scanner.scanKeyword(keyword.args[1], []() {
    keyword.arg[0](peekKeyword(-1));  // 把内容传递给参数
    keyword.arg[2](peekKeyword(1));
})
```

### match
match函数用于匹配相应的arg，传递值并调用函数  
也可以直接使用match  

```cpp
keyword.args.match("url", peekKeyword(-1));  // 对于占位符，重复绑定和bind的行为一样

keyword.args[0].match(peekKeyword(-1));
```

### policy
policy系列函数用于改变扫描政策  
policy系列函数在处理复杂的参数值时会很有用  
政策有  
NORMAL 常规  
COLLENT 收集  
SKIP 跳过  

```cpp
/*

printMylove({
    url: $,
    mode: $,  // 模式可以选择ASCII或Pixel
    width: () => {},  // 宽度，支持的单位有CSS单位以及百分比，小数，纯数字(像素)
    height: {},  // 高度
    scale:  $,  // 缩放倍数，限定为等比缩放策略
});

*/

// 如果参数值传递的是函数 / 对象如何解决？你可以改变扫描器的策略
scanner.scanKeyword(printMylove, []() {
    std::string result[] = {};

    for (int i = 0; i < keyword.args.length(); i++)
    {
        scanner.policyChangeBegin(":", Policy::COLLENT);  // 遇到":"后，策略更改，收集所有的内容
        const std::string& str = scanner.policyChangeEnd("}", Policy::NORMAL);  // 遇到"}"后，函数结束，停止收集，并返回string
        result[i] = str;
    }

    keyword.args.match("width", result[2]);
    keyword.args.match("height", result[3]);
});
```

### transform(需要进行修正)
transform函数用于将arg转变为JS代码  

```cpp
keyword.args.transform("url", "URL url = new URL()");
keyword.args[0].transform("URL url = new URL()");
```

### result
result函数用于将转变为JS代码后的arg组合起来  

```cpp
keyword.args.result();
```

### generateCode
generateCode函数用于生成JS代码  

```cpp
generateCode(keyword);  // 直接写即可，只要前面步骤不错的话
```

### isObject
辅助函数，判断是不是对象  

### isFunction
辅助函数，判断是不是函数  

### slice
辅助函数，切割字符串  
先前用来处理函数与对象  

### peekKeyword
辅助函数，获取前后关键字的内容  

## 总结

流程  
syntaxAnalys(拆分成多个arg) -> bind(绑定获取值的函数) -> transform(最终的JS代码) -> scanKeyword(什么关键字作为统一扫描器的搜索) -> match(捕获参数值，这个要拿到代码片段才行，否则无法使用peekKeyword) -> result(组合所有参数的最终JS代码) -> generateCode(把代码交给生成器，生成器将代码输出为标准js)  

严格根据CHTL语法文档与项目架构，相关源代码的实现完成此阶段  

后续工作，对仓库进行全面清理，清理成发布状态  
然后对代码进行去重，合并，优化文件名称(现在的文件命名一坨)  
接着是在script下创建三个平台的跨平台脚本文件夹  
由于兼容性问题，只能使用英文  
创建release构建脚本  
创建debug构建脚本  
创建统一构建脚本(构建编译器，打包Module文件夹中符合模块特征的文件夹，构建VSCode插件)  
创建VSCode插件构建脚本(只构建VSCode插件)  
创建CMOD打包脚本  ->  特别要留意语法文档  
创建CJMOD打包脚本   
创建统一打包脚本  
创建构建清理脚本  

下述使用中文  
创建CHTL项目概述  
创建CHTL编译器开发指南  
创建CHTL JS编译器开发指南  
创建CHTL项目构建指南  
创建MIT开源协议条款  
创建CHTL语法文档  
创建CHTL JS语法文档  
创建CMOD开发指南  
创建CJMOD开发指南  
创建项目环境依赖文档  
创建统一扫描器架构说明  
创建珂朵莉模块使用文档  
创建由比滨结衣模块使用文档    
创建项目贡献指南  
创建编译计时器说明  


全程使用中文模式，阅读CHTL语法文档，基于CHTL语法文档，创建下述架构，
使用编程语言，C++ 17，全程听我指示开发  

我们不建议各个编译器共用文件，而是每一个编译器具有一套文件  
我们要求每一次都是完整实现，每一次任务完成之后项目都能够直接编译成功  

┌─────────────────────────────────────────────────────────────────┐
│                         CHTL源代码                               │
└─────────────────────────────┬───────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                    CHTLUnifiedScanner                           │
│                   (精准代码切割器)                               │
└─────────────────────────────┬───────────────────────────────────┘
                              │
                              ▼
        ┌──────────────┬──────────────┬──────────────┬────────────┐
        │   CHTL片段   │ CHTL JS片段  │   CSS片段    │   JS片段   │
        └──────┬───────┴──────┬───────┴──────┬───────┴──────┬─────┘
               │              │              │              │
               ▼              ▼              ▼              ▼
┌─────────────────────────────────────────────────────────────────┐
│                    CompilerDispatcher                           │
│                   (编译器调度器)                                 │
└──────┬──────────────┬──────────────┬──────────────┬────────────┘
       │              │              │              │
       ▼              ▼              ▼              ▼
┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐
│    CHTL     │ │   CHTL JS   │ │     CSS     │ │JavaScript   │
│  Compiler   │ │  Compiler   │ │  Compiler   │ │  Compiler   │
│  (手写)     │ │  (手写)     │ │  (ANTLR)    │ │  (ANTLR)    │
└─────────────┘ └─────────────┘ └─────────────┘ └─────────────┘
       │              │              │              │
       └──────────────┴──────────────┴──────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                      编译结果合并                                │
│                     (HTML输出)                                   │
└─────────────────────────────────────────────────────────────────┘

编译器对应关系如下  
局部style  ->  CHTL编译器  
全局style  ->  CSS编译器  
script  ->  由CHTL，CHTL JS，JS编译器共同管理  

注意！扫描器是这个统一架构的基础，扫描器需要基于可变长度切片进行工作  
例如，扫描器将一次读取一个片段的代码，并检查是否下一个片段的开头是否可能组成了一个完整的CHTL或CHTL JS代码片段  
如果组成，说明截断的位置不合理，需要向前扩增，扩增一定的长度  
如果截断的位置合理，则对切片进行根据CHTL / CHTL JS的最小单元进行二次切割，确保结果绝对精确  
例如{{box}}->click将被切割为{{box}}->和click  
CHTL和CHTL JS代码虽然要以最小单元为切割标准，但是仍要基于上下文，确保代码片段不会过小，让多个CHTL和CHTL JS代码片段连续而非全部细分为最小单元  
这样的机制允许JS和CSS编译器在无需特别处理的情况下支持CHTL和CHTL JS的特征  
后续会说明相关方面的约束  
CHTL代码几乎总是以块存在，因此可以收集块来推送给编译器，但是有一些特殊的地方，例如全局style中允许的chtl语法，他们也应该被正确分离，例如局部script，全局script，局部script允许使用部分chtl语法，完全的chtl js和js语法，这意味着分离规则需要特别仔细，而全局script则允许完全的chtl js和js语句，因此要做到以chtl和chtl js作为切割的点位，如果没有切割到chtl和chtl js这些语法，那么毫无疑问的说，前面绝对是纯净的代码，这就是宽判的由来，要做到以正确处理大块的chtl，又做到处理允许的那些chtl语法，而chtl js和js相互混杂，chtl js本身具有特殊性，其提供的函数内部甚至可能具有js代码，因此最小单元是绝对需要的，那么除了chtl，chtl js外，毫无疑问，剩下的就是js代码