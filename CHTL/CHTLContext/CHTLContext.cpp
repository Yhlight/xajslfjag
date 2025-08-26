#include "CHTLContext.h"

namespace CHTL {

CHTLContext::CHTLContext() {
    // Initialize default configuration
    configuration["INDEX_INITIAL_COUNT"] = "0";
    configuration["DISABLE_NAME_GROUP"] = "true";
    configuration["DISABLE_CUSTOM_ORIGIN_TYPE"] = "false";
    configuration["DEBUG_MODE"] = "false";
    configuration["DISABLE_STYLE_AUTO_ADD_CLASS"] = "false";
    configuration["DISABLE_STYLE_AUTO_ADD_ID"] = "false";
    configuration["DISABLE_SCRIPT_AUTO_ADD_CLASS"] = "true";
    configuration["DISABLE_SCRIPT_AUTO_ADD_ID"] = "true";
    configuration["DISABLE_DEFAULT_NAMESPACE"] = "false";
}

void CHTLContext::setCurrentNamespace(const std::string& ns) {
    current_namespace = ns;
}

void CHTLContext::setCurrentElement(const std::string& element) {
    current_element = element;
}

void CHTLContext::setCurrentTemplate(const std::string& template_name) {
    current_template = template_name;
}

void CHTLContext::setCurrentCustom(const std::string& custom_name) {
    current_custom = custom_name;
}

void CHTLContext::setConfig(const std::string& key, const std::string& value) {
    configuration[key] = value;
}

std::string CHTLContext::getConfig(const std::string& key) const {
    auto it = configuration.find(key);
    return (it != configuration.end()) ? it->second : "";
}

bool CHTLContext::hasConfig(const std::string& key) const {
    return configuration.find(key) != configuration.end();
}

void CHTLContext::addConstraint(const std::string& constraint) {
    local_constraints.insert(constraint);
}

void CHTLContext::removeConstraint(const std::string& constraint) {
    local_constraints.erase(constraint);
}

bool CHTLContext::hasConstraint(const std::string& constraint) const {
    return local_constraints.find(constraint) != local_constraints.end();
}

bool CHTLContext::isAllowed(const std::string& operation) const {
    return !hasConstraint("disable_" + operation);
}

void CHTLContext::clear() {
    current_namespace.clear();
    current_element.clear();
    current_template.clear();
    current_custom.clear();
    local_constraints.clear();
    // Keep configuration
}

void CHTLContext::clearLocal() {
    current_element.clear();
    current_template.clear();
    current_custom.clear();
    local_constraints.clear();
}

} // namespace CHTL