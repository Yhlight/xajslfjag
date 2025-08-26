# ThirdParty

这个目录用于存放第三方库和依赖。

## 当前包含的第三方库

### ANTLR4
- **用途**: CSS和JavaScript语法解析
- **版本**: 4.13.2
- **文件**: `../antlr-4.13.2-complete.jar`
- **语法文件**:
  - `../Grammars/CSS3/css3Lexer.g4`
  - `../Grammars/CSS3/css3Parser.g4`
  - `../Grammars/JavaScript/JavaScriptLexer.g4`
  - `../Grammars/JavaScript/JavaScriptParser.g4`

### 可能的未来第三方库

1. **JSON库** - 用于配置文件解析
2. **ZIP库** - 用于CMOD/CJMOD打包
3. **HTTP库** - 用于模块下载
4. **文件系统库** - 跨平台文件操作

## 使用说明

当添加新的第三方库时，请：

1. 将库文件放在对应的子目录中
2. 更新此README文档
3. 在CMakeLists.txt中添加相应的链接配置
4. 确保遵循各库的许可证要求

## 许可证

请确保所有第三方库都与CHTL项目的许可证兼容。