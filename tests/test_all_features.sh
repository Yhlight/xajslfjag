#!/bin/bash

# CHTL综合测试脚本

echo "===== CHTL Feature Testing Suite ====="
echo

# 创建测试输出目录
mkdir -p test_outputs

# 测试函数
test_feature() {
    local test_name=$1
    local test_file=$2
    local output_file="test_outputs/${test_name}.html"
    
    echo -n "Testing $test_name... "
    
    if ../build/chtlc "$test_file" -o "$output_file" 2>&1 > /dev/null; then
        echo "✓ PASSED"
        return 0
    else
        echo "✗ FAILED"
        ../build/chtlc "$test_file" -o "$output_file" 2>&1
        return 1
    fi
}

# 运行测试
cd tests

echo "1. Basic Features Tests"
test_feature "elements_and_attributes" "basic/elements.chtl"
test_feature "text_nodes" "basic/text.chtl"
test_feature "comments" "basic/comments.chtl"

echo
echo "2. Style System Tests"
test_feature "global_styles" "styles/global.chtl"
test_feature "local_styles" "styles/local.chtl"
test_feature "selector_automation" "styles/auto_selectors.chtl"

echo
echo "3. Template System Tests"
test_feature "style_templates" "templates/style_template.chtl"
test_feature "element_templates" "templates/element_template.chtl"
test_feature "var_templates" "templates/var_template.chtl"

echo
echo "4. Custom System Tests"
test_feature "custom_styles" "custom/custom_style.chtl"
test_feature "custom_elements" "custom/custom_element.chtl"
test_feature "custom_vars" "custom/custom_var.chtl"

echo
echo "5. Advanced Features Tests"
test_feature "imports" "advanced/imports.chtl"
test_feature "namespaces" "advanced/namespaces.chtl"
test_feature "configuration" "advanced/configuration.chtl"
test_feature "constraints" "advanced/constraints.chtl"
test_feature "origin_embedding" "advanced/origin.chtl"

echo
echo "6. Comprehensive Test"
test_feature "comprehensive" "comprehensive_syntax_test.chtl"

echo
echo "===== Test Summary ====="
echo "All tests completed. Check test_outputs/ for generated HTML files."