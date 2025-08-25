#pragma once

#include <string>
#include "../Core/Ast.h"

namespace CHTL_JS {
namespace Builtins {

    class DelegateFunction {
    public:
        static std::string GenerateCode(const AstNodePtr& target, const AstNodePtr& args);
    };

}
}
