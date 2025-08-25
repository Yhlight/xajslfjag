#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <variant>

namespace CHTL_JS {

    // Base class for all AST nodes
    struct AstNode {
        virtual ~AstNode() = default;
    };

    // Represents an enhanced selector {{selector}}
    struct SelectorNode : public AstNode {
        std::string Selector;
        std::optional<int> Index; // For {{selector[0]}}
    };

    // Represents a listen() call
    struct ListenNode : public AstNode {
        std::shared_ptr<AstNode> Target;
        std::map<std::string, std::string> EventHandlers; // event -> handler code
    };

    // Represents a delegate() call
    struct DelegateNode : public AstNode {
        std::shared_ptr<AstNode> ParentSelector;
        std::variant<std::shared_ptr<AstNode>, std::vector<std::shared_ptr<AstNode>>> TargetSelectors;
        std::map<std::string, std::string> EventHandlers;
    };

    // Represents an animate() call
    struct AnimateNode : public AstNode {
        std::variant<std::shared_ptr<AstNode>, std::vector<std::shared_ptr<AstNode>>> Targets;
        int Duration;
        std::string Easing;
        std::map<std::string, std::string> BeginState;
        std::vector<std::pair<double, std::map<std::string, std::string>>> KeyFrames; // at -> properties
        std::map<std::string, std::string> EndState;
        int LoopCount = 1;
        std::string Direction = "normal";
        int Delay = 0;
        std::string Callback;
    };

    // Represents a function literal
    struct FunctionLiteralNode : public AstNode {
        std::vector<std::string> Parameters;
        std::string Body;
    };

    // Represents a vir object definition
    struct VirObjectDefNode : public AstNode {
        std::string Name;
        std::string FunctionName; // The CHTL JS function being called
        std::shared_ptr<AstNode> ArgumentsNode;
    };

    // Represents accessing a vir object member
    struct VirMemberAccessNode : public AstNode {
        std::string ObjectName;
        std::string MemberName;
    };

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

}