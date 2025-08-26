#include "CodeSlice.h"
#include <sstream>

namespace CHTL {

std::string CodeSlice::ToString() const {
    std::stringstream ss;
    
    // 类型名称
    std::string typeName;
    switch (type_) {
        case SliceType::UNKNOWN: typeName = "UNKNOWN"; break;
        case SliceType::CHTL: typeName = "CHTL"; break;
        case SliceType::CHTL_JS: typeName = "CHTL_JS"; break;
        case SliceType::CSS: typeName = "CSS"; break;
        case SliceType::JAVASCRIPT: typeName = "JAVASCRIPT"; break;
        case SliceType::HTML: typeName = "HTML"; break;
        case SliceType::TEXT: typeName = "TEXT"; break;
    }
    
    // 上下文名称
    std::string contextName;
    switch (context_) {
        case SliceContext::GLOBAL: contextName = "GLOBAL"; break;
        case SliceContext::ELEMENT_BODY: contextName = "ELEMENT_BODY"; break;
        case SliceContext::LOCAL_STYLE: contextName = "LOCAL_STYLE"; break;
        case SliceContext::LOCAL_SCRIPT: contextName = "LOCAL_SCRIPT"; break;
        case SliceContext::GLOBAL_STYLE: contextName = "GLOBAL_STYLE"; break;
        case SliceContext::GLOBAL_SCRIPT: contextName = "GLOBAL_SCRIPT"; break;
        case SliceContext::TEMPLATE_DEFINITION: contextName = "TEMPLATE_DEFINITION"; break;
        case SliceContext::CUSTOM_DEFINITION: contextName = "CUSTOM_DEFINITION"; break;
        case SliceContext::CONFIGURATION: contextName = "CONFIGURATION"; break;
        case SliceContext::ORIGIN_BLOCK: contextName = "ORIGIN_BLOCK"; break;
    }
    
    ss << "CodeSlice(" << typeName << ", " << contextName << ", "
       << startLocation_.ToString() << "-" << endLocation_.ToString() 
       << ", size=" << content_.size() << ")";
    
    return ss.str();
}

} // namespace CHTL