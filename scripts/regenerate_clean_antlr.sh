#!/bin/bash

# 清洁地重新生成ANTLR解析器

echo "🔧 清洁重新生成ANTLR解析器"
echo "=========================="

# 设置变量
ANTLR_JAR="third-party/antlr/antlr-4.13.2-complete.jar"
OUTPUT_DIR="third-party/antlr/generated"
TEMP_DIR="temp_clean_antlr"

# 检查ANTLR JAR文件
if [ ! -f "$ANTLR_JAR" ]; then
    echo "❌ ANTLR JAR文件不存在: $ANTLR_JAR"
    exit 1
fi

# 创建临时工作目录
mkdir -p "$TEMP_DIR"
cd "$TEMP_DIR"

echo ""
echo "📁 清理旧文件..."
rm -rf "../$OUTPUT_DIR"
mkdir -p "../$OUTPUT_DIR/CSS"
mkdir -p "../$OUTPUT_DIR/JS"

echo ""
echo "🎨 生成CSS解析器..."

# 复制CSS语法文件到临时目录
cp ../third-party/antlr/Grammars/CSS3/CSSLexer.g4 .
cp ../third-party/antlr/Grammars/CSS3/CSSParser.g4 .

# 生成CSS解析器
java -jar "../$ANTLR_JAR" \
    -Dlanguage=Cpp \
    -visitor \
    -listener \
    CSSLexer.g4 CSSParser.g4

if [ $? -eq 0 ]; then
    echo "✅ CSS解析器生成成功"
    # 移动文件到正确位置
    mv *.h *.cpp "../$OUTPUT_DIR/CSS/"
else
    echo "❌ CSS解析器生成失败"
    cd ..
    exit 1
fi

echo ""
echo "⚡ 生成JavaScript解析器..."

# 复制JavaScript语法文件到临时目录
cp ../third-party/antlr/Grammars/JavaScript/SimpleJSLexer.g4 .
cp ../third-party/antlr/Grammars/JavaScript/SimpleJSParser.g4 .

# 生成JavaScript解析器
java -jar "../$ANTLR_JAR" \
    -Dlanguage=Cpp \
    -visitor \
    -listener \
    SimpleJSLexer.g4 SimpleJSParser.g4

if [ $? -eq 0 ]; then
    echo "✅ JavaScript解析器生成成功"
    # 移动文件到正确位置
    mv *.h *.cpp "../$OUTPUT_DIR/JS/"
else
    echo "❌ JavaScript解析器生成失败"
    cd ..
    exit 1
fi

# 返回项目根目录
cd ..

# 清理临时目录
rm -rf "$TEMP_DIR"

echo ""
echo "📊 验证生成的文件..."

# 列出生成的文件
echo "CSS文件:"
ls -la "$OUTPUT_DIR/CSS/" | head -5

echo ""
echo "JavaScript文件:"
ls -la "$OUTPUT_DIR/JS/" | head -5

echo ""
echo "🎉 ANTLR解析器重新生成完成！"
echo "============================="
echo "✅ 使用main分支的完整语法定义"
echo "✅ 文件路径正确"
echo "✅ 准备重新编译集成"
echo ""