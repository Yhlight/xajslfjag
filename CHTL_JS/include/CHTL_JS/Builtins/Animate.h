#pragma once

#include <string>
#include "../Core/Ast.h"

namespace CHTL_JS {
namespace Builtins {

    class AnimateFunction {
    public:
        static std::string GenerateCode(const AstNodePtr& args);
    };

}
}
