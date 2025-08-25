# CHTL JS Language Specification

## Important: CHTL JS is NOT JavaScript

CHTL JS is an independent programming language designed specifically for CHTL. It has its own:
- Syntax
- Runtime
- Object model
- DOM manipulation methods
- Event system

## Core Principles

1. **Independent Language**: CHTL JS is not a superset, subset, or variant of JavaScript
2. **Own Compiler**: CHTL JS has its own compiler, separate from JavaScript engines
3. **Own Runtime**: CHTL JS code executes in its own runtime environment
4. **No JS Interop**: CHTL JS code cannot directly call JavaScript functions or vice versa

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
3. CHTL JS Bytecode → CHTL JS Runtime
4. Execution in CHTL JS VM

The CHTL JS compiler NEVER generates JavaScript code. It generates its own bytecode or intermediate representation that runs in the CHTL JS runtime.