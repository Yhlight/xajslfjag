#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include <stack>
#include <string>
#include <unordered_map>

namespace CHTL {

enum class ParsingState {
    // Default state, top-level
    GlobalScope,

    // General States
    InElementBody,
    InAttributeList,
    InTextNodeBody,

    // Style States
    InStyleBlock,
    InStyleProperty,
    InStyleSelector,

    // Script State
    InScriptBlock,

    // Definition States
    InTemplateDefinition,
    InCustomDefinition,
    InOriginDefinition,
    InNamespaceDefinition,
    InConfigurationDefinition,
    InConfigurationNameGroup,
    InConfigurationOriginTypeGroup,

    // Specialization & Usage States
    InStyleSpecialization,
    InElementSpecialization,
    InVarSpecialization,
    InTemplateUsage,
    InVariableUsage,

    // Import & Use States
    InImportStatement,
    InUseStatement,

    // Constraint State
    InExceptClause,
};

struct StateInfo {
    ParsingState state;
    std::string context_name;
    std::unordered_map<std::string, std::string> properties;
    
    StateInfo(ParsingState s = ParsingState::GlobalScope, const std::string& name = "")
        : state(s), context_name(name) {}
};

class CHTLState {
private:
    std::stack<StateInfo> state_stack;
    std::unordered_map<std::string, std::string> global_context;
    
public:
    CHTLState();
    
    void pushState(ParsingState state, const std::string& context_name = "");
    void popState();
    
    ParsingState getCurrentState() const;
    const StateInfo& getCurrentStateInfo() const;
    
    bool isInState(ParsingState state) const;
    bool isInAnyStyleState() const;
    bool isInDefinitionState() const;
    bool isInSpecializationState() const;
    
    void setStateProperty(const std::string& key, const std::string& value);
    std::string getStateProperty(const std::string& key) const;
    
    void setGlobalProperty(const std::string& key, const std::string& value);
    std::string getGlobalProperty(const std::string& key) const;
    
    size_t getDepth() const;
    void clear();
    
    // Utility methods for common state checks
    bool canDefineTemplate() const;
    bool canDefineCustom() const;
    bool canUseTemplate() const;
    bool canUseStyle() const;
    bool canUseScript() const;
    bool canAddAttribute() const;
};

} // namespace CHTL

#endif // CHTL_STATE_H