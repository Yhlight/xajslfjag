# CHTL管理器架构

## 设计原则

遵循**单一职责原则**，每个管理器只负责一个特定领域的功能。

## 管理器层次结构

```
CHTLManager (总协调器)
├── SymbolManager      - 符号创建和管理
├── PathManager        - 路径解析和文件系统操作
├── ImportManager      - 导入记录管理
├── NamespaceManager   - 命名空间管理
├── ConfigManager      - 配置管理
├── DependencyManager  - 依赖关系跟踪
├── SelectorManager    - 选择器解析和自动化
├── InheritanceManager - 继承关系管理
└── GlobalMap          - 全局符号注册表（纯数据存储）
```

## 各管理器职责

### 1. SymbolManager（符号管理器）
- **职责**：符号的创建、注册和查询
- **功能**：
  - 创建各类符号（模板、自定义、原始嵌入等）
  - 符号注册到GlobalMap
  - 符号查询和验证
  - 符号元数据管理

### 2. PathManager（路径管理器）
- **职责**：所有路径相关操作
- **功能**：
  - 路径类型检测（绝对、相对、模块名、通配符）
  - 路径解析和规范化
  - 模块路径搜索
  - 文件系统操作
  - 官方模块处理

### 3. ImportManager（导入管理器）
- **职责**：导入记录的管理
- **功能**：
  - 导入记录的增删改查
  - 导入别名管理
  - 导入状态跟踪
  - 反向导入索引
  - **不负责**：路径解析（由PathManager处理）

### 4. NamespaceManager（命名空间管理器）
- **职责**：命名空间的创建和管理
- **功能**：
  - 命名空间创建和嵌套
  - 同名命名空间自动合并
  - 命名空间符号管理
  - 默认命名空间处理
  - 命名空间别名

### 5. ConfigManager（配置管理器）
- **职责**：配置选项的管理
- **功能**：
  - 配置选项存储和访问
  - 活动配置管理
  - Name组和OriginType管理
  - 选择器自动化配置
  - 配置合并和验证

### 6. DependencyManager（依赖管理器）
- **职责**：依赖关系的跟踪和分析
- **功能**：
  - 依赖图构建
  - 循环依赖检测
  - 拓扑排序
  - 依赖分析
  - **不负责**：具体的导入处理（由ImportManager处理）

### 7. SelectorManager（选择器管理器）
- **职责**：选择器的解析和自动化
- **功能**：
  - 选择器类型解析
  - 选择器自动化规则应用
  - 引用（&）解析
  - 选择器优先级计算
  - CSS/JS选择器转换

### 8. InheritanceManager（继承管理器）
- **职责**：继承关系的管理和解析
- **功能**：
  - 继承图构建
  - 属性继承解析
  - delete操作处理
  - 循环继承检测
  - 继承冲突解决

### 9. GlobalMap（全局符号注册表）
- **职责**：纯粹的符号存储和索引
- **功能**：
  - 符号注册和查询
  - 多维度索引（名称、类型、命名空间、文件）
  - 冲突检测
  - 统计信息
  - **不负责**：任何管理逻辑

## 协作模式

### 导入处理流程
```
1. Parser解析导入语句 → ImportRecord
2. CHTLManager.processImport()
   ├── PathManager.resolvePath() - 解析路径
   ├── ImportManager.addImportRecord() - 记录导入
   ├── DependencyManager.addDependency() - 添加依赖
   └── NamespaceManager.addImportedSymbol() - 处理命名空间
```

### 符号注册流程
```
1. Parser解析符号定义
2. CHTLManager.createSymbol()
   ├── SymbolManager.createXXXSymbol() - 创建符号
   ├── NamespaceManager.getCurrentNamespace() - 获取命名空间
   └── SymbolManager.registerSymbol() → GlobalMap - 注册
```

### 选择器自动化流程
```
1. Parser解析局部style/script
2. SelectorManager.parseSelectors() - 解析选择器
3. SelectorManager.autoGenerateSelectors() - 自动生成
4. 根据ConfigManager的规则应用
```

## 优势

1. **高内聚低耦合**：每个管理器专注于自己的领域
2. **易于测试**：每个管理器可以独立测试
3. **易于扩展**：添加新功能只需要修改相关管理器
4. **清晰的接口**：CHTLManager提供统一的高层接口
5. **性能优化**：每个管理器可以独立优化其数据结构

## 使用示例

```cpp
// 初始化
auto manager = std::make_shared<CHTLManager>();
manager->initialize();
manager->setCurrentFile("test.chtl");

// 配置管理
manager->loadConfiguration("default");

// 路径解析
std::string resolvedPath = manager->resolvePath("Chtholly", ImportType::CHTL_MODULE);

// 符号创建和注册
auto symbol = manager->createSymbol("Box", "@Element", SymbolType::ELEMENT_TEMPLATE);
manager->registerSymbol(symbol);

// 导入处理
ImportRecord import;
import.sourcePath = "test.chtl";
import.rawPath = "Chtholly";
import.baseType = ImportType::CHTL_MODULE;
manager->processImport(import);

// 依赖检查
if (manager->checkCircularDependencies()) {
    auto errors = manager->getAllErrors();
    // 处理错误...
}
```