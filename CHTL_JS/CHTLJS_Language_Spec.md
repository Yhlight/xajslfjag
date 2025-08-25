# CHTL JS Language Specification

## Important: CHTL JS is NOT JavaScript

CHTL JS is an independent programming language designed specifically for CHTL. It has its own:
- Syntax (completely different from JavaScript)
- Compiler (compiles TO JavaScript, but is not JavaScript itself)
- Language constructs

## Core Principles

1. **Independent Source Language**: CHTL JS source code is not JavaScript and cannot contain JavaScript code
2. **Compiles to JavaScript**: The CHTL JS compiler generates JavaScript as its target output
3. **Own Syntax**: CHTL JS has unique syntax like `{{selector}}`, `->`, etc.
4. **No JS in Source**: CHTL JS source files cannot include raw JavaScript code

## CHTL JS Syntax Examples

### Selectors
```chtljs
// CHTL JS selector syntax
{{.box}}              // Class selector
{{#header}}           // ID selector  
{{button}}            // Element selector
{{.container div}}    // Descendant selector
```

### Event Handling
```chtljs
// CHTL JS event syntax
{{box}}->listen({
    click: () => {
        // CHTL JS code, not JavaScript
        console->log("Clicked!");
    }
});
```

### Animation
```chtljs
// CHTL JS animation syntax
animation fadeIn {
    targets: {{.box}};
    duration: 1000ms;
    keyframes {
        0% { opacity: 0; }
        100% { opacity: 1; }
    }
}
```

### Virtual Objects
```chtljs
// CHTL JS virtual object syntax
vir myHandler = listen({
    click: () => { /* CHTL JS code */ }
});

myHandler->click();  // Invoke virtual method
```

## Runtime Functions

CHTL JS provides its own runtime functions:
- `select_element(tag)` - Select by tag name
- `select_class(className)` - Select by class
- `select_id(id)` - Select by ID
- `smart_select(selector)` - Smart selection
- `add_event_handler(element, event, handler)` - Event binding
- `invoke_animation(animationId)` - Start animation

## Compilation Process

1. CHTL JS source code → CHTL JS Parser
2. CHTL JS AST → CHTL JS Compiler
3. JavaScript output → Browser/JS Engine

The CHTL JS compiler generates JavaScript code as its compilation target. This is similar to how TypeScript compiles to JavaScript, but CHTL JS has completely different syntax and is not related to JavaScript at the source level.

## Example Compilation

CHTL JS source:
```chtljs
{{.box}}->listen({
    click: () => {
        console->log("Clicked!");
    }
});
```

Compiles to JavaScript:
```javascript
Array.from(document.getElementsByClassName('box')).forEach(elem => {
    elem.addEventListener('click', () => {
        console.log("Clicked!");
    });
});
```