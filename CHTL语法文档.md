# 关于CHTL
CHTL是基于C++语言实现的超文本语言，其本质是为了提供一种更符合开发者编写HTML代码的方式，使用MIT开源协议  

## 注释
在CHTL中，使用//表示注释  
使用/**/代表多行注释  
使用--代表会被生成器识别的注释  
//和/**/注释不会被生成器所识别，生成的HTML不会带有这些注释  
而--注释则会根据上下文生成不同编程语言类型的注释  

## 文本节点
在CHTL中，使用text { }表示一段文本

```chtl
text
{
    "这是一段文本"
}
```

## 字面量
CHTL支持text与属性值使用无修饰字面量(没有引号的字符串)，这允许你像CSS一样无需书写引号  
除此之外，CHTL支持双引号字符串("")，单引号字符串('')  

```chtl
text
{
    这是一段文本
}

style
{
    color: red;
}
```

## CE对等式
CE对等式即Colon = Equal  
即在CHTL之中，':'与'='完全等价  
你可以在一些推荐的情景下使用'='  

## 元素节点
CHTL支持HTML所有的元素，无论是单标签还是双标签，还是块级，行内，行内块元素  

```chtl
html
{
    head
    {

    }

    body
    {
        div
        {
            text
            {

            }
        }

        span
        {

        }
    }
}
```

## 属性
在CHTL中，使用`属性名 : "属性值";`表示属性  

```chtl
div
{
    id: box;
    class: welcome;

    text
    {
        HelloWorld
    }
}
```

## 局部样式块
CHTL对<style></style>进行了改进，允许开发者在元素的内部嵌套style {}，以此进行一些css操作  
包括但不限于内联样式，类选择器，id选择器，伪类选择器，伪元素选择器  
类选择器，id选择器，伪类选择器，伪元素选择器相关的代码将会被自动添加至全局样式块之中  
注意！CHTL并没有对全局样式块进行增强，请不要尝试在全局样式块使用局部样式块的功能  

### 内联样式
你可以在style{}内部直接添加属性，这些属性会成为元素的内联样式  

```chtl
body
{
    div
    {
        // 内联样式
        style
        {
            width: 100px;
            height: 200px;
        }
    }
}
```

### 自动化类名 / id
你可以直接在局部样式块中使用类 / id选择器  
无需手动编写class / id  
CHTL会自动为元素添加类名 / id  

```chtl
div
{
    style
    {
        .box  // 自动添加类名box给元素，且整部分代码将自动添加至全局样式块
        {
            width: 300px;
        }
    }
}
```

### 上下文推导
你可以在局部样式块之中使用&表示类名 / id(优先类名)  
&将会根据上下文来推导成类名 / id，例如检查元素的class / id  

这里有两种使用方式，一种是上文使用class / id  
下文使用&，&:hover，&::before代替类 / id选择器  
CHTL并不建议使用这一种方式  

而是建议下述这一种使用方式  
不写元素的class / id，直接使用.box / #box，让CHTL自动添加类名 / id  
将&用于简化伪类选择器(&hover)与伪元素选择器(&::before)的使用  

```chtl
div
{
    style
    {
        .box  // 让CHTL自动添加类名
        {

        }

        &:hover  // 使用&简化伪类的使用，之后&会被解析成类名，然后整体被添加至全局样式块
        {

        }
    }
}
```

## 模板
你可以使用[Template]来创建模板，在CHTL之中，存在样式组模板，元素模板，变量组模板  

### 样式组模板
使用`[Template] @Style 组名`来创建样式组模板  

```chtl
[Template] @Style DefaultText
{
	color: "black";
	line-height: 1.6;
}

div
{
    style
    {
        @Style DefaultText;  // 使用样式组模板，模板之中所有的属性都会被添加到div身上
    }
}
```

### 元素模板
使用`[Template] @Element 元素名`来创建元素模板  

```chtl
[Template] @Element Box
{
    span
    {
        text
        {
            这是一组div
        }
    }

    div
    {
        style
        {
            width: 200px;
            height: 200px;
            background-color: red;
        }
    }

    div
    {
        style
        {
            width: 200px;
            height: 200px;
            background-color: blue;
        }
    }
}

body
{
    @Element Box;  // 使用元素模板
}
```

### 变量组模板
使用`[Template] @Var 变量组名`来创建变量组名  

```chtl
[Template] @Var ThemeColor
{
    // 无需像CSS一样以--为前缀，因为变量组本质上不是CSS变量，而是值的替换
    tableColor: "rgb(255, 192, 203)";
}

[Template] @Element Box
{
    div
    {
        style
        {
            color: ThemeColor(tableColor);  // 使用模板变量组，无需以@Var为前缀，可以写，但不必要
        }
    }
}

body
{
    div
    {
        style
        {
            background-color: ThemeColor(tableColor);
        }
    }
}
```

### 继承
模板可以继承，无论什么类型，都能够继承同种类型的模板  

```chtl
[Template] @Style ThemeColor
{
    color: rgba(255, 192, 203, 1);
    background-color: rgba(253, 144, 162, 1);
}

[Template] @Style ThemeColor2
{
    background-color: yellow;  // 重复的元素，根据书写的顺序进行值的替换
    @Style ThemeColor;  // 样式组模板继承，获得ThemeColor所有的属性
}
```

#### 显性继承
除了组合式继承，你还可以使用inherit显式继承  

```chtl
[Template] @Style ThemeColor
{
    color: rgba(255, 192, 203, 1);
    background-color: rgba(253, 144, 162, 1);
}

[Template] @Style ThemeColor2
{
    background-color: yellow;
    inherit @Style ThemeColor;
}
```

## 自定义
自定义是模板的的扩展，自定义具有极高的灵活性以及更多的扩展操作  
你可以使用[Custom]创建自定义内容  
自定义与模板之间最大的差别就是自定义允许特例化操作  
模板与自定义之间允许相互继承，只要类型匹配  

### 自定义样式组
#### 无值样式组
自定义样式组允许样式组属性不具有值，开发者需要在使用时自行填入值  

```chtl
[Custom] @Style TextSet
{
    color,
    font-size;
}

[Custom] @Style WhiteText
{
    @Style TextSet
    {
        color: white;
        font-size: 16px;
    }
}

div
{
    style
    {
        @Style TextSet
        {
            color: red;
            font-size: 16px;
        }
    }
}
```

#### 样式组的特例化
##### 删除属性
```chtl
[Template] @Style WhiteText
{
    color: white;
    font-size: 16px;
    line-height: 1.6;
    border: 1px black soild;
}

[Custom] @Style YellowText
{
    @Style WhiteText
    {
        delete line-height, border;
    }
    color: yellow;
}

div
{
    style
    {
        @Style YellowText
        {
            delete color;
        }
    }
}
```

##### 删除样式组继承
```chtl
[Template] @Style WhiteText
{
    color: white;
    font-size: 16px;
    line-height: 1.6;
    border: 1px black soild;
}

[Custom] @Style YellowText
{
    @Style WhiteText
    color: yellow;
}

div
{
    style
    {
        @Style YellowText
        {
            delete @Style WhiteText;
        }
    }
}
```

### 自定义元素
#### 自定义元素的特例化
##### 增加样式
```chtl
[Custom] @Element Box
{
    div
    {

    }

    div
    {

    }

    span
    {

    }
}

body
{
    @Element Box
    {
        span
        {
            // 为span添加样式
            style
            {

            }
        }

        div
        {

        }

        div
        {
            // 为第二个div添加样式
            style
            {

            }
        }
    }
}
```

##### 索引访问
你可以使用[index]索引来访问自定义元素中的某一个元素  

```chtl
[Custom] @Element Box
{
    div
    {

    }

    div
    {

    }

    span
    {

    }
}

body
{
    @Element Box
    {
        div[1]  // 索引访问
        {
            // 为第二个div添加样式
            style
            {

            }
        }
    }
}
```

##### 插入元素
使用`insert 位置 选择器`插入元素  

```chtl
[Custom] @Element Box
{
    div
    {

    }

    div
    {

    }

    span
    {

    }
}

body
{
    @Element Box
    {
        // 在第一个div后面插入内容
        insert after div[0] {  // 可选关键字，after，before，replace，at top / at bottom(可以不使用div[0])
            div
            {
                style
                {

                }
            }

            @Element Line;
        }
    }
}
```

##### 删除元素
```chtl
[Custom] @Element Box
{
    div
    {

    }

    div
    {

    }

    span
    {

    }

    div
    {

    }
}

body
{
    @Element Box
    {
        delete span;
        delete div[1];
    }
}
```

##### 删除元素继承
```chtl
[Custom] @Element Box
{
    div
    {

    }

    div
    {

    }

    span
    {

    }

    @Element Line;
}

body
{
    @Element Box
    {
        delete @Element Line;
    }
}
```

### 变量组
#### 变量组特例化
```chtl
[Custom] @Var ThemeColor
{
    tableColor: "rgb(255, 192, 203)";
    TextColor: "black";
}

div
{
    style
    {
        color: ThemeColor(tableColor = rgb(145, 155, 200));
    }
}
```

### 全缀名
CHTL允许使用全缀名来访问模板元素，样式组，变量组，自定义元素，样式组，变量组  
这在处理命名冲突时非常有用  

```chtl
[Custom] @Element Box
{
    div
    {

    }

    div
    {

    }
}

body
{
    @Element Box;  // 使用全缀名访问自定义元素Box

    [Custom] @Element Box;  // 使用全缀名访问自定义元素Box
}
```

## 原始嵌入
在CHTL中，你可以使用[Origin]表示这是一段原始的代码，这部分代码不会被CHTL处理，而是让生成器直接生成  
原始嵌入是CHTL的兼容处理机制，避免CHTL考虑不到的极端问题  
原始嵌入允许在任意节点中被解析  
原始嵌入是直接把内容进行输出，绝对不会进行处理  

### 嵌入HTML代码
```chtl
[Origin] @Html
{

}

body
{
    [Origin] @Html
    {

    }
}
```

### 嵌入CSS代码
```chtl
[Origin] @Style
{

}
```

### 嵌入JS代码
```chtl
[Origin] @JavaScript
{

}
```

### 增强原始嵌入
#### 带名原始嵌入
你可以为原始嵌入块赋予一个名称  
```chtl
[Origin] @Html box
{

}

body
{
    [Origin] @Html box;
}
```

#### 自定义类型系统
@Html，@Style，@Javascript是CHTL中基本的类型，CHTL只为原始嵌入提供了这三种类型  
如果你需要更多的类型，你可以直接创建你想要的类型的原始嵌入，注意！必须以@为前缀    

```chtl
[Origin] @Vue box
{

}

body
{
    [Origin] @Vue box;
}
```

自定义类型系统会隐式创建一个配置块，你可以在配置组之中了解到这一个配置块  

## 配置组
配置组允许开发者自定义很多行为

```chtl
[Configuration]
{
    // 索引的起始计数
    INDEX_INITIAL_COUNT = 0;
    // 是否禁用Name配置组(即是否允许自定义关键字名称)
    DISABLE_NAME_GROUP = true;
    // 是否禁用自定义原始嵌入类型
    DISABLE_CUSTOM_ORIGIN_TYPE = false;

    // DEBUG模式
    DEBUG_MODE = false;

    // Name配置块，存放关键字名称用
    [Name]
    {
        // 这里可以定义关键字的名称，无需改动源码即可实现支持
        // 组选项，即CUSTOM_STYLE具有多个值
        CUSTOM_STYLE = [@Style, @style, @CSS, @Css, @css];
        CUSTOM_ELEMENT = @Element;
        CUSTOM_VAR = @Var;
        TEMPLATE_STYLE = @Style;
        TEMPLATE_ELEMENT = @Element;
        TEMPLATE_VAR = @Var;
        ORIGIN_HTML = @Html;
        ORIGIN_STYLE = @Style;
        ORIGIN_JAVASCRIPT = @JavaScript;
        IMPORT_HTML = @Html;
        IMPORT_STYLE = @Style;
        IMPORT_JAVASCRIPT = @JavaScript;
        IMPORT_CHTL = @Chtl;
        IMPORT_CRMOD = @CJmod;
        KEYWORD_INHERIT = inherit;
        KEYWORD_DELETE = delete;
        KEYWORD_INSERT = insert;
        KEYWORD_AFTER = after;
        KEYWORD_BEFORE = before;
        KEYWORD_REPLACE = replace;
        KEYWORD_ATTOP = at top;
        KEYWORD_ATBOTTOM = at bottom;
        KEYWORD_FROM = from;
        KEYWORD_AS = as;
        KEYWORD_EXCEPT = except;
        KEYWORD_TEXT = text;
        KEYWORD_STYLE = style;  // 局部样式块
        KEYWORD_SCRIPT = script;  // 局部脚本块
        KEYWORD_CUSTOM = [Custom];
        KEYWORD_TEMPLATE = [Template];
        KEYWORD_ORIGIN = [Origin];
        KEYWORD_IMPORT = [Import]
        KEYWORD_NAMESPACE = [Namespace]

        // 组选项的数量限制，避免在大型项目中对性能的过高消耗
        OPTION_COUNT = 3;
    }

    [OriginType]  // 定义Origin具有什么自定义类型
    {
        ORIGINTYPE_VUE = @Vue;  // 如果我创建了一个@Vue类型的原始嵌入，那么CHTL会隐式创建

        // 你也可以显式创建，更明确说明，但是要注意  
        // ORIGINTYPE_VUE = @Vue;
        // ORIGINTYPE_MARKDOWN = @Markdown;
        // CHTL强制要求使用ORIGINTYPE_全写的类型名称 = @全大写后 = 全写的类型名称的内容
    }
}
```

### 命名配置组
你现在可以为配置组命名，命名配置组不会被使用，不被命名的配置组才会启用，如果存在多个无名的配置组则冲突，命名配置组可以创建多个，这是因为命名配置组服务于导入[Import]  

```chtl
[Configuration] @Config Basic
{
    INDEX_INITIAL_COUNT = 0;
    DISABLE_NAME_GROUP = true;
    DISABLE_CUSTOM_ORIGIN_TYPE = false;
    DEBUG_MODE = false;

    [Name]
    {
        CUSTOM_STYLE = [@Style, @style, @CSS, @Css, @css];
        CUSTOM_ELEMENT = @Element;
        CUSTOM_VAR = @Var;
        TEMPLATE_STYLE = @Style;
        TEMPLATE_ELEMENT = @Element;
        TEMPLATE_VAR = @Var;
        ORIGIN_HTML = @Html;
        ORIGIN_STYLE = @Style;
        ORIGIN_JAVASCRIPT = @JavaScript;
        CONFIGURATION_CONFIG = @Config;  // 新增
        IMPORT_HTML = @Html;
        IMPORT_STYLE = @Style;
        IMPORT_JAVASCRIPT = @JavaScript;
        IMPORT_CHTL = @Chtl;
        IMPORT_CRMOD = @CJmod;
        IMPORT_CONFIG = @Config;  // 新增
        KEYWORD_INHERIT = inherit;
        KEYWORD_DELETE = delete;
        KEYWORD_INSERT = insert;
        KEYWORD_AFTER = after;
        KEYWORD_BEFORE = before;
        KEYWORD_REPLACE = replace;
        KEYWORD_ATTOP = at top;
        KEYWORD_ATBOTTOM = at bottom;
        KEYWORD_FROM = from;
        KEYWORD_AS = as;
        KEYWORD_EXCEPT = except;
        KEYWORD_TEXT = text;
        KEYWORD_STYLE = style;
        KEYWORD_SCRIPT = script;
        KEYWORD_CUSTOM = [Custom];
        KEYWORD_TEMPLATE = [Template];
        KEYWORD_ORIGIN = [Origin];
        KEYWORD_IMPORT = [Import]
        KEYWORD_NAMESPACE = [Namespace]
        OPTION_COUNT = 3;
    }

    [OriginType]
    {
        ORIGINTYPE_VUE = @Vue;
    }
}

[Configuration] @Config Std
{
    INDEX_INITIAL_COUNT = 0;
    DISABLE_NAME_GROUP = true;
    DISABLE_CUSTOM_ORIGIN_TYPE = false;
    DEBUG_MODE = false;

    [Name]
    {
        CUSTOM_STYLE = [@Style, @style, @CSS, @Css, @css];
        CUSTOM_ELEMENT = @Element;
        CUSTOM_VAR = @Var;
        TEMPLATE_STYLE = @Style;
        TEMPLATE_ELEMENT = @Element;
        TEMPLATE_VAR = @Var;
        ORIGIN_HTML = @Html;
        ORIGIN_STYLE = @Style;
        ORIGIN_JAVASCRIPT = @JavaScript;
        CONFIGURATION_CONFIG = @Config;
        IMPORT_HTML = @Html;
        IMPORT_STYLE = @Style;
        IMPORT_JAVASCRIPT = @JavaScript;
        IMPORT_CHTL = @Chtl;
        IMPORT_CRMOD = @CJmod;
        IMPORT_CONFIG = @Config;
        KEYWORD_INHERIT = inherit;
        KEYWORD_DELETE = delete;
        KEYWORD_INSERT = insert;
        KEYWORD_AFTER = after;
        KEYWORD_BEFORE = before;
        KEYWORD_REPLACE = replace;
        KEYWORD_ATTOP = at top;
        KEYWORD_ATBOTTOM = at bottom;
        KEYWORD_FROM = from;
        KEYWORD_AS = as;
        KEYWORD_EXCEPT = except;
        KEYWORD_TEXT = text;
        KEYWORD_STYLE = style;
        KEYWORD_SCRIPT = script;
        KEYWORD_CUSTOM = [Custom];
        KEYWORD_TEMPLATE = [Template];
        KEYWORD_ORIGIN = [Origin];
        KEYWORD_IMPORT = [Import]
        KEYWORD_NAMESPACE = [Namespace]
        OPTION_COUNT = 3;
    }

    [OriginType]
    {
        ORIGINTYPE_VUE = @Vue;
    }
}
```

## 导入
你可以使用[Import]导入CHTL，HTML，CSS，JS，CJMOD文件  

导入HTML文件  
[Import] @Html from html文件路径 as(必须) 命名为  

导入CSS文件  
[Import] @Style from css文件路径 as(必须) 命名为  

导入JS文件  
[Import] @JavaScript from js文件路径 as(必须) 命名为  // 如果不具有as，直接跳过，具有as，则是创建命名原始嵌入节点  

对于上述三种类型  
如果写的是文件名（不带后缀）：在编译文件所在目录（非递归）按类型搜索相关文件  
具体文件名（带后缀）：在编译文件所在目录（非递归）直接搜索该文件  
如果路径为文件夹或不包含具体文件信息时，触发报错  

导入另一个chtl文件之中的自定义元素  
[Import] [Custom] @Element 需要导入的自定义元素名 from chtl文件路径 as(可选) 命名为  

导入另一个chtl文件之中的自定义样式组  
[Import] [Custom] @Style 需要导入的样式组名称 from chtl文件路径 as(可选) 命名为  

导入另一个chtl文件之中的自定义变量组  
[Import] [Custom] @Var 需要导入的变量组名称 from chtl文件路径 as(可选) 命名为  

导入另一个chtl文件之中的元素模板  
[Import] [Template] @Element 需要导入的自定义元素名 from chtl文件路径 as(可选) 命名为  

导入另一个chtl文件之中的样式组模板  
[Import] [Template] @Style 需要导入的样式组名称 from chtl文件路径 as(可选) 命名为  

导入另一个chtl文件之中的变量组模板  
[Import] [Template] @Var 需要导入的变量组名称 from chtl文件路径 as(可选) 命名为  

导入一个chtl文件  
[Import] @Chtl from chtl文件路径  

导入CJmod文件  
[Import] @CJmod from cjmod文件路径  

[Import] @Config 导入的配置组名称(可选，不写默认导入无名配置组，如果没有则是第一个有名) from chtl文件路径 as(无用选项) 命名为  

全缀名写法  
[Import] [Configuration] @Config 导入的配置组名称(可选，不写默认导入无名配置组，如果没有则是第一个有名) from chtl文件路径 as(无用选项) 命名为  

导入原始嵌入  
[Import] [Origin] @Html 名称 from chtl文件路径 as(可选) 命名为  
[Import] [Origin] @Style 名称 from chtl文件路径 as(可选) 命名为  
[Import] [Origin] @Javascript 名称 from chtl文件路径 as(可选) 命名为  

导入所有模板  
[Import] [Template] from chtl文件路径 as(无效) 命名为  

导入所有自定义  
[Import] [Custom] from chtl文件路径 as(无效) 命名为  

导入所有命名原始嵌入(注意！命名)  
[Import] [Origin] from chtl文件路径 as(无效) 命名为  

导入另一个chtl文件之中所有的自定义元素  
[Import] [Custom] @Element from chtl文件路径 as(无效) 命名为  

导入另一个chtl文件之中所有的自定义样式组  
[Import] [Custom] @Style from chtl文件路径 as(无效) 命名为   

导入另一个chtl文件之中所有的自定义变量组  
[Import] [Custom] @Var from chtl文件路径 as(无效) 命名为   

导入另一个chtl文件之中所有的元素模板  
[Import] [Template] @Element from chtl文件路径 as(无效) 命名为  

导入另一个chtl文件之中所有的样式组模板  
[Import] [Template] @Style from chtl文件路径 as(无效) 命名为  

导入另一个chtl文件之中所有的变量组模板  
[Import] [Template] @Var from chtl文件路径 as(无效) 命名为  

可以使用'.'来表示'/'  

对于@Chtl类型来说  
名称（不带后缀）：优先搜索官方模块目录(源码编译后生成的module文件夹，通常和编译器同一个文件夹，含cmod，chtl和cjmod文件），其次搜索编译文件所在的目录module文件夹，最后搜索编译文件所在目录，优先匹配cmod文件，其次chtl，不匹配cjmod文件)
具体名称（带后缀）：按官方模块目录→当前目录module文件夹→当前目录顺序搜索指定文件
具体路径（含文件信息）：直接按路径查找，未找到则报错
具体路径（不含文件信息）：触发报错
对于使用官方模块前缀，直接在官方模块目录中搜索  

## use
use语法能够明确当前文件使用什么配置组  
use语法必须在文件开头，且只能有一个用于配置组  

### HTML5类型
```chtl
use html5;
```

生成HTML5声明  

### 使用命名配置组
```chtl
use @Config Basic;  // 此文件使用Basic配置组

use [Configuration] @Config Basic;  // 也可以使用全缀名
```

## 命名空间
你可以使用[Namespace]创建命名空间，命名空间能够有效防止模块污染  
导入一整个文件，或导入了重名的任意单元时，命名空间起效  
命名空间允许不使用{}  

test.chtl  
```chtl
[Namespace] space  // 如果文件没有命名空间，则默认使用文件名称来作为命名空间

[Custom] @Element Box
{
    div
    {
        style
        {

        }
    }

    div
    {
        style
        {

        }
    }
}
```

```chtl
[Import] @Chtl from test.chtl的路径  

body
{
    @Element Box from space;  // 使用space命名空间中的Box自定义元素
}
```

### 命名空间嵌套
```chtl
[Namespace] space
{
    [Namespace] room  // 嵌套命名空间
    // 嵌套命名空间时，如果仅仅是只有一层关系 或 只有一层平级，可以不用写花括号

    [Custom] @Element Box
    {
        div
        {
            style
            {

            }
        }

        div
        {
            style
            {

            }
        }
    }
}
```

```chtl
[Namespace] space
{
    [Namespace] room
    {

    }

    [Namespace] room2
    {
        [Custom] @Element Box
        {
            div
            {
                style
                {

                }
            }

            div
            {
                style
                {

                }
            }
        }
    }
}
```

```chtl
[Import] @Chtl from test.chtl的路径

body
{
    @Element Box from space.room2;  // 嵌套命名空间的使用
}
```

命名空间功能说明  
同名命名空间自动合并  
命名空间冲突检测策略  
对于没有使用命名空间的文件，在被导入时，会默认以文件名作为命名空间  
你可以在[Configuration]之中添加下述规则禁用默认命名空间功能，这意味着导入时可能会造成污染  
```chtl
[Configuration]
{
    DISABLE_DEFAULT_NAMESPACE = false;
}
```

## 约束
你可以使用except关键字来进行定义域约束  

### 精确约束
精确约束能够作用的对象有HTML元素，自定义与模板对象  

```chtl
div
{
    except span, [Custom] @Element Box;  // 禁止使用span与Box
}
```

### 类型约束
精确约束能够作用的对象有@Html，[Custom]，以及模板[Template]  

```chtl
div
{
    except @Html;  // 禁止html元素
    except [Template] @Var; // 禁止在div内部使用变量组模板的对象
    except [Custom];  // 禁止出现[Custom]类型的对象
}
```

### 全局约束
在命名空间内使用全局约束，全局约束只支持前面列出来的类型  

```chtl
[Namespace] space
{
    except [Template];

    [Template] // 错误，不允许定义模板
}
```

## 选择器自动化与引用规则
现在，你可以在[Configuration]之中添加下述规则禁用自动化    

```chtl
[Configuration]
{
    // 禁止局部样式块自动添加类选择器
    DISABLE_STYLE_AUTO_ADD_CLASS = false;
    // 禁止局部样式块自动添加id选择器
    DISABLE_STYLE_AUTO_ADD_ID = false;
    // 禁止局部脚本自动添加类选择器
    DISABLE_SCRIPT_AUTO_ADD_CLASS = true;
    // 禁止局部脚本自动添加id选择器
    DISABLE_SCRIPT_AUTO_ADD_ID = true;
}
```

当局部style内部存在多组类选择器时，若class属性缺失，则自动添加第一个类选择器  
当局部style内部存在多组id选择器时，若id属性缺失，则自动添加第一个id选择器  
对于局部style来说，& 引用选择器优先选择class  

// 如果DISABLE_SCRIPT_AUTO_ADD_CLASS 和 DISABLE_SCRIPT_AUTO_ADD_ID 为真  
当局部script内部存在多组类选择器时，若class属性，局部style没有触发class自动化添加缺失，第一个{{.box}}会被自动添加  
当局部script内部存在多组id选择器时，若id属性，局部style没有触发id自动化添加缺失，第一个{{#box}}会被自动添加  
{{box}}不会自动添加，只有{{.box}}和{{#box}}能够触发自动化添加  
对于局部script来说，& 引用选择器优先选择id  

## CHTL JS
CHTL JS是CHTL的扩展语法，并不是JS的超集，也不支持JS的语法  
CHTL JS完全独立于JS，只是最终转变为JS代码  
JS的语法由CHTL内置的JS编译器解析，CHTL JS的语法由CHTL JS编译器解析  
两者之间并不兼容，CHTL JS的语法是CHTL JS编译器的扩展语法  

### 局部script
CHTL允许在局部样式块中使用script{}来编写JS代码  
局部script会被添加到一个不会全局污染，具有高优先级的全局script块之中  

```chtl
div
{
    style
    {
        .box
        {
            width: 100px;
            height: 100px;
            background-color: red;
        }
    }

    script
    {
        {{box}}.addEventListener('click', () => {
            console.log('Box clicked!');
        });
    }
}
```

### 增强选择器
你可以使用{{CSS选择器}}来创建一个DOM对象  

```chtl
button
{
    style
    {
        .box
        {
            
        }
    }
}

script
{
    {{box}}.textContent()  // 先判断是否为tag，然后查找类名 / id = box(id优先)的元素，并创建DOM对象
    {{.box}}  // 查找类名为box的元素，并创建DOM对象
    {{#box}}  // 查找id为box的元素，并创建DOM对象
    {{button}}  // 所有的button元素
    {{.box button}}  // 查找类名为box的元素的所有的button后代，并创建DOM对象  
    
    // 精确访问
    {{button[0]}}  // 第一个button元素

    // 增强选择器仅支持上述的种类，这是出于性能与复杂性之间的考虑
    // .boxbutton这种交集选择器会消耗很多性能，因此这里不得不放弃支持
}
```

### 明确使用CHTL语法
使用到CHTL JS语法时，我们推荐使用->代替.  
以便明确使用了CHTL JS语法  
->与.是完全等价的，因此你可以直接使用->进行链式访问  

```chtl
button
{
    style
    {
        .box
        {
            
        }
    }
}

script
{
    {{box}}->textContent();
}
```

### 增强监听器
你现在可以使用listen来快捷绑定事件监听器  

```chtl
button
{
    style
    {
        .box
        {

        }
    }
}

script
{
    {{box}}->listen({
        click: () => {

        },

        mouseenter: mouseEnterEvent,  // 已经存在的函数

        mousemove: function() {

        }
    });
}
```

### 事件委托
为了解决SPA页面中元素动态更新导致事件监听丢失的问题，提供了基于事件委托的增强语法  
通过将事件绑定到不会销毁的父元素，监听冒泡阶段的事件，从而实现稳定的事件绑定  

```chtl
script
{
    {{父元素选择器}}->delegate({
        target: {{选择器}} | [{{选择器1}}, {{选择器2}},...], // 要代理的子元素对象 / 子元素对象数组
        click: 函数,  // 绑定的事件类型及对应回调函数
        mouseenter: 函数,
        mouseleave: 函数,
    });
}
```
需要创建一个全局注册表，管理所有事件委托的父元素，重复绑定父元素的子元素会作为分支合并在同一个事件委托之中  
避免创建多个相同的事件委托  

### 动画
CHTL JS简化了动画的使用，封装了requestAnimationFrame  

```chtl
script
{
    const anim = animate({
        target: {{选择器}} || [{{选择器1}}, {{选择器2}}] || DOM对象
        duration: 100,  // 动画持续时间，ms
        easing: ease-in-out,  // 缓慢函数

        begin: {  // 起始状态，写css代码

        }

        when: [
            {
                at: 0.4;  // 动画播放到什么时刻

                // css代码
                opacity: 0.5,
                transform: 'scale(1.1)'
            },
            {
                at: 0.8;  // 动画播放到什么时刻
                // css代码
            }
        ]

        end: {  // 终止状态，写css代码

        }

        loop: -1,  // 循环次数
        direction: ,  // 播放的方向
        delay:  ,  // 开始播放的延迟，ms
        callback: function,  // 播放完毕后做什么
    });
}
```

### 虚对象
虚对象是CHTL JS重要的特征之一，你可以使用虚对象访问CHTL JS函数中键值为函数的键  

```
vir test = listen({
    click: () => {

        }
});

test->click();  // 调用click键的函数
```

虚对象的原理是创建特殊命名的全局函数，然后解析成这个函数的引用  
```chtl
vir Test = listen({
    click: ()=>{

    }
});
```

vir是CHTL JS层面的语法糖，不涉及JS  
listen会按原样生成JS代码  
vir本身就不存在，是编译期间的语法糖  

```
vir Test = listen({
    click: ()=>{

    }
});
```
编译器扫描到vir时，会创建一个C++对象，这个C++对象负责vir的解析  
假设这个对象为View  
View对象需要做两件事情，一件是记录vir虚对象的名称，第二个是解析listen CHTL JS函数中的函数键，并创建对应表  
后续在解析时，遇到Test->click;时，会创建对应的全局函数并加入注册表之中防止重复创建，然后将Test->click替换成函数引用(函数的名称)  

## 开发者专题
对于想要开发CHTL模块的开发者，可以查看此篇  

### CMOD
CMOD是CHTL提供的一种模块化方式  
CHTL编译器 / 打包脚本能够将符合CMOD格式的文件夹打包成.cmod文件  
你需要获取CHTL编译器的源码，CHTL源码中包含了打包脚本，如果你会编译CHTL编译器，那么更推荐你使用CHTL编译器来打包CMOD  

```打包脚本的内容待补充
```

#### CMOD标准格式
Chtholly  
    src  
        Chtholly.chtl  
        Other.chtl  
    info  
        Chtholly.chtl  

src是模块的源码，而info则是存放模块信息的文件夹  
CHTL要求，模块文件夹，主模块chtl文件，模块信息chtl文件必须同名  

在没有子模块的情况下，主模块chtl文件必须存在  

如果src中存在子模块，则src文件夹内部的主模块chtl文件可以省略(推荐)，也可以保留，保留的意义并不大  
最好是让主模块作为一个框架  

##### 信息
info文件夹中的chtl文件主要提供两个功能  
提供模块的信息与提供外部查询表  

如下所示  
```chtl
// 只需要写这个即可
[Info]
{
    name = "chtholly";
    version = "1.0.0";
    description = "珂朵莉主题模块 - 世界上最幸福的女孩";
    author = "CHTL Team";
    license = "MIT";
    dependencies = "";
    category = "theme";
    minCHTLVersion = "1.0.0";
    maxCHTLVersion = "2.0.0";
}

// 外部查询表，优化性能用，无实际作用
// 不需要写这个，此由系统生成
[Export]
{
    [Custom] @Style ChthollyStyle, ChthollyCard, ChthollyButton,
           ChthollyHeader, ChthollyFooter;

    [Custom] @Element ChthollyPage, ChthollySection, ChthollyNav,
             ChthollyModal, ChthollyGallery;

    [Custom] @Var ChthollyColors, ChthollyFonts, ChthollySpacing,
         ChthollyBreakpoints, ChthollyAnimations;

    [Template] @Style ...;
    [Template] @Element ...;
    [Template] @Var ...;
    [Origin] @Html ...;
    [Origin] @Style ...;
    [Origin] @Javascript ...;
    [Origin] @Vue ...;  // 一样支持导出自定义类型的原始嵌入
    [Configuration] @Config ...;
}
```

#### 子模块
具有子模块的CMOD文件结构并没有太大的改变  
相当于嵌套了一层结构  

Chtholly  
    src  
        (Chtholly.chtl)  
        Other  
            src  
                Other.chtl
            info  
                Other.chtl
        Space  
            src  
                Space.chtl
            info  
                Space.chtl
    info  
        Chtholly.chtl  

#### CMOD的使用
如何导入模块？使用`Import @Chtl from 模块名`即可  
会导入所有的子模块，也可以精确到具体的子模块，例如Import @Chtl from Chtholly.Space  
如何使用其他人开发的Cmod呢？你需要在需要执行的文件处，新建module文件夹，并将Cmod放置文件夹之中  

### CJMOD
CJMOD是CHTL提供扩展CHTL JS语法的一种方式，允许通过CHTL JS编译器的一些接口使用C++自由实现CHTL JS语法  
让创建无限的JS语法成为一种可能，CJMOD的文件结构与CMOD类似  
CJMOD让扩展CHTL JS语法变得更简单，无需阅读源码即可实现CHTL JS语法的扩展，并通过CJMOD的方式发布  

CJmod结构  

CJmod文件夹  
    src/xxx.cpp xxx.h  
        Box  
            src/xxx.cpp xxx.h  
            info/Box.chtl  
        Box2  
            src/xxx.cpp xxx.h  
            info/Box2.chtl  
    info/CJmod文件夹名称.chtl  

你无需害怕CJMOD存在局限，你可以自由包含头文件，编写全局变量管理状态  

### CMOD + CJMOD
如果你想要提供组件的同时提供扩展CHTL JS语法  
我们建议使用如下的结构  

模块名称  
    CMOD / Cmod / cmod  
        Box  
            src/Box.chtl, Other.chtl  
            info/Box.chtl  
    CJMOD / CJmod / cjmod  
        Box  
            src/xxx.cpp xxx.h  
            info/Box.chtl  

对于子模块，你只需这样创建  
模块名称  
    CMOD  
        Box  
            Box1  
                src/  
                info/  
            Box2  
                src/  
                info/  

如何使用？例如这个模块叫Box，那么如果我想要调用Box的CMOD模块时，我们直接使用[Import] @Chtl即可  
如果需要使用CJMOD，需要使用[Import] @CJmod，CHTL不会对此进行统一处理，我们不推荐使用@Chtl同时管理CMOD和CJMOD  

### Chtholly 珂朵莉模块
珂朵莉对于我来说，是一个很特别的角色，是我一直喜欢着的人物，我希望我能让珂朵莉成为CHTL中重要的模块  
珂朵莉模块采用CMOD + CJMOD的混合模块  

#### CMOD
##### 手风琴
##### 四叶窗相册
##### 备忘录
##### 暖色笔记
##### 樱花雨
##### 鼠标特效
##### 鼠标拖尾
##### 视差滚动背景
##### 右键菜单栏
##### 进度条

#### CJMOD
##### printMylove
printMylove可以将一张图片变成字符像素块的形式，你可以使用printMylove来把图片转换成字符像素块或ASCII的形式  
然后输出到控制台  
```chtl
const str = printMylove({
    url: ,
    mode: ,  // 模式可以选择ASCII或Pixel
    width: ,  // 宽度，支持的单位有CSS单位以及百分比，小数，纯数字(像素)
    height: ,  // 高度
    scale:  ,  // 缩放倍数，限定为等比缩放策略
});
```

##### iNeverAway
iNeverAway是一个很特别的功能，从名称上面你完全是理解不到这个功能的实际作用的 iNeverAway用于创建一组标记函数  
iNeverAway与其他CHTL JS功能不同，它允许开发者定义键，而不是使用键，并可以使用状态区分同名的键  
iNeverAway需要与虚对象共用  

```chtl
vir Test = iNeverAway({
    Void<A>: function(int, int) {

    },

    Void<B>: funtion(int, int) {  // 通过状态同名同参重载

    },

    Void: {

    },

    Ax: {

    }
});

Test->Void<A>();
```
Test是虚拟对象，是不存在的对象，这里并没有创建一个对象  

实现原理：  
iNeverAway  ->  创建一组JS全局函数，名称由CHTL编译器统一管理，在调用时才生成对应的JS函数代码  
vir对象本身不存在，最终转变成相对应的函数的引用  

iNeverAway函数存在的意义其实很迷惑人，这是因为相对于使用iNeverAway，更多人更喜欢使用普通的函数  
这是当然，毕竟iNeverAway存在的意义本身就不是作为实用功能而存在，然而，iNeverAway其实是CHTL JS的一种新方向  
也是对函数重载的重定义  

### Yuigahama 由比滨结衣模块
由比滨结衣模块使用CMOD

#### 音乐播放器
#### 手风琴
#### 四叶窗相册
#### 备忘录
#### 暖色笔记
#### 樱花雨
#### 鼠标特效


对于@Chtl类型来说  
名称（不带后缀）：优先搜索官方模块目录(源码编译后生成的module文件夹，通常和编译器同一个文件夹，含cmod，chtl和cjmod文件），其次搜索编译文件所在的目录module文件夹，最后搜索编译文件所在目录，优先匹配cmod文件，其次chtl，不匹配cjmod文件)
具体名称（带后缀）：按官方模块目录→当前目录module文件夹→当前目录顺序搜索指定文件
具体路径（含文件信息）：直接按路径查找，未找到则报错
具体路径（不含文件信息）：触发报错
对于使用官方模块前缀，直接在官方模块目录中搜索  

对于@CJmod类型来说  
名称（不带后缀）：优先搜索官方模块目录，其次搜索当前目录module文件夹，最后搜索当前目录，仅匹配cjmod文件  
具体名称（带后缀）：按官方模块目录→当前目录module文件夹→当前目录顺序搜索指定文件  
具体路径（含文件信息）：直接按路径查找，未找到则报错  
具体路径（不含文件信息）：触发报错
对于使用官方模块前缀，直接在官方模块目录中搜索  

编译生成的module文件夹(官方模块)有两种结构，一种就是常规的混杂，chtl文件，cmod，cjmod  
一种是使用CMOD / cmod / Cmod(包括chtl文件) + CJMOD / cjmod / CJmod(不包括chtl文件)两个文件夹进行分类  

对于用户来说，他们创建的module文件夹也能够使用分类结构  
值得一提，源代码目录下的模块源码目录Module也可以使用分类结构  

[Import] @Chtl from 具体路径.*  // 导入具体路径下的所有.cmod和.chtl文件  
[Import] @Chtl from 具体路径.*.cmod  // 导入具体路径下的所有.cmod文件  
[Import] @Chtl from 具体路径.*.chtl  // 导入具体路径下的所有.chtl文件  
等价于
[Import] @Chtl from 具体路径/*  // 导入具体路径下的所有.cmod和.chtl文件  
[Import] @Chtl from 具体路径/*.cmod  // 导入具体路径下的所有.cmod文件  
[Import] @Chtl from 具体路径/*.chtl  // 导入具体路径下的所有.chtl文件  

// 导入子模块时，支持使用'/'替代'.'作为路径分隔符  
[Import] @Chtl from Chtholly.*  // 导入Chtholly模块的所有子模块  
[Import] @Chtl from Chtholly.Space  // 导入Chtholly模块中指定的Space子模块  

[Import] @CJmod from 模块名称  // 导入指定名称的CJmod模块  

CJmod与Cmod采用相同的路径搜索策略。  
