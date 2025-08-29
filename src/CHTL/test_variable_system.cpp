#include <iostream>
#include <string>
#include <memory>
#include <cassert>
#include "CHTLState/State.h"
#include "CHTLContext/Context.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLManage/TemplateManager.h"

using namespace std;

void testVarGroupBasicFunctionality() {
    cout << "Testing VarGroup basic functionality..." << endl;
    
    // 创建变量组
    CHTL::CHTLContext context;
    auto varGroup = context.CreateVarGroup();
    
    if (!varGroup) {
        cout << "✗ Failed to create VarGroup" << endl;
        return;
    }
    
    // 测试设置和获取变量
    varGroup->SetVariable("baseColor", "#3498db");
    varGroup->SetVariable("primaryFont", "Arial, sans-serif");
    varGroup->SetVariable("spacing", "16px");
    varGroup->SetVariable("enabled", "true");
    
    // 验证变量设置成功
    string baseColor = varGroup->GetVariable("baseColor");
    string primaryFont = varGroup->GetVariable("primaryFont");
    string spacing = varGroup->GetVariable("spacing");
    string enabled = varGroup->GetVariable("enabled");
    
    cout << "Variables:" << endl;
    cout << "  baseColor: " << baseColor << endl;
    cout << "  primaryFont: " << primaryFont << endl;
    cout << "  spacing: " << spacing << endl;
    cout << "  enabled: " << enabled << endl;
    
    assert(baseColor == "#3498db");
    assert(primaryFont == "Arial, sans-serif");
    assert(spacing == "16px");
    assert(enabled == "true");
    
    cout << "✓ VarGroup basic functionality working correctly" << endl;
}

void testVarGroupMergeAndInheritance() {
    cout << "\nTesting VarGroup merge and inheritance..." << endl;
    
    CHTL::CHTLContext context;
    
    // 创建父变量组
    auto parentVarGroup = std::make_shared<CHTL::VarGroup>();
    parentVarGroup->SetVariable("baseColor", "#2c3e50");
    parentVarGroup->SetVariable("spacing", "12px");
    parentVarGroup->SetVariable("fontFamily", "Roboto");
    
    // 创建子变量组
    auto childVarGroup = std::make_shared<CHTL::VarGroup>();
    childVarGroup->SetVariable("childColor", "#34495e");
    childVarGroup->SetVariable("spacing", "16px");  // 覆盖父级spacing
    
    // 测试合并
    childVarGroup->Merge(*parentVarGroup);
    
    // 验证合并结果
    assert(childVarGroup->GetVariable("baseColor") == "#2c3e50");    // 来自父级
    assert(childVarGroup->GetVariable("childColor") == "#34495e");   // 子级独有
    assert(childVarGroup->GetVariable("spacing") == "16px");         // 子级覆盖
    assert(childVarGroup->GetVariable("fontFamily") == "Roboto");    // 来自父级
    
    cout << "✓ VarGroup merge and inheritance working correctly" << endl;
}

void testVarGroupHasVariable() {
    cout << "\nTesting VarGroup HasVariable functionality..." << endl;
    
    CHTL::CHTLContext context;
    auto varGroup = context.CreateVarGroup();
    
    varGroup->SetVariable("existingVar", "value");
    
    assert(varGroup->HasVariable("existingVar") == true);
    assert(varGroup->HasVariable("nonExistentVar") == false);
    
    cout << "✓ VarGroup HasVariable functionality working correctly" << endl;
}

void testVarGroupGetAllVariables() {
    cout << "\nTesting VarGroup GetAllVariables functionality..." << endl;
    
    CHTL::CHTLContext context;
    auto varGroup = context.CreateVarGroup();
    
    varGroup->SetVariable("var1", "value1");
    varGroup->SetVariable("var2", "value2");
    varGroup->SetVariable("var3", "value3");
    
    const auto& allVars = varGroup->GetAllVariables();
    
    assert(allVars.size() == 3);
    assert(allVars.at("var1") == "value1");
    assert(allVars.at("var2") == "value2");
    assert(allVars.at("var3") == "value3");
    
    cout << "✓ VarGroup GetAllVariables functionality working correctly" << endl;
}

void testVarGroupClear() {
    cout << "\nTesting VarGroup Clear functionality..." << endl;
    
    CHTL::CHTLContext context;
    auto varGroup = context.CreateVarGroup();
    
    varGroup->SetVariable("var1", "value1");
    varGroup->SetVariable("var2", "value2");
    
    assert(varGroup->GetAllVariables().size() == 2);
    
    varGroup->Clear();
    
    assert(varGroup->GetAllVariables().size() == 0);
    assert(varGroup->HasVariable("var1") == false);
    assert(varGroup->HasVariable("var2") == false);
    
    cout << "✓ VarGroup Clear functionality working correctly" << endl;
}

void testContextProcessedCSS() {
    cout << "\nTesting Context ProcessedCSS functionality..." << endl;
    
    CHTL::CHTLContext context;
    
    string testCSS = R"(.button {
        background-color: #3498db;
        color: white;
        padding: 10px 20px;
        border-radius: 4px;
    })";
    
    context.SetProcessedCSS(testCSS);
    string retrievedCSS = context.GetProcessedCSS();
    
    assert(retrievedCSS == testCSS);
    
    cout << "✓ Context ProcessedCSS functionality working correctly" << endl;
}

void testVarGroupVariableReplacement() {
    cout << "\nTesting VarGroup variable replacement..." << endl;
    
    CHTL::CHTLContext context;
    auto varGroup = context.CreateVarGroup();
    
    varGroup->SetVariable("primaryColor", "#e74c3c");
    varGroup->SetVariable("fontSize", "14px");
    varGroup->SetVariable("borderRadius", "8px");
    
    string input = R"(.button {
        background-color: VarGroup(primaryColor);
        font-size: VarGroup(fontSize);
        border-radius: VarGroup(borderRadius);
        border: 1px solid VarGroup(primaryColor);
    })";
    
    string expected = R"(.button {
        background-color: #e74c3c;
        font-size: 14px;
        border-radius: 8px;
        border: 1px solid #e74c3c;
    })";
    
    string result = CHTL::VarGroup::ReplaceVariables(input, *varGroup);
    
    cout << "Input CSS:" << endl << input << endl;
    cout << "Output CSS:" << endl << result << endl;
    
    // 验证变量替换
    assert(result.find("#e74c3c") != string::npos);
    assert(result.find("14px") != string::npos);
    assert(result.find("8px") != string::npos);
    assert(result.find("VarGroup(") == string::npos); // 应该没有剩余的变量引用
    
    cout << "✓ Variable replacement working correctly" << endl;
}

void testVarGroupContainsVariableReferences() {
    cout << "\nTesting VarGroup ContainsVariableReferences..." << endl;
    
    string withVars = "color: VarGroup(primaryColor); font-size: VarGroup(fontSize);";
    string withoutVars = "color: red; font-size: 14px;";
    
    assert(CHTL::VarGroup::ContainsVariableReferences(withVars) == true);
    assert(CHTL::VarGroup::ContainsVariableReferences(withoutVars) == false);
    
    cout << "✓ ContainsVariableReferences working correctly" << endl;
}

void testVarGroupUnknownVariables() {
    cout << "\nTesting VarGroup unknown variable handling..." << endl;
    
    CHTL::CHTLContext context;
    auto varGroup = context.CreateVarGroup();
    
    varGroup->SetVariable("knownVar", "value");
    
    string input = "known: VarGroup(knownVar); unknown: VarGroup(unknownVar);";
    string result = CHTL::VarGroup::ReplaceVariables(input, *varGroup);
    
    cout << "Input: " << input << endl;
    cout << "Output: " << result << endl;
    
    // 已知变量应该被替换
    assert(result.find("known: value") != string::npos);
    // 未知变量应该保持原样
    assert(result.find("VarGroup(unknownVar)") != string::npos);
    
    cout << "✓ Unknown variable handling working correctly" << endl;
}

int main() {
    cout << "=== CHTL Variable System Test ===" << endl;
    
    testVarGroupBasicFunctionality();
    testVarGroupMergeAndInheritance();
    testVarGroupHasVariable();
    testVarGroupGetAllVariables();
    testVarGroupClear();
    testContextProcessedCSS();
    testVarGroupVariableReplacement();
    testVarGroupContainsVariableReferences();
    testVarGroupUnknownVariables();
    
    cout << "\n=== All Variable System Tests Completed Successfully ===" << endl;
    return 0;
}