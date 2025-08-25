#include "CHTL_JS/Parser/Parser.h"
#include "CHTL_JS/Core/ViewRegistry.h"

namespace CHTL_JS {

// ... (other parse functions)

void Parser::ParseVirDefinition() {
    // ... (consume vir, name, =)

    // ... (get rhs function call and object literal)

    View newView;
    newView.Name = viewName;

    // Extract method names and types from the keys of the object literal.
    for (const auto& property : objectLiteral->Properties) {
        MemberInfo memberInfo;
        memberInfo.ValueNode = property.Value;

        // Determine the type of the member
        if (std::dynamic_pointer_cast<FunctionLiteralNode>(property.Value)) {
            memberInfo.Type = MemberType::FUNCTION;
        } else if (std::dynamic_pointer_cast<ObjectLiteralNode>(property.Value)) {
            memberInfo.Type = MemberType::OBJECT;
        } else if (std::dynamic_pointer_cast<ArrayLiteralNode>(property.Value)) {
            memberInfo.Type = MemberType::ARRAY;
        } else {
            memberInfo.Type = MemberType::LITERAL;
        }
        
        newView.Members[property.Key] = memberInfo;
    }

    ViewRegistry::GetInstance().RegisterView(viewName, std::move(newView));
}

}