#pragma once

#include <string>

namespace CHTL_JS {

    // CHTL JS Runtime definitions
    // These represent CHTL JS's own runtime functions, not JavaScript
    
    struct RuntimeFunction {
        std::string Name;
        std::string Signature;
        std::string Implementation; // CHTL JS bytecode or intermediate representation
    };
    
    // CHTL JS has its own object model
    struct CHTLJSObject {
        enum Type { Element, Array, Function, String, Number, Boolean };
        Type ObjectType;
        std::string Value;
    };
    
    // CHTL JS DOM manipulation is independent from JavaScript DOM
    namespace DOM {
        const char* SelectElement = "CHTLJS_SELECT_ELEMENT";
        const char* SelectClass = "CHTLJS_SELECT_CLASS";
        const char* SelectId = "CHTLJS_SELECT_ID";
        const char* AddEventHandler = "CHTLJS_ADD_EVENT_HANDLER";
        const char* Animate = "CHTLJS_ANIMATE";
    }
    
    // CHTL JS has its own event system
    namespace Events {
        const char* Click = "CHTLJS_CLICK";
        const char* MouseEnter = "CHTLJS_MOUSEENTER";
        const char* MouseLeave = "CHTLJS_MOUSELEAVE";
        const char* KeyPress = "CHTLJS_KEYPRESS";
    }

}