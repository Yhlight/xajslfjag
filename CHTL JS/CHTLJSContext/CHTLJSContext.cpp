#include "CHTLJSContext.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTLJS {

CHTLJSContext::CHTLJSContext() {
    state_ = std::make_shared<CHTLJSState>();
    reset();
}

void CHTLJSContext::setState(CHTLJSCompilerState newState) {
    if (canTransitionTo(newState)) {
        state_->setState(newState);
    }
}

CHTLJSCompilerState CHTLJSContext::getCurrentState() const {
    return state_->getCurrentState();
}

bool CHTLJSContext::canTransitionTo(CHTLJSCompilerState newState) const {
    return state_->canTransitionTo(newState);
}

bool CHTLJSContext::transitionTo(CHTLJSCompilerState newState, const std::string& trigger) {
    if (canTransitionTo(newState)) {
        StateTransition transition(getCurrentState(), newState, trigger);
        addStateTransition(transition);
        setState(newState);
        return true;
    }
    return false;
}

void CHTLJSContext::pushContext() {
    state_->pushContext();
}

void CHTLJSContext::popContext() {
    state_->popContext();
}

ParseContext& CHTLJSContext::getCurrentContext() {
    return state_->getCurrentContext();
}

const ParseContext& CHTLJSContext::getCurrentContext() const {
    return state_->getCurrentContext();
}

void CHTLJSContext::pushFunction(const std::string& function) {
    state_->pushFunction(function);
}

void CHTLJSContext::popFunction() {
    state_->popFunction();
}

std::string CHTLJSContext::getCurrentFunction() const {
    return state_->getCurrentFunction();
}

std::vector<std::string> CHTLJSContext::getFunctionStack() const {
    return state_->getFunctionStack();
}

void CHTLJSContext::pushObject(const std::string& object) {
    state_->pushObject(object);
}

void CHTLJSContext::popObject() {
    state_->popObject();
}

std::string CHTLJSContext::getCurrentObject() const {
    return state_->getCurrentObject();
}

std::vector<std::string> CHTLJSContext::getObjectStack() const {
    return state_->getObjectStack();
}

void CHTLJSContext::setCurrentModule(const std::string& module) {
    state_->setCurrentModule(module);
}

std::string CHTLJSContext::getCurrentModule() const {
    return state_->getCurrentModule();
}

void CHTLJSContext::setCurrentVirtualObject(const std::string& virtualObject) {
    state_->setCurrentVirtualObject(virtualObject);
}

std::string CHTLJSContext::getCurrentVirtualObject() const {
    return state_->getCurrentVirtualObject();
}

void CHTLJSContext::enterScriptBlock() {
    state_->enterScriptBlock();
}

void CHTLJSContext::exitScriptBlock() {
    state_->exitScriptBlock();
}

void CHTLJSContext::enterModuleBlock() {
    state_->enterModuleBlock();
}

void CHTLJSContext::exitModuleBlock() {
    state_->exitModuleBlock();
}

void CHTLJSContext::enterVirtualObjectBlock() {
    state_->enterVirtualObjectBlock();
}

void CHTLJSContext::exitVirtualObjectBlock() {
    state_->exitVirtualObjectBlock();
}

void CHTLJSContext::enterListenBlock() {
    state_->enterListenBlock();
}

void CHTLJSContext::exitListenBlock() {
    state_->exitListenBlock();
}

void CHTLJSContext::enterDelegateBlock() {
    state_->enterDelegateBlock();
}

void CHTLJSContext::exitDelegateBlock() {
    state_->exitDelegateBlock();
}

void CHTLJSContext::enterAnimateBlock() {
    state_->enterAnimateBlock();
}

void CHTLJSContext::exitAnimateBlock() {
    state_->exitAnimateBlock();
}

void CHTLJSContext::enterINeverAwayBlock() {
    state_->enterINeverAwayBlock();
}

void CHTLJSContext::exitINeverAwayBlock() {
    state_->exitINeverAwayBlock();
}

void CHTLJSContext::enterPrintMyloveBlock() {
    state_->enterPrintMyloveBlock();
}

void CHTLJSContext::exitPrintMyloveBlock() {
    state_->exitPrintMyloveBlock();
}

bool CHTLJSContext::isInScriptBlock() const {
    return state_->isInScriptBlock();
}

bool CHTLJSContext::isInModuleBlock() const {
    return state_->isInModuleBlock();
}

bool CHTLJSContext::isInVirtualObjectBlock() const {
    return state_->isInVirtualObjectBlock();
}

bool CHTLJSContext::isInListenBlock() const {
    return state_->isInListenBlock();
}

bool CHTLJSContext::isInDelegateBlock() const {
    return state_->isInDelegateBlock();
}

bool CHTLJSContext::isInAnimateBlock() const {
    return state_->isInAnimateBlock();
}

bool CHTLJSContext::isInINeverAwayBlock() const {
    return state_->isInINeverAwayBlock();
}

bool CHTLJSContext::isInPrintMyloveBlock() const {
    return state_->isInPrintMyloveBlock();
}

void CHTLJSContext::setError(const std::string& error, size_t line, size_t column) {
    std::ostringstream oss;
    if (line > 0 || column > 0) {
        oss << "[" << line << ":" << column << "] ";
    }
    oss << error;
    state_->setError(oss.str(), line, column);
}

bool CHTLJSContext::hasError() const {
    return state_->hasError();
}

const std::vector<std::string>& CHTLJSContext::getErrors() const {
    return state_->getErrors();
}

void CHTLJSContext::clearErrors() {
    state_->clearErrors();
}

void CHTLJSContext::addStateTransition(const StateTransition& transition) {
    state_->addStateTransition(transition);
}

const std::vector<StateTransition>& CHTLJSContext::getStateHistory() const {
    return state_->getStateHistory();
}

void CHTLJSContext::reset() {
    state_->reset();
    clearErrors();
}

std::string CHTLJSContext::getContextInfo() const {
    std::ostringstream oss;
    oss << "CHTL JS Context Info:\n";
    oss << "  Current State: " << static_cast<int>(getCurrentState()) << "\n";
    oss << "  Function Stack Depth: " << getFunctionStack().size() << "\n";
    oss << "  Object Stack Depth: " << getObjectStack().size() << "\n";
    oss << "  Current Module: " << (getCurrentModule().empty() ? "(none)" : getCurrentModule()) << "\n";
    oss << "  Current Virtual Object: " << (getCurrentVirtualObject().empty() ? "(none)" : getCurrentVirtualObject()) << "\n";
    
    // 块状态信息
    oss << "  Block States:\n";
    oss << "    Script Block: " << (isInScriptBlock() ? "active" : "inactive") << "\n";
    oss << "    Module Block: " << (isInModuleBlock() ? "active" : "inactive") << "\n";
    oss << "    Virtual Object Block: " << (isInVirtualObjectBlock() ? "active" : "inactive") << "\n";
    oss << "    Listen Block: " << (isInListenBlock() ? "active" : "inactive") << "\n";
    oss << "    Delegate Block: " << (isInDelegateBlock() ? "active" : "inactive") << "\n";
    oss << "    Animate Block: " << (isInAnimateBlock() ? "active" : "inactive") << "\n";
    oss << "    I Never Away Block: " << (isInINeverAwayBlock() ? "active" : "inactive") << "\n";
    oss << "    Print My Love Block: " << (isInPrintMyloveBlock() ? "active" : "inactive") << "\n";
    
    return oss.str();
}

std::string CHTLJSContext::getStateInfo() const {
    return state_->getStateInfo();
}

} // namespace CHTLJS