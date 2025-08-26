# CHTL Compiler

A complete C++17 implementation of the CHTL (C++ Hypertext Template Language) compiler.

## Overview

CHTL is a powerful template language that compiles to HTML, CSS, and JavaScript. It features:

- **Template System**: Reusable style, element, and variable templates
- **Custom Elements**: Define custom HTML elements with encapsulated styles
- **Local Styles**: CSS scoped to specific elements with automatic selector generation
- **Namespace Support**: Organize code with namespace declarations
- **Module System**: Import and use external CHTL modules (CMOD files)
- **Configuration Groups**: Fine-grained control over compilation behavior

## Project Structure

```
CHTL/
├── CHTLLexer/          # Lexical analysis (tokenization)
├── CHTLParser/         # Syntax analysis (AST generation)
├── CHTLGenerator/      # Code generation (HTML/CSS/JS output)
├── CHTLNode/           # AST node system
├── CHTLState/          # Parsing state management
├── CHTLContext/        # Compilation context and configuration
├── CHTLLoader/         # File and module loading
├── CHTLManage/         # Main compiler management
└── CMODSystem/         # Module system implementation
```

## Building

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 6+, MSVC 2017+)
- Make (for Unix-like systems)

### Build Instructions

```bash
# Clone and build
git clone <repository>
cd chtl-compiler

# Build the library and test program
make all

# Run tests
make test

# Clean build files
make clean
```

## Usage

### Simple API

```cpp
#include "CHTL/CHTL.h"

// Compile CHTL source to HTML/CSS/JS
std::string chtl_source = R"(
    use html5;
    
    html {
        body {
            div {
                id: container;
                
                h1 {
                    text { Hello, CHTL! }
                }
                
                style {
                    #container {
                        max-width: 800px;
                        margin: 0 auto;
                    }
                    
                    h1 {
                        color: blue;
                        text-align: center;
                    }
                }
            }
        }
    }
)";

auto result = CHTL::compile(chtl_source);

if (result.success) {
    std::cout << "HTML:\n" << result.html << std::endl;
    std::cout << "CSS:\n" << result.css << std::endl;
} else {
    for (const auto& error : result.errors) {
        std::cerr << "Error: " << error << std::endl;
    }
}
```

### Advanced Usage with Manager

```cpp
#include "CHTL/CHTLManage/CHTLManager.h"

CHTL::CHTLManager manager;

// Configure compiler
manager.setPrettyPrint(true);
manager.setDebugMode(false);
manager.addIncludePath("./templates");

// Compile file
auto result = manager.compileFile("example.chtl");

// Save output
manager.saveCompilationResult(result, "output/example");
```

## CHTL Language Features

### Basic Elements

```chtl
div {
    id: my-div;
    class: container;
    
    p {
        text {
            This is a paragraph inside a div.
        }
    }
}
```

### Templates

```chtl
[Template] @Style ButtonStyle {
    background-color: blue;
    color: white;
    padding: 10px 20px;
    border: none;
    border-radius: 5px;
}

[Template] @Element Card {
    div {
        class: card;
        
        h3 {
            text { Card Title }
        }
        
        p {
            text { Card content }
        }
    }
}

// Usage
button {
    style {
        @Style ButtonStyle;
    }
    
    text { Click Me }
}

@Element Card;
```

### Local Styles

```chtl
div {
    id: main-content;
    
    style {
        // Direct styles applied to this element
        width: 100%;
        max-width: 1200px;
        margin: 0 auto;
        
        // Nested selectors
        .highlight {
            background-color: yellow;
            padding: 4px;
        }
        
        &:hover {
            background-color: #f0f0f0;
        }
    }
    
    p {
        class: highlight;
        text { Highlighted text }
    }
}
```

### Namespaces

```chtl
[Namespace] MyComponents {
    [Template] @Style PrimaryButton {
        background-color: #007bff;
        color: white;
    }
    
    [Template] @Element Modal {
        div {
            class: modal;
            // Modal content
        }
    }
}

// Usage
use MyComponents;

button {
    style {
        @Style PrimaryButton;
    }
}
```

### Configuration

```chtl
[Configuration] {
    INDEX_INITIAL_COUNT: 1;
    DISABLE_STYLE_AUTO_ADD_CLASS: false;
    DEBUG_MODE: true;
}
```

## API Reference

### Core Classes

- **`CHTLLexer`**: Tokenizes CHTL source code
- **`CHTLParser`**: Parses tokens into Abstract Syntax Tree
- **`CHTLGenerator`**: Generates HTML/CSS/JS from AST
- **`CHTLManager`**: High-level compilation management
- **`CHTLNode`**: AST node representation
- **`CHTLContext`**: Compilation context and configuration
- **`GlobalMap`**: Symbol table for templates, customs, and variables

### Main Functions

```cpp
// Simple compilation
CHTL::CompileResult compile(const std::string& source, const CHTL::CompileOptions& options = {});
CHTL::CompileResult compileFile(const std::string& filepath, const CHTL::CompileOptions& options = {});

// Individual output
std::string compileToHTML(const std::string& source, const CHTL::CompileOptions& options = {});
std::string compileToCSS(const std::string& source, const CHTL::CompileOptions& options = {});
std::string compileToJS(const std::string& source, const CHTL::CompileOptions& options = {});
```

## Error Handling

The compiler provides detailed error reporting:

```cpp
auto result = CHTL::compile(source);

if (!result.success) {
    for (const auto& error : result.errors) {
        std::cerr << "Error: " << error << std::endl;
    }
}
```

## Testing

Run the test suite:

```bash
make test
```

The test program demonstrates:
- Basic HTML structure compilation
- Template definition and usage
- Local style processing
- Error handling

## Contributing

1. Follow C++17 standards
2. Maintain consistent code style
3. Add tests for new features
4. Update documentation

## License

This project is licensed under the MIT License.

## Documentation

For complete language specification, see:
- `CHTL语法文档.md` - Complete CHTL syntax documentation
- `目标规划.ini` - Project architecture and goals
- `完善选择器自动化与引用规则.ini` - Advanced features specification

## Version

Current version: 1.0.0

Built with C++17 for cross-platform compatibility.