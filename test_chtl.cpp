#include "CHTL/CHTL.h"
#include <iostream>
#include <fstream>

int main() {
    std::cout << "CHTL Compiler Test v" << CHTL::getVersion() << std::endl;
    std::cout << "================================" << std::endl;
    
    // Test 1: Simple HTML element
    std::string test1 = R"(
use html5;

html {
    head {
        title {
            text {
                CHTL Test Page
            }
        }
    }
    
    body {
        div {
            id: container;
            class: main-content;
            
            h1 {
                text {
                    Welcome to CHTL
                }
            }
            
            p {
                text {
                    This is a test of the CHTL compiler.
                }
            }
        }
    }
}
)";

    std::cout << "Test 1: Basic HTML structure" << std::endl;
    auto result1 = CHTL::compile(test1);
    
    if (result1.success) {
        std::cout << "✓ Compilation successful!" << std::endl;
        std::cout << "HTML Output:" << std::endl;
        std::cout << result1.html << std::endl;
        
        if (!result1.css.empty()) {
            std::cout << "CSS Output:" << std::endl;
            std::cout << result1.css << std::endl;
        }
        
        if (!result1.js.empty()) {
            std::cout << "JS Output:" << std::endl;
            std::cout << result1.js << std::endl;
        }
    } else {
        std::cout << "✗ Compilation failed!" << std::endl;
        for (const auto& error : result1.errors) {
            std::cout << "Error: " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
    
    // Test 2: Template definition and usage
    std::string test2 = R"(
[Template] @Style DefaultButton {
    background-color: blue;
    color: white;
    padding: 10px 20px;
    border: none;
    border-radius: 5px;
}

[Template] @Element Card {
    div {
        style {
            .card {
                border: 1px solid #ccc;
                border-radius: 8px;
                padding: 16px;
                margin: 8px;
            }
        }
        
        h3 {
            text { Card Title }
        }
        
        p {
            text { Card content goes here. }
        }
    }
}

body {
    @Element Card;
    
    button {
        style {
            @Style DefaultButton;
        }
        
        text {
            Click Me
        }
    }
}
)";

    std::cout << "Test 2: Templates and styles" << std::endl;
    auto result2 = CHTL::compile(test2);
    
    if (result2.success) {
        std::cout << "✓ Compilation successful!" << std::endl;
        std::cout << "HTML Output:" << std::endl;
        std::cout << result2.html << std::endl;
        
        if (!result2.css.empty()) {
            std::cout << "CSS Output:" << std::endl;
            std::cout << result2.css << std::endl;
        }
    } else {
        std::cout << "✗ Compilation failed!" << std::endl;
        for (const auto& error : result2.errors) {
            std::cout << "Error: " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
    
    // Test 3: Local styles
    std::string test3 = R"(
div {
    id: main-container;
    
    style {
        width: 100%;
        max-width: 1200px;
        margin: 0 auto;
        
        .highlight {
            background-color: yellow;
            padding: 4px;
        }
        
        #main-container {
            border: 2px solid black;
        }
    }
    
    p {
        class: highlight;
        
        text {
            This paragraph will be highlighted.
        }
    }
}
)";

    std::cout << "Test 3: Local styles and selectors" << std::endl;
    auto result3 = CHTL::compile(test3);
    
    if (result3.success) {
        std::cout << "✓ Compilation successful!" << std::endl;
        std::cout << "HTML Output:" << std::endl;
        std::cout << result3.html << std::endl;
        
        if (!result3.css.empty()) {
            std::cout << "CSS Output:" << std::endl;
            std::cout << result3.css << std::endl;
        }
    } else {
        std::cout << "✗ Compilation failed!" << std::endl;
        for (const auto& error : result3.errors) {
            std::cout << "Error: " << error << std::endl;
        }
    }
    
    // Save test results to files
    if (result1.success) {
        std::ofstream html_file("test_output.html");
        html_file << result1.html;
        html_file.close();
        
        if (!result1.css.empty()) {
            std::ofstream css_file("test_output.css");
            css_file << result1.css;
            css_file.close();
        }
        
        std::cout << "Output saved to test_output.html" << std::endl;
    }
    
    return 0;
}