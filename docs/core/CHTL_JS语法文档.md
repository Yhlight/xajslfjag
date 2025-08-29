# CHTL JS语法文档

## 概述

CHTL JS是CHTL项目的JavaScript扩展语法，提供了一系列语法糖和扩展功能，使JavaScript编程更加简洁和强大。CHTL JS完全兼容标准JavaScript，同时添加了专门针对Web开发优化的特性。

## 基础语法

### 🎭 **Listen语法**

Listen是CHTL JS的核心特性之一，提供了优雅的事件处理机制。

#### 基本语法
```javascript
listen variableName = {
    eventType: handler,
    eventType2: handler2,
    // ...
};
```

#### 使用示例
```javascript
// 基础listen语法
listen buttonHandler = {
    click: () => {
        console.log('Button clicked!');
    },
    
    hover: (event) => {
        event.target.style.backgroundColor = '#f0f0f0';
    },
    
    focus: function(event) {
        this.classList.add('focused');
    }
};

// 支持多种函数写法
listen formHandler = {
    submit: (e) => e.preventDefault(),
    
    reset: function() {
        console.log('Form reset');
    },
    
    // 箭头函数简写
    change: e => console.log('Input changed:', e.target.value)
};
```

#### 编译后的JavaScript
```javascript
const buttonHandler = {
    click: () => {
        console.log('Button clicked!');
    },
    
    hover: (event) => {
        event.target.style.backgroundColor = '#f0f0f0';
    },
    
    focus: function(event) {
        this.classList.add('focused');
    }
};
```

### 🎪 **Delegate语法**

Delegate提供了事件委托的简洁语法。

#### 基本语法
```javascript
delegate containerSelector on eventType for targetSelector {
    // 事件处理代码
}
```

#### 使用示例
```javascript
// 事件委托
delegate .container on click for .button {
    console.log('Delegated button click');
    this.classList.toggle('active');
}

// 多个事件类型
delegate #app on ['click', 'touch'] for .interactive {
    console.log('User interaction detected');
}
```

#### 编译后的JavaScript
```javascript
document.addEventListener('DOMContentLoaded', () => {
    const container = document.querySelector('.container');
    container.addEventListener('click', function(event) {
        if (event.target.matches('.button')) {
            console.log('Delegated button click');
            event.target.classList.toggle('active');
        }
    });
});
```

### 🎨 **Animate语法**

Animate提供了简化的动画语法。

#### 基本语法
```javascript
animate selector {
    property: [from, to],
    duration: time,
    easing: 'easing-function',
    // ...
}
```

#### 使用示例
```javascript
// 基础动画
animate .box {
    opacity: [0, 1],
    transform: ['translateY(-20px)', 'translateY(0)'],
    duration: 300,
    easing: 'ease-in-out'
}

// 链式动画
animate .card {
    transform: ['scale(1)', 'scale(1.05)'],
    duration: 200
} then {
    transform: ['scale(1.05)', 'scale(1)'],
    duration: 200
}

// 条件动画
animate .modal when .show {
    opacity: [0, 1],
    transform: ['scale(0.8)', 'scale(1)'],
    duration: 250
}
```

#### 编译后的JavaScript
```javascript
// 基础动画
document.querySelectorAll('.box').forEach(element => {
    element.animate([
        { opacity: 0, transform: 'translateY(-20px)' },
        { opacity: 1, transform: 'translateY(0)' }
    ], {
        duration: 300,
        easing: 'ease-in-out',
        fill: 'forwards'
    });
});
```

## 虚对象系统

### 🌟 **vir关键字**

虚对象是CHTL JS的重要特征，提供编译时的元信息访问能力。

#### 基本语法
```javascript
vir objectName = listen {
    eventType: handler,
    property: value
};

// 访问虚对象
objectName->eventType();    // 函数引用
objectName->property;       // 属性访问
```

#### 使用示例
```javascript
// 虚对象定义
vir menuController = listen {
    open: () => {
        document.getElementById('menu').classList.add('open');
    },
    
    close: () => {
        document.getElementById('menu').classList.remove('open');
    },
    
    toggle: function() {
        this.isOpen ? this.close() : this.open();
    },
    
    isOpen: false,
    
    items: ['Home', 'About', 'Contact']
};

// 虚对象访问
menuController->open();         // 调用函数
menuController->toggle();       // 调用方法
console.log(menuController->isOpen);     // 访问属性
console.log(menuController->items);      // 访问数组
```

#### 编译特性
```javascript
// 虚对象在编译时被处理，运行时不存在实际对象
// menuController->open() 被编译为直接的函数调用

(() => {
    document.getElementById('menu').classList.add('open');
})();

// 属性访问被编译为直接的值引用
console.log(false);  // menuController->isOpen
console.log(['Home', 'About', 'Contact']);  // menuController->items
```

## 事件绑定操作符

### 🎯 **&-> 操作符**

事件绑定操作符提供了简洁的DOM事件绑定语法。

#### 基本语法
```javascript
{{selector}} &-> eventType {
    // 事件处理代码
}
```

#### 使用示例
```javascript
// 基础事件绑定
{{.button}} &-> click {
    console.log('Button clicked!');
    this.style.backgroundColor = 'blue';
}

// 多个选择器
{{.btn, .link}} &-> click {
    console.log('Interactive element clicked');
}

// 事件对象使用
{{input[type="text"]}} &-> input {
    console.log('Input value:', event.target.value);
    
    if (event.target.value.length > 10) {
        event.target.classList.add('warning');
    }
}

// 复杂事件处理
{{.form}} &-> submit {
    event.preventDefault();
    
    const formData = new FormData(event.target);
    console.log('Form data:', Object.fromEntries(formData));
    
    // 异步提交
    fetch('/api/submit', {
        method: 'POST',
        body: formData
    });
}
```

#### 编译后的JavaScript
```javascript
document.addEventListener('DOMContentLoaded', () => {
    const elements = document.querySelectorAll('.button');
    elements.forEach(element => {
        element.addEventListener('click', function(event) {
            console.log('Button clicked!');
            this.style.backgroundColor = 'blue';
        });
    });
});
```

## 异步语法糖

### ⚡ **异步链式调用 (await>>)**

#### 基本语法
```javascript
await>> expression1 >> expression2 >> expression3
```

#### 使用示例
```javascript
// 链式异步调用
await>> fetchUserData()
    >> processUserData
    >> updateUI
    >> logSuccess;

// 带参数的链式调用
await>> fetchUser(userId)
    >> (user) => enrichUserData(user)
    >> (enrichedUser) => {
        console.log('User loaded:', enrichedUser.name);
        return enrichedUser;
    }
    >> saveToCache;

// 错误处理
await>> fetchData()
    >> processData
    >> saveData
    catch (error) => {
        console.error('Chain failed:', error);
        showErrorMessage(error.message);
    };
```

#### 编译后的JavaScript
```javascript
(async () => {
    try {
        let result = await fetchUserData();
        result = await processUserData(result);
        result = await updateUI(result);
        result = await logSuccess(result);
        return result;
    } catch (error) {
        console.error('Async chain error:', error);
        throw error;
    }
})();
```

### 🔀 **并行异步调用 (parallel>>)**

#### 基本语法
```javascript
parallel>> [expression1, expression2, expression3]
```

#### 使用示例
```javascript
// 并行执行
parallel>> [
    fetchUserProfile(),
    fetchUserPosts(),
    fetchUserFriends()
];

// 并行执行带结果处理
parallel>> [
    loadCSS('/styles/main.css'),
    loadJS('/scripts/app.js'),
    fetchConfig('/api/config')
] then ([css, js, config]) => {
    console.log('All resources loaded');
    initializeApp(config);
};
```

#### 编译后的JavaScript
```javascript
Promise.all([
    fetchUserProfile(),
    fetchUserPosts(),
    fetchUserFriends()
]);
```

### ⏱️ **超时控制 (timeout>>)**

#### 基本语法
```javascript
timeout>> expression within timeLimit
```

#### 使用示例
```javascript
// 超时控制
timeout>> fetchData() within 5000;

// 超时带默认值
timeout>> slowAPI() within 3000 or defaultData;

// 超时错误处理
timeout>> criticalOperation() within 2000
    catch timeout => {
        console.warn('Operation timed out');
        return fallbackResult;
    };
```

#### 编译后的JavaScript
```javascript
Promise.race([
    fetchData(),
    new Promise((_, reject) => 
        setTimeout(() => reject(new Error('Timeout')), 5000)
    )
]);
```

### 🔄 **重试机制 (retry>>)**

#### 基本语法
```javascript
retry>> expression times maxRetries
```

#### 使用示例
```javascript
// 基础重试
retry>> unreliableAPI() times 3;

// 重试带延迟
retry>> fetchImportantData() times 5 delay 1000;

// 条件重试
retry>> networkRequest() times 3 
    when (error) => error.code === 'NETWORK_ERROR';
```

#### 编译后的JavaScript
```javascript
async function retryOperation(operation, maxRetries = 3, delay = 0) {
    for (let i = 0; i < maxRetries; i++) {
        try {
            return await operation();
        } catch (error) {
            if (i === maxRetries - 1) throw error;
            if (delay > 0) await new Promise(resolve => setTimeout(resolve, delay));
        }
    }
}

retryOperation(() => unreliableAPI(), 3);
```

## 响应式绑定语法

### 🔗 **双向绑定 (<->)**

#### 基本语法
```javascript
object.property <-> element.attribute
```

#### 使用示例
```javascript
// 双向数据绑定
user.name <-> {{#nameInput}}.value;

// 复杂绑定
formData.email <-> {{input[name="email"]}}.value;

// 带转换的绑定
user.age <-> {{#ageSlider}}.value as Number;

// 多个元素绑定
settings.theme <-> [
    {{#themeSelect}}.value,
    {{body}}.className as (theme) => `theme-${theme}`
];
```

#### 编译后的JavaScript
```javascript
{
    let _value = user.name;
    const element = document.querySelector('#nameInput');
    
    Object.defineProperty(user, 'name', {
        get() { return _value; },
        set(newValue) {
            _value = newValue;
            element.value = newValue;
        }
    });
    
    element.addEventListener('input', (e) => {
        user.name = e.target.value;
    });
}
```

### ➡️ **计算属性 (=>)**

#### 基本语法
```javascript
object.computedProperty => expression
```

#### 使用示例
```javascript
// 计算属性
user.fullName => `${user.firstName} ${user.lastName}`;

// 复杂计算
cart.total => cart.items.reduce((sum, item) => sum + item.price * item.quantity, 0);

// 依赖多个属性
settings.isValid => settings.email && settings.password && settings.terms;

// 异步计算属性
user.avatar => fetch(`/api/avatar/${user.id}`).then(r => r.blob());
```

#### 编译后的JavaScript
```javascript
Object.defineProperty(user, 'fullName', {
    get() {
        return `${user.firstName} ${user.lastName}`;
    },
    enumerable: true,
    configurable: true
});
```

### ❓ **条件渲染 (?:)**

#### 基本语法
```javascript
condition ?: trueExpression : falseExpression
```

#### 使用示例
```javascript
// 条件显示
user.isLoggedIn ?: {{#userMenu}}.show() : {{#loginButton}}.show();

// 条件类名
user.isPremium ?: {{body}}.addClass('premium') : {{body}}.removeClass('premium');

// 条件内容
cart.itemCount > 0 ?: 
    {{#cartCount}}.textContent = cart.itemCount :
    {{#cartCount}}.textContent = 'Empty';
```

### 🔄 **列表渲染 (*:)**

#### 基本语法
```javascript
array *: template
```

#### 使用示例
```javascript
// 列表渲染
users *: (user, index) => `
    <div class="user-item" data-id="${user.id}">
        <h3>${user.name}</h3>
        <p>${user.email}</p>
    </div>
` into {{#userList}};

// 带过滤的列表渲染
products.filter(p => p.inStock) *: product => `
    <div class="product">
        <h4>${product.name}</h4>
        <span>${product.price}</span>
    </div>
` into {{#productGrid}};
```

### 👁️ **观察器 (~>)**

#### 基本语法
```javascript
expression ~> handler
```

#### 使用示例
```javascript
// 属性观察
user.status ~> (newStatus, oldStatus) => {
    console.log(`Status changed: ${oldStatus} -> ${newStatus}`);
    updateStatusIcon(newStatus);
};

// 深度观察
settings ~> {
    deep: true,
    handler: (newSettings) => {
        localStorage.setItem('settings', JSON.stringify(newSettings));
    }
};

// 立即执行的观察器
theme.mode ~> {
    immediate: true,
    handler: (mode) => {
        document.body.className = `theme-${mode}`;
    }
};
```

### 🔀 **事件流 (|>)**

#### 基本语法
```javascript
eventSource |> operator1 |> operator2 |> handler
```

#### 使用示例
```javascript
// 事件流处理
{{#searchInput}}.input |> 
    debounce(300) |> 
    map(e => e.target.value) |> 
    filter(query => query.length > 2) |> 
    (query) => {
        searchAPI(query).then(results => {
            displayResults(results);
        });
    };

// 复杂事件流
{{window}}.scroll |>
    throttle(16) |>
    map(() => window.scrollY) |>
    distinctUntilChanged() |>
    (scrollY) => {
        updateScrollProgress(scrollY);
        toggleHeaderVisibility(scrollY);
    };
```

## CJMOD扩展系统

### 🔌 **自定义语法扩展**

CHTL JS支持通过CJMOD (C++ JavaScript Module) 扩展语法。

#### 扩展示例
```javascript
// 假设通过CJMOD扩展了 'animate' 语法
animate .element {
    duration: 300,
    easing: 'ease-out',
    properties: {
        opacity: [0, 1],
        transform: ['translateY(20px)', 'translateY(0)']
    }
}

// 假设扩展了 'reactive' 语法
reactive state = {
    count: 0,
    message: '',
    computed: {
        doubleCount: () => state.count * 2
    },
    watch: {
        count: (newVal) => console.log('Count changed:', newVal)
    }
};
```

### 🛠️ **CJMOD API使用**

```cpp
// C++ CJMOD扩展示例
class AnimateExtension : public CJMODExtension {
public:
    std::vector<std::string> getKeywords() const override {
        return {"animate", "transition", "effect"};
    }
    
    std::unique_ptr<BaseJSNode> parse(const std::string& keyword, 
                                      const std::vector<Arg>& args,
                                      CHTLJSParser& parser) override {
        // 解析自定义语法
        return std::make_unique<AnimateNode>(args);
    }
};
```

## 高级特性

### 🎪 **模块系统集成**

```javascript
// CHTL JS 模块导入
import { MyComponent } from '@chtl/ui-components';
import * as Utils from './utils.chtljs';

// 使用导入的组件
listen pageController = {
    init: () => {
        MyComponent.initialize();
        Utils.setupEventHandlers();
    }
};
```

### 🔄 **生命周期钩子**

```javascript
// 组件生命周期
listen componentLifecycle = {
    mounted: () => {
        console.log('Component mounted');
        this.initializeData();
    },
    
    updated: (changes) => {
        console.log('Component updated:', changes);
    },
    
    unmounted: () => {
        console.log('Component cleanup');
        this.cleanup();
    }
};
```

### 🎨 **样式绑定**

```javascript
// 动态样式绑定
element.style <-> {
    backgroundColor: theme.primaryColor,
    fontSize: `${settings.fontSize}px`,
    opacity: user.isActive ? 1 : 0.5
};

// CSS类绑定
element.classList <-> {
    active: state.isActive,
    disabled: !state.enabled,
    [`theme-${theme.name}`]: true
};
```

## 错误处理和调试

### 🐛 **错误捕获**

```javascript
// 全局错误处理
listen errorHandler = {
    error: (error, context) => {
        console.error('CHTL JS Error:', error);
        reportError(error, context);
    },
    
    unhandledRejection: (reason, promise) => {
        console.error('Unhandled Promise Rejection:', reason);
    }
};

// 局部错误处理
try {
    await>> riskyOperation() >> processResult;
} catch (error) {
    console.error('Operation failed:', error);
    showUserFriendlyError();
}
```

### 🔍 **调试工具**

```javascript
// 调试模式
if (CHTL.DEBUG) {
    listen debugController = {
        logState: () => console.log('Current state:', state),
        inspectElement: (selector) => console.dir(document.querySelector(selector)),
        showEventLog: () => CHTL.EventLogger.display()
    };
}

// 性能监控
CHTL.performance.mark('operation-start');
await>> heavyOperation();
CHTL.performance.mark('operation-end');
CHTL.performance.measure('operation-duration', 'operation-start', 'operation-end');
```

## 最佳实践

### ✅ **代码组织**

```javascript
// 推荐的代码结构
// 1. 导入和配置
import { CONFIG } from './config.js';

// 2. 状态定义
const appState = {
    user: null,
    settings: {},
    ui: { loading: false }
};

// 3. 事件处理器
listen mainController = {
    init: () => {
        // 初始化逻辑
    },
    
    handleUserAction: (action) => {
        // 用户操作处理
    }
};

// 4. 事件绑定
{{.main-button}} &-> click {
    mainController->handleUserAction('button-click');
};

// 5. 响应式绑定
appState.user <-> {{#user-display}}.textContent;
```

### 🚫 **避免的反模式**

```javascript
// ❌ 避免：过度使用虚对象
vir overusedVirtual = listen {
    // 太多属性和方法
};

// ✅ 推荐：合理分组
vir uiController = listen {
    showModal: () => {},
    hideModal: () => {}
};

vir dataController = listen {
    loadData: () => {},
    saveData: () => {}
};

// ❌ 避免：复杂的事件绑定
{{complex-selector[data-attribute*="value"]}} &-> click {
    // 复杂逻辑
};

// ✅ 推荐：简化选择器，提取逻辑
{{.clickable}} &-> click {
    handleClick(this);
};
```

## 兼容性说明

### 🌐 **浏览器支持**

| 浏览器 | 最低版本 | 支持特性 |
|--------|----------|----------|
| Chrome | 60+ | 全部特性 |
| Firefox | 55+ | 全部特性 |
| Safari | 12+ | 全部特性 |
| Edge | 79+ | 全部特性 |
| IE | 11 | 基础特性 (需要polyfill) |

### 📦 **Polyfill支持**

```javascript
// 自动polyfill注入
if (!window.Promise) {
    // Promise polyfill
}

if (!Array.prototype.includes) {
    // Array.includes polyfill
}

// CHTL JS runtime
CHTL.polyfill.ensure(['Promise', 'fetch', 'CustomEvent']);
```

---

CHTL JS语法文档为开发者提供了全面的语法参考，通过这些特性，您可以编写更加简洁、强大和易维护的JavaScript代码。