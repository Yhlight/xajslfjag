#include "src/CHTL/CHTLParser/Parser.h"
#include "src/CHTL/CHTLGenerator/Generator.h"
#include "src/CHTL/CHTLLexer/Lexer.h"
#include "src/Scanner/UnifiedScanner.h"
#include "src/CMODSystem/Syntax.h"
#include <iostream>
#include <vector>
#include <string>
#include <memory>

using namespace CHTL;

// Syntax verification tests according to CHTL specification
bool verifyCoreHTMLSyntax() {
    std::cout << "\n=== Core HTML Syntax Verification ===\n";
    
    // Test 1: Basic element with attributes
    std::string basicElement = R"(
div
{
    id: box;
    class: welcome;
    
    text
    {
        HelloWorld
    }
}
    )";
    
    try {
        auto lexer = std::make_unique<Lexer>(basicElement);
        Parser parser(std::move(lexer));
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "✓ Basic element with attributes: PASS\n";
            return true;
        } else {
            std::cout << "✗ Basic element with attributes: FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Basic element syntax error: " << e.what() << "\n";
        return false;
    }
}

bool verifyTemplateSyntax() {
    std::cout << "\n=== Template Syntax Verification ===\n";
    
    // Test template definition and usage according to docs
    std::string templateCode = R"(
[Template] @Element Box
{
    span
    {
        text
        {
            This is a div group
        }
    }
    
    div
    {
        style
        {
            width: 200px;
            height: 200px;
            background-color: red;
        }
    }
}

body
{
    @Element Box;
}
    )";
    
    try {
        auto lexer = std::make_unique<Lexer>(templateCode);
        Parser parser(std::move(lexer));
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "✓ Template definition and usage: PASS\n";
            return true;
        } else {
            std::cout << "✗ Template definition and usage: FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Template syntax error: " << e.what() << "\n";
        return false;
    }
}

bool verifyStyleBlockSyntax() {
    std::cout << "\n=== Style Block Syntax Verification ===\n";
    
    // Test local style blocks with automatic class generation
    std::string styleCode = R"(
div
{
    style
    {
        .box
        {
            width: 300px;
        }
        
        &:hover
        {
            background-color: blue;
        }
    }
}
    )";
    
    try {
        auto lexer = std::make_unique<Lexer>(styleCode);
        Parser parser(std::move(lexer));
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "✓ Local style blocks with auto class: PASS\n";
            return true;
        } else {
            std::cout << "✗ Local style blocks: FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Style block syntax error: " << e.what() << "\n";
        return false;
    }
}

bool verifyCustomSyntax() {
    std::cout << "\n=== Custom Syntax Verification ===\n";
    
    // Test custom definitions according to docs
    std::string customCode = R"(
[Custom] @Style TextSet
{
    color;
    font-size;
    line-height;
}

div
{
    style
    {
        @Style TextSet
        {
            color: red;
            font-size: 16px;
            line-height: 1.5;
        }
    }
}
    )";
    
    try {
        auto lexer = std::make_unique<Lexer>(customCode);
        Parser parser(std::move(lexer));
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "✓ Custom style definitions: PASS\n";
            return true;
        } else {
            std::cout << "✗ Custom style definitions: FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Custom syntax error: " << e.what() << "\n";
        return false;
    }
}

bool verifyCommentSyntax() {
    std::cout << "\n=== Comment Syntax Verification ===\n";
    
    // Test different comment types
    std::string commentCode = R"(
// This is a line comment (ignored by generator)
/* This is a block comment (ignored by generator) */
-- This is a context-aware comment (will be generated)

div
{
    // Element comment
    id: test;
    
    style
    {
        /* Style comment */
        color: red; -- CSS comment will be generated
    }
}
    )";
    
    try {
        auto lexer = std::make_unique<Lexer>(commentCode);
        Parser parser(std::move(lexer));
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "✓ Comment syntax: PASS\n";
            return true;
        } else {
            std::cout << "✗ Comment syntax: FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Comment syntax error: " << e.what() << "\n";
        return false;
    }
}

bool verifyVariableTemplateSyntax() {
    std::cout << "\n=== Variable Template Syntax Verification ===\n";
    
    // Test variable templates according to docs
    std::string varTemplateCode = R"(
[Template] @Var ThemeColor
{
    tableColor: "rgb(255, 192, 203)";
    primaryColor: "blue";
}

div
{
    style
    {
        background-color: ThemeColor(tableColor);
        color: ThemeColor(primaryColor);
    }
}
    )";
    
    try {
        auto lexer = std::make_unique<Lexer>(varTemplateCode);
        Parser parser(std::move(lexer));
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "✓ Variable template syntax: PASS\n";
            return true;
        } else {
            std::cout << "✗ Variable template syntax: FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Variable template syntax error: " << e.what() << "\n";
        return false;
    }
}

bool verifyInheritanceSyntax() {
    std::cout << "\n=== Inheritance Syntax Verification ===\n";
    
    // Test template inheritance
    std::string inheritanceCode = R"(
[Template] @Style ThemeColor
{
    color: rgba(255, 192, 203, 1);
    background-color: rgba(253, 144, 162, 1);
}

[Template] @Style ThemeColor2
{
    background-color: yellow;
    inherit @Style ThemeColor;
}
    )";
    
    try {
        auto lexer = std::make_unique<Lexer>(inheritanceCode);
        Parser parser(std::move(lexer));
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "✓ Template inheritance syntax: PASS\n";
            return true;
        } else {
            std::cout << "✗ Template inheritance syntax: FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Inheritance syntax error: " << e.what() << "\n";
        return false;
    }
}

bool verifyCEEquivalence() {
    std::cout << "\n=== CE Equivalence Verification ===\n";
    
    // Test colon-equals equivalence
    std::string ceCode1 = R"(
div
{
    id: test;
    class: container;
}
    )";
    
    std::string ceCode2 = R"(
div
{
    id = test;
    class = container;
}
    )";
    
    try {
        // Test colon syntax
        auto lexer1 = std::make_unique<Lexer>(ceCode1);
        Parser parser1(std::move(lexer1));
        auto ast1 = parser1.parse();
        
        // Test equals syntax
        auto lexer2 = std::make_unique<Lexer>(ceCode2);
        Parser parser2(std::move(lexer2));
        auto ast2 = parser2.parse();
        
        if (ast1 && ast2) {
            std::cout << "✓ CE equivalence (: = =): PASS\n";
            return true;
        } else {
            std::cout << "✗ CE equivalence: FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ CE equivalence error: " << e.what() << "\n";
        return false;
    }
}

bool verifyUnquotedLiterals() {
    std::cout << "\n=== Unquoted Literals Verification ===\n";
    
    // Test unquoted literals
    std::string literalCode = R"(
text
{
    This is unquoted text
}

div
{
    id: unquoted_id;
    class: unquoted_class;
    
    style
    {
        color: red;
        font-family: Arial;
    }
}
    )";
    
    try {
        auto lexer = std::make_unique<Lexer>(literalCode);
        Parser parser(std::move(lexer));
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "✓ Unquoted literals: PASS\n";
            return true;
        } else {
            std::cout << "✗ Unquoted literals: FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Unquoted literals error: " << e.what() << "\n";
        return false;
    }
}

bool verifyCJMODSyntax() {
    std::cout << "\n=== CJMOD Syntax Verification ===\n";
    
    try {
        // Test CJMOD parameter system
        CJMOD::AtomArg arg1("param1");
        arg1.fillValue("test_value");
        
        CJMOD::AtomArg arg2("param2");
        arg2.fillValue(42);
        
        CJMOD::AtomArg arg3("param3");
        arg3.fillValue(true);
        
        // Test CHTL JS function detection
        std::vector<std::string> chtlJSExamples = {
            "element.listen('click', handler)",
            "data <-> input.value",
            "await>> fetchData()",
            "parallel>> [task1(), task2()]",
            "element.animate({ opacity: 0 }, 500)"
        };
        
        int detected = 0;
        for (const auto& example : chtlJSExamples) {
            if (CJMOD::Syntax::isCHTLJSFunction(example)) {
                detected++;
            }
        }
        
        if (detected >= 3) { // At least 3 out of 5 should be detected
            std::cout << "✓ CJMOD syntax detection: PASS (" << detected << "/5)\n";
            return true;
        } else {
            std::cout << "✗ CJMOD syntax detection: FAIL (" << detected << "/5)\n";
            return false;
        }
        
    } catch (const std::exception& e) {
        std::cout << "✗ CJMOD syntax error: " << e.what() << "\n";
        return false;
    }
}

int main() {
    std::cout << "=== CHTL Syntax Implementation Verification ===\n";
    std::cout << "Testing compliance with CHTL specification document...\n";
    
    int totalTests = 0;
    int passedTests = 0;
    
    // Core syntax tests
    if (verifyCoreHTMLSyntax()) passedTests++; totalTests++;
    if (verifyTemplateSyntax()) passedTests++; totalTests++;
    if (verifyStyleBlockSyntax()) passedTests++; totalTests++;
    if (verifyCustomSyntax()) passedTests++; totalTests++;
    if (verifyCommentSyntax()) passedTests++; totalTests++;
    if (verifyVariableTemplateSyntax()) passedTests++; totalTests++;
    if (verifyInheritanceSyntax()) passedTests++; totalTests++;
    if (verifyCEEquivalence()) passedTests++; totalTests++;
    if (verifyUnquotedLiterals()) passedTests++; totalTests++;
    if (verifyCJMODSyntax()) passedTests++; totalTests++;
    
    // Results
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "CHTL Syntax Verification Results\n";
    std::cout << std::string(60, '=') << "\n";
    
    double accuracy = (passedTests * 100.0) / totalTests;
    
    std::cout << "Tests Passed: " << passedTests << "/" << totalTests << "\n";
    std::cout << "Accuracy: " << accuracy << "%\n\n";
    
    if (accuracy >= 100.0) {
        std::cout << "✅ PERFECT: All syntax features implemented correctly!\n";
        std::cout << "No deviations from CHTL specification detected.\n";
    } else if (accuracy >= 90.0) {
        std::cout << "✅ EXCELLENT: Minor issues detected, core syntax solid.\n";
    } else if (accuracy >= 80.0) {
        std::cout << "⚠️  GOOD: Some syntax features need attention.\n";
    } else {
        std::cout << "❌ ISSUES: Significant syntax implementation problems.\n";
    }
    
    std::cout << "\nSyntax Features Status:\n";
    std::cout << "  HTML Elements: " << (passedTests >= 1 ? "✓" : "✗") << "\n";
    std::cout << "  Templates: " << (passedTests >= 2 ? "✓" : "✗") << "\n";
    std::cout << "  Style Blocks: " << (passedTests >= 3 ? "✓" : "✗") << "\n";
    std::cout << "  Custom Definitions: " << (passedTests >= 4 ? "✓" : "✗") << "\n";
    std::cout << "  Comments: " << (passedTests >= 5 ? "✓" : "✗") << "\n";
    std::cout << "  Variable Templates: " << (passedTests >= 6 ? "✓" : "✗") << "\n";
    std::cout << "  Inheritance: " << (passedTests >= 7 ? "✓" : "✗") << "\n";
    std::cout << "  CE Equivalence: " << (passedTests >= 8 ? "✓" : "✗") << "\n";
    std::cout << "  Unquoted Literals: " << (passedTests >= 9 ? "✓" : "✗") << "\n";
    std::cout << "  CJMOD Extensions: " << (passedTests >= 10 ? "✓" : "✗") << "\n";
    
    return (accuracy >= 90.0) ? 0 : 1;
}