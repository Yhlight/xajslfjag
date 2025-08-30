#!/bin/bash

# 重新生成完整的ANTLR解析器
# 使用main分支中的完整语法文件

echo "🔧 重新生成完整ANTLR解析器"
echo "==========================="

# 设置变量
ANTLR_JAR="third-party/antlr/antlr-4.13.2-complete.jar"
GRAMMAR_DIR="third-party/antlr/Grammars"
OUTPUT_DIR="third-party/antlr/generated"
TEMP_DIR="temp_antlr_generation"

# 检查ANTLR JAR文件
if [ ! -f "$ANTLR_JAR" ]; then
    echo "❌ ANTLR JAR文件不存在: $ANTLR_JAR"
    exit 1
fi

# 创建临时目录
mkdir -p "$TEMP_DIR"

echo ""
echo "📁 清理旧的生成文件..."
rm -rf "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR/CSS"
mkdir -p "$OUTPUT_DIR/JS"

echo ""
echo "🎨 生成完整CSS3解析器..."
echo "使用语法文件: css3Lexer.g4, css3Parser.g4"

# 生成CSS3解析器
cd "$TEMP_DIR"
java -jar "../$ANTLR_JAR" \
    -Dlanguage=Cpp \
    -visitor \
    -listener \
    -o "../$OUTPUT_DIR/CSS" \
    "../$GRAMMAR_DIR/CSS3/css3Lexer.g4" \
    "../$GRAMMAR_DIR/CSS3/css3Parser.g4"

if [ $? -eq 0 ]; then
    echo "✅ CSS3解析器生成成功"
else
    echo "❌ CSS3解析器生成失败"
    cd ..
    exit 1
fi

echo ""
echo "⚡ 生成完整JavaScript解析器..."
echo "使用语法文件: JavaScriptLexer.g4, JavaScriptParser.g4"

# 生成JavaScript解析器
java -jar "../$ANTLR_JAR" \
    -Dlanguage=Cpp \
    -visitor \
    -listener \
    -o "../$OUTPUT_DIR/JS" \
    "../$GRAMMAR_DIR/JavaScript/JavaScriptLexer.g4" \
    "../$GRAMMAR_DIR/JavaScript/JavaScriptParser.g4"

if [ $? -eq 0 ]; then
    echo "✅ JavaScript解析器生成成功"
else
    echo "❌ JavaScript解析器生成失败"
    cd ..
    exit 1
fi

cd ..

echo ""
echo "📊 检查生成的文件..."

# 检查CSS文件
CSS_FILES=$(find "$OUTPUT_DIR/CSS" -name "*.cpp" -o -name "*.h" | wc -l)
echo "CSS解析器文件数: $CSS_FILES"

# 检查JavaScript文件
JS_FILES=$(find "$OUTPUT_DIR/JS" -name "*.cpp" -o -name "*.h" | wc -l)
echo "JavaScript解析器文件数: $JS_FILES"

echo ""
echo "🔧 更新头文件引用..."

# 更新CSS解析器头文件引用
if [ -f "src/CSS/CSSParser/Parser.h" ]; then
    echo "更新CSS解析器引用..."
    sed -i 's|SimpleJSLexer|css3Lexer|g' src/CSS/CSSParser/Parser.h
    sed -i 's|SimpleJSParser|css3Parser|g' src/CSS/CSSParser/Parser.h
    sed -i 's|CSSLexer|css3Lexer|g' src/CSS/CSSParser/Parser.h
    sed -i 's|CSSParser|css3Parser|g' src/CSS/CSSParser/Parser.h
fi

# 更新JavaScript解析器头文件引用
if [ -f "src/JS/JSParser/Parser.h" ]; then
    echo "更新JavaScript解析器引用..."
    sed -i 's|SimpleJSLexer|JavaScriptLexer|g' src/JS/JSParser/Parser.h
    sed -i 's|SimpleJSParser|JavaScriptParser|g' src/JS/JSParser/Parser.h
fi

echo ""
echo "📦 更新CMakeLists.txt..."

# 更新CMakeLists.txt中的源文件引用
if [ -f "CMakeLists.txt" ]; then
    # 备份原文件
    cp CMakeLists.txt CMakeLists.txt.backup
    
    # 更新CSS源文件引用
    sed -i 's|third-party/antlr/generated/CSS/CSSLexer.cpp|third-party/antlr/generated/CSS/css3Lexer.cpp|g' CMakeLists.txt
    sed -i 's|third-party/antlr/generated/CSS/CSSParser.cpp|third-party/antlr/generated/CSS/css3Parser.cpp|g' CMakeLists.txt
    sed -i 's|third-party/antlr/generated/CSS/CSSParserBaseListener.cpp|third-party/antlr/generated/CSS/css3ParserBaseListener.cpp|g' CMakeLists.txt
    
    # 更新JavaScript源文件引用
    sed -i 's|third-party/antlr/generated/JS/SimpleJSLexer.cpp|third-party/antlr/generated/JS/JavaScriptLexer.cpp|g' CMakeLists.txt
    sed -i 's|third-party/antlr/generated/JS/SimpleJSParser.cpp|third-party/antlr/generated/JS/JavaScriptParser.cpp|g' CMakeLists.txt
    sed -i 's|third-party/antlr/generated/JS/SimpleJSParserBaseListener.cpp|third-party/antlr/generated/JS/JavaScriptParserBaseListener.cpp|g' CMakeLists.txt
fi

# 清理临时目录
rm -rf "$TEMP_DIR"

echo ""
echo "🎉 完整ANTLR解析器重新生成完成！"
echo "================================="
echo "✅ CSS3解析器: 使用完整语法定义"
echo "✅ JavaScript解析器: 使用完整语法定义"
echo "✅ 头文件引用: 已更新"
echo "✅ CMakeLists.txt: 已更新"
echo ""
echo "📋 下一步:"
echo "1. 重新编译项目: cmake --build build --clean-first"
echo "2. 运行测试验证完整ANTLR集成"
echo ""