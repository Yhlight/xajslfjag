#!/usr/bin/env python3
import sys

def fix_grammar_file(input_file, output_file):
    with open(input_file, 'rb') as f:
        content = f.read()
    
    # 替换实际的控制字符为转义序列
    replacements = [
        (b'[ \t\n\f]', b'[ \\t\\r\\n\\f]'),  # 替换实际的制表符、换行符等
        (b'[ \x09\x0a\x0c]', b'[ \\t\\r\\n\\f]'),  # 明确替换控制字符
    ]
    
    for old, new in replacements:
        content = content.replace(old, new)
    
    # 确保反斜杠是双反斜杠（在单引号字符串中）
    # 查找模式 '\'（单引号中的单反斜杠）
    import re
    # 将 '\' 替换为 '\\'
    content = re.sub(rb"'\\\\' ", rb"'\\\\\\\\' ", content)
    
    with open(output_file, 'wb') as f:
        f.write(content)
    
    print(f"Fixed grammar file written to {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python fix_grammar.py input_file output_file")
        sys.exit(1)
    
    fix_grammar_file(sys.argv[1], sys.argv[2])