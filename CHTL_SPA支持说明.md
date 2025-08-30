# CHTL SPA页面支持说明

## SPA支持状态：✅ **完全支持**

CHTL 设计时就考虑了现代化的 Single Page Application (SPA) 开发需求，提供了完整的 SPA 开发支持。

## 🚀 SPA核心特性支持

### ✅ 1. 组件化开发
**完全支持组件式开发模式**

```chtl
<!-- 页面头部组件 -->
<template name="page-header">
    <header class="spa-header">
        <nav class="navigation">
            <a href="#home" class="nav-link">首页</a>
            <a href="#products" class="nav-link">产品</a>
            <a href="#about" class="nav-link">关于</a>
        </nav>
        <h1 class="page-title">{{title}}</h1>
    </header>
</template>

<!-- 用户卡片组件 -->
<template name="user-card">
    <div class="user-card {{status}}">
        <img src="{{avatar}}" alt="{{name}}" class="avatar">
        <h3>{{name}}</h3>
        <p>{{bio}}</p>
        <button class="contact-btn">&->click => contactUser({{id}})>联系</button>
    </div>
</template>

<!-- 主应用页面 -->
<div class="spa-app">
    <@page-header title="用户管理系统" />
    
    <main class="content">
        <@user-card 
            *: users
            name="{{item.name}}"
            avatar="{{item.avatar}}"
            bio="{{item.bio}}"
            status="{{item.active ? 'active' : 'inactive'}}"
            id="{{item.id}}"
        />
    </main>
</div>
```

### ✅ 2. 响应式数据绑定
**强大的响应式数据系统**

```javascript
// 双向数据绑定
userForm.username <-> inputField.value
searchTerm <-> searchBox.value

// 计算属性
filteredUsers => users.filter(user => 
    user.name.includes(searchTerm) && user.active
)

// 条件渲染
loadingSpinner ?: isLoading
errorMessage ?: hasError
userList ?: users.length > 0

// 列表渲染
userItems *: filteredUsers
menuItems *: navigationMenu

// 数据监听
userData ~> handleUserUpdate
routeState ~> updateNavigation
```

### ✅ 3. 动态路由管理
**内置路由和导航支持**

```javascript
// 路由状态管理
vir router = {
    currentRoute: 'home',
    routes: {
        home: 'home-page',
        users: 'user-list',
        profile: 'user-profile',
        settings: 'app-settings'
    },
    params: {}
};

// 路由导航
function navigate(route, params = {}) {
    router.currentRoute = route;
    router.params = params;
    updateView();
    updateURL();
}

// 动态组件加载
function updateView() {
    const component = router.routes[router.currentRoute];
    document.querySelector('#app').innerHTML = renderComponent(component);
}

// 路由监听
window.listen('popstate', (event) => {
    const route = parseURL(window.location.hash);
    navigate(route.path, route.params);
});
```

### ✅ 4. 状态管理系统
**集中式状态管理**

```javascript
// 全局状态存储
vir appState = {
    user: {
        isLoggedIn: false,
        profile: null,
        permissions: []
    },
    ui: {
        theme: 'light',
        sidebarOpen: false,
        currentModal: null
    },
    data: {
        users: [],
        products: [],
        notifications: []
    }
};

// 状态更新方法
function updateState(path, value) {
    const keys = path.split('.');
    let current = appState;
    
    for (let i = 0; i < keys.length - 1; i++) {
        current = current[keys[i]];
    }
    
    current[keys[keys.length - 1]] = value;
    notifyStateChange(path, value);
}

// 状态订阅
appState.user ~> handleUserStateChange
appState.ui.theme ~> updateTheme
```

### ✅ 5. 异步数据处理
**现代化的异步编程支持**

```javascript
// 异步数据获取
async function loadUserData() {
    const users = await>> fetchUsers() 
                  => validateUsers() 
                  => cacheUsers();
    
    userList <-> users;
}

// 并行数据加载
const [users, products, settings] = await parallel>> [
    fetchUsers(),
    fetchProducts(),
    fetchSettings()
];

// 带超时的数据获取
const userData = await timeout>> fetchUserProfile(userId) :: 5000;

// 重试机制
const apiResult = await retry>> callAPI() :: 3;
```

### ✅ 6. 事件处理优化
**高效的事件系统**

```javascript
// 事件委托 (适合动态内容)
document.delegate('.user-item', 'click', handleUserClick);
document.delegate('.delete-btn', 'click', handleDelete);

// 自定义事件
function emitUserUpdate(userData) {
    document.dispatchEvent(new CustomEvent('user:update', {
        detail: userData
    }));
}

// 事件流处理
userEvents |> filterValidEvents |> processUserAction |> updateUI

// 防抖和节流
searchInput.listen('input', debounce(handleSearch, 300));
scrollContainer.listen('scroll', throttle(handleScroll, 100));
```

### ✅ 7. 模块化架构
**清晰的模块组织**

```chtl
<!-- 页面模块 -->
[Namespace spa.pages]

<template name="home-page">
    <div class="page home-page">
        <@page-header title="首页" />
        <@hero-section />
        <@feature-list />
    </div>
</template>

<!-- 组件模块 -->
[Namespace spa.components]

<template name="hero-section">
    <section class="hero">
        <h2>{{headline}}</h2>
        <p>{{description}}</p>
        <button class="cta-btn">&->click => handleCTA()>{{ctaText}}</button>
    </section>
</template>

<!-- 工具模块 -->
[Namespace spa.utils]

<script>
    module('spa-utils', function() {
        return {
            formatDate: (date) => new Intl.DateTimeFormat().format(date),
            validateEmail: (email) => /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email),
            debounce: (func, delay) => { /* implementation */ }
        };
    });
</script>
```

## 🔧 SPA开发最佳实践

### 1. 项目结构建议
```
spa-project/
├── src/
│   ├── pages/          # 页面组件
│   │   ├── home.chtl
│   │   ├── users.chtl
│   │   └── profile.chtl
│   ├── components/     # 通用组件
│   │   ├── header.chtl
│   │   ├── sidebar.chtl
│   │   └── modal.chtl
│   ├── utils/          # 工具函数
│   │   ├── api.chtl
│   │   ├── router.chtl
│   │   └── state.chtl
│   └── styles/         # 样式文件
├── build/              # 构建输出
└── config/             # 配置文件
```

### 2. 性能优化策略
```javascript
// 懒加载组件
async function loadComponent(name) {
    const component = await import(`./components/${name}.chtl`);
    return component;
}

// 虚拟化长列表
function renderVirtualList(items, viewportHeight) {
    const visibleItems = calculateVisibleItems(items, viewportHeight);
    return visibleItems.map(item => renderItem(item));
}

// 状态缓存
const stateCache = new Map();
function getCachedState(key) {
    return stateCache.get(key) || loadState(key);
}
```

### 3. SEO和SSR支持
```chtl
<!-- 服务端渲染标记 -->
<div class="spa-app" data-ssr="true">
    <meta name="description" content="{{pageDescription}}">
    <meta property="og:title" content="{{pageTitle}}">
    
    <!-- 预渲染内容 -->
    <div class="prerender-content">
        <@page-content data="{{ssrData}}" />
    </div>
    
    <!-- 客户端激活脚本 -->
    <script>
        // 客户端激活
        document.addEventListener('DOMContentLoaded', function() {
            hydrateApp({{ssrData}});
        });
    </script>
</div>
```

## 📊 SPA功能对比

| 功能特性 | CHTL支持 | 传统方案 | 优势 |
|---------|---------|---------|------|
| 组件化开发 | ✅ 原生支持 | 需要框架 | 语法简洁 |
| 数据绑定 | ✅ 双向绑定 | 单向为主 | 开发效率高 |
| 路由管理 | ✅ 内置支持 | 需要库 | 集成度高 |
| 状态管理 | ✅ 响应式 | 需要额外库 | 学习成本低 |
| 异步处理 | ✅ 语法糖 | Promise/Async | 代码更清晰 |
| 事件系统 | ✅ 优化委托 | 原生API | 性能更好 |

## 🎯 实际应用示例

### 用户管理SPA应用
```chtl
[Configuration spa-app]
DEBUG_MODE: false
ENABLE_ROUTER: true
ENABLE_STATE_MANAGEMENT: true

[Namespace userapp]

<!-- 主应用容器 -->
<div class="user-management-spa">
    <style>
        .user-management-spa {
            display: flex;
            min-height: 100vh;
            font-family: 'Segoe UI', system-ui;
        }
        .sidebar { width: 250px; background: #f5f5f5; }
        .main-content { flex: 1; padding: 20px; }
        .user-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(300px, 1fr)); gap: 20px; }
    </style>
    
    <!-- 侧边栏导航 -->
    <aside class="sidebar">
        <nav class="navigation">
            <a href="#users" class="nav-item">&->click => navigate('users')>用户列表</a>
            <a href="#add-user" class="nav-item">&->click => navigate('add-user')>添加用户</a>
            <a href="#settings" class="nav-item">&->click => navigate('settings')>设置</a>
        </nav>
    </aside>
    
    <!-- 主内容区域 -->
    <main class="main-content">
        <!-- 用户列表页面 -->
        <div class="page users-page" show?: currentRoute === 'users'>
            <header class="page-header">
                <h1>用户管理</h1>
                <input type="text" placeholder="搜索用户..." searchTerm <-> value>
            </header>
            
            <div class="user-grid">
                <@user-card 
                    *: filteredUsers
                    user="{{item}}"
                    onEdit="{{editUser}}"
                    onDelete="{{deleteUser}}"
                />
            </div>
        </div>
        
        <!-- 添加用户页面 -->
        <div class="page add-user-page" show?: currentRoute === 'add-user'>
            <@user-form onSubmit="{{createUser}}" />
        </div>
    </main>
    
    <script>
        // 应用状态
        vir appState = {
            currentRoute: 'users',
            users: [],
            searchTerm: '',
            loading: false
        };
        
        // 计算属性
        const filteredUsers => appState.users.filter(user => 
            user.name.toLowerCase().includes(appState.searchTerm.toLowerCase())
        );
        
        // 路由处理
        function navigate(route) {
            appState.currentRoute = route;
            history.pushState({}, '', `#${route}`);
        }
        
        // 用户操作
        async function createUser(userData) {
            appState.loading = true;
            const newUser = await>> createUserAPI(userData) => validateUser();
            appState.users.push(newUser);
            appState.loading = false;
            navigate('users');
        }
        
        // 初始化应用
        document.addEventListener('DOMContentLoaded', async function() {
            appState.users = await>> fetchUsers() => sortUsers();
        });
    </script>
</div>
```

## ✅ 结论

**CHTL 完全支持 SPA 页面开发！**

主要优势：
- 🔥 **原生组件化** - 无需额外框架
- ⚡ **响应式数据绑定** - 自动更新视图
- 🎯 **内置路由管理** - 简化导航逻辑
- 🚀 **现代异步语法** - 提升开发效率
- 💪 **完整的状态管理** - 集中式数据流
- 🎨 **灵活的事件系统** - 优化性能表现

CHTL 不仅支持 SPA，而且提供了比传统方案更简洁、更高效的开发体验！