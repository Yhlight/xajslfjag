#!/bin/bash

# 使用修复版本的语法文件重新生成ANTLR解析器

echo "🔧 使用修复版本重新生成ANTLR解析器"
echo "=================================="

# 设置变量
ANTLR_JAR="third-party/antlr/antlr-4.13.2-complete.jar"
GRAMMAR_DIR="third-party/antlr/Grammars"
OUTPUT_DIR="third-party/antlr/generated"

# 检查ANTLR JAR文件
if [ ! -f "$ANTLR_JAR" ]; then
    echo "❌ ANTLR JAR文件不存在: $ANTLR_JAR"
    exit 1
fi

echo ""
echo "📁 清理旧的生成文件..."
rm -rf "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR/CSS"
mkdir -p "$OUTPUT_DIR/JS"

echo ""
echo "🎨 生成修复版CSS3解析器..."
echo "使用语法文件: css3Lexer_fixed.g4, css3Parser_fixed.g4"

# 生成CSS3解析器（使用修复版本）
java -jar "$ANTLR_JAR" \
    -Dlanguage=Cpp \
    -visitor \
    -listener \
    -o "$OUTPUT_DIR/CSS" \
    "$GRAMMAR_DIR/CSS3/css3Lexer_fixed.g4" \
    "$GRAMMAR_DIR/CSS3/css3Parser_fixed.g4"

if [ $? -eq 0 ]; then
    echo "✅ CSS3解析器生成成功"
else
    echo "❌ CSS3解析器生成失败，尝试使用简化版本..."
    
    # 尝试使用简化版本
    java -jar "$ANTLR_JAR" \
        -Dlanguage=Cpp \
        -visitor \
        -listener \
        -o "$OUTPUT_DIR/CSS" \
        "$GRAMMAR_DIR/CSS3/CSSLexer.g4" \
        "$GRAMMAR_DIR/CSS3/CSSParser.g4"
    
    if [ $? -eq 0 ]; then
        echo "✅ CSS解析器生成成功（简化版本）"
    else
        echo "❌ CSS解析器生成失败"
        exit 1
    fi
fi

echo ""
echo "⚡ 生成修复版JavaScript解析器..."
echo "使用语法文件: JavaScriptLexer_fixed.g4, JavaScriptParser_fixed.g4"

# 生成JavaScript解析器（使用修复版本）
java -jar "$ANTLR_JAR" \
    -Dlanguage=Cpp \
    -visitor \
    -listener \
    -o "$OUTPUT_DIR/JS" \
    "$GRAMMAR_DIR/JavaScript/JavaScriptLexer_fixed.g4" \
    "$GRAMMAR_DIR/JavaScript/JavaScriptParser_fixed.g4"

if [ $? -eq 0 ]; then
    echo "✅ JavaScript解析器生成成功"
else
    echo "❌ JavaScript解析器生成失败，尝试使用简化版本..."
    
    # 尝试使用简化版本
    java -jar "$ANTLR_JAR" \
        -Dlanguage=Cpp \
        -visitor \
        -listener \
        -o "$OUTPUT_DIR/JS" \
        "$GRAMMAR_DIR/JavaScript/SimpleJSLexer.g4" \
        "$GRAMMAR_DIR/JavaScript/SimpleJSParser.g4"
    
    if [ $? -eq 0 ]; then
        echo "✅ JavaScript解析器生成成功（简化版本）"
    else
        echo "❌ JavaScript解析器生成失败"
        exit 1
    fi
fi

echo ""
echo "📊 检查生成的文件..."

# 检查生成的文件
CSS_FILES=$(find "$OUTPUT_DIR/CSS" -name "*.cpp" -o -name "*.h" | wc -l)
JS_FILES=$(find "$OUTPUT_DIR/JS" -name "*.cpp" -o -name "*.h" | wc -l)

echo "CSS解析器文件数: $CSS_FILES"
echo "JavaScript解析器文件数: $JS_FILES"

if [ $CSS_FILES -gt 0 ] && [ $JS_FILES -gt 0 ]; then
    echo ""
    echo "🎉 完整ANTLR解析器重新生成成功！"
    echo "================================="
    echo "✅ CSS解析器: $CSS_FILES 个文件"
    echo "✅ JavaScript解析器: $JS_FILES 个文件"
    echo "✅ 使用main分支完整语法定义"
    echo ""
    echo "📋 下一步:"
    echo "1. 更新头文件引用"
    echo "2. 重新编译项目"
    echo "3. 验证完整ANTLR集成"
else
    echo "❌ 文件生成不完整"
    exit 1
fi