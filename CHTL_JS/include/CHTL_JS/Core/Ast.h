#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL_JS {

    // ... (other nodes)

    enum class MemberType { FUNCTION, OBJECT, ARRAY, LITERAL };

    struct MemberInfo {
        std::shared_ptr<AstNode> ValueNode;
        MemberType Type;
    };

    // Represents the compile-time proxy object for a 'vir' variable
    class View {
    public:
        std::string Name;
        // Maps member names to their value's AST node and type
        std::map<std::string, MemberInfo> Members;
    };

    // ... (VirMemberAccessNode remains the same)

}