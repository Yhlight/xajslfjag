#pragma once

#include "CHTL_JS/Core/Ast.h"
#include <string>

namespace CHTL_JS {

    // Processes a delegate() call and registers it with the DelegateRegistry.
    class DelegateHandler {
    public:
        static void Process(const DelegateNode& node);
    };

}
