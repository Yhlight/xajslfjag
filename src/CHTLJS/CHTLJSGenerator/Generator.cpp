#include "Generator.h"
#include <algorithm>
#include <random>

namespace CHTLJS {

Generator::Generator(std::shared_ptr<CompileContext> context,
                     const GeneratorConfig& config)
    : context_(context), config_(config) {}

std::string Generator::generate(std::shared_ptr<ProgramNode> program) {
    output_.str("");
    moduleLoader_.str("");
    indentLevel_ = 0;
    
    // 生成辅助函数
    generateSelectorHelpers();
    generateEventDelegationSystem();
    generateAnimationHelpers();
    generateVirtualObjectSystem();
    
    // 如果需要包装在IIFE中
    if (config_.wrapInIIFE) {
        writeLine("(function() {");
        indent();
        writeLine("'use strict';");
        writeLine();
    }
    
    // 访问AST
    program->accept(this);
    
    // 如果有模块加载器代码，添加到开头
    if (!moduleLoader_.str().empty()) {
        std::string mainCode = output_.str();
        output_.str("");
        output_ << moduleLoader_.str() << "\n" << mainCode;
    }
    
    if (config_.wrapInIIFE) {
        dedent();
        writeLine("})();");
    }
    
    return output_.str();
}

void Generator::visitProgramNode(ProgramNode* node) {
    for (const auto& statement : node->getStatements()) {
        statement->accept(this);
    }
}

void Generator::visitStatementNode(StatementNode* node) {
    if (node->getExpression()) {
        node->getExpression()->accept(this);
        writeLine(";");
    }
}

void Generator::visitIdentifierNode(IdentifierNode* node) {
    write(node->getName());
}

void Generator::visitLiteralNode(LiteralNode* node) {
    switch (node->getLiteralType()) {
        case LiteralNode::LiteralType::STRING:
            write("\"" + escapeString(std::get<std::string>(node->getValue())) + "\"");
            break;
        case LiteralNode::LiteralType::NUMBER:
            if (std::holds_alternative<int64_t>(node->getValue())) {
                write(std::to_string(std::get<int64_t>(node->getValue())));
            } else {
                write(std::to_string(std::get<double>(node->getValue())));
            }
            break;
        case LiteralNode::LiteralType::BOOLEAN:
            write(std::get<bool>(node->getValue()) ? "true" : "false");
            break;
        case LiteralNode::LiteralType::NULL_VALUE:
            write("null");
            break;
        case LiteralNode::LiteralType::UNQUOTED:
            // 无修饰字面量转换为字符串
            write("\"" + escapeString(std::get<std::string>(node->getValue())) + "\"");
            break;
    }
}

void Generator::visitModuleNode(ModuleNode* node) {
    // 生成AMD风格的模块加载器
    pushState();
    currentState_.inModule = true;
    
    moduleLoader_ << "// CHTL JS Module Loader\n";
    moduleLoader_ << "var CHTLModuleLoader = (function() {\n";
    moduleLoader_ << "  var modules = {};\n";
    moduleLoader_ << "  var loaded = {};\n";
    moduleLoader_ << "  \n";
    moduleLoader_ << "  function loadModule(path, callback) {\n";
    moduleLoader_ << "    if (loaded[path]) {\n";
    moduleLoader_ << "      callback();\n";
    moduleLoader_ << "      return;\n";
    moduleLoader_ << "    }\n";
    moduleLoader_ << "    \n";
    moduleLoader_ << "    var script = document.createElement('script');\n";
    moduleLoader_ << "    script.src = path;\n";
    moduleLoader_ << "    script.onload = function() {\n";
    moduleLoader_ << "      loaded[path] = true;\n";
    moduleLoader_ << "      callback();\n";
    moduleLoader_ << "    };\n";
    moduleLoader_ << "    document.head.appendChild(script);\n";
    moduleLoader_ << "  }\n";
    moduleLoader_ << "  \n";
    moduleLoader_ << "  return { load: loadModule };\n";
    moduleLoader_ << "})();\n\n";
    
    // 生成模块加载调用
    writeLine("// Load modules");
    for (const auto& module : node->getLoadItems()) {
        writeLine("CHTLModuleLoader.load('" + module + "', function() {});");
    }
    writeLine();
    
    popState();
}

void Generator::visitEnhancedSelectorNode(EnhancedSelectorNode* node) {
    pushState();
    currentState_.inSelector = true;
    
    std::string selectorCode = generateSelectorCode(node);
    write(selectorCode);
    
    popState();
}

std::string Generator::generateSelectorCode(EnhancedSelectorNode* node) {
    std::string selector = node->getSelector();
    std::string funcName = getSelectorFunction(node->getSelectorType());
    
    // 检查缓存
    auto cacheKey = selector + (node->getIndex().has_value() ? 
                                "[" + std::to_string(node->getIndex().value()) + "]" : "");
    auto it = currentState_.selectorCache.find(cacheKey);
    if (it != currentState_.selectorCache.end()) {
        return it->second;
    }
    
    std::string code;
    
    // 处理引用选择器
    if (node->getSelectorType() == EnhancedSelectorNode::SelectorType::REFERENCE) {
        // & 引用选择器需要特殊处理
        auto selectorCtx = context_->getCurrentSelectorContext();
        if (selectorCtx) {
            selector = selectorCtx->selector;
        }
    }
    
    // 生成选择器代码
    switch (node->getSelectorType()) {
        case EnhancedSelectorNode::SelectorType::CLASS:
            code = funcName + "('" + selector + "')";
            break;
        case EnhancedSelectorNode::SelectorType::ID:
            code = funcName + "('" + selector + "')";
            break;
        case EnhancedSelectorNode::SelectorType::TAG:
            code = funcName + "('" + selector + "')";
            break;
        case EnhancedSelectorNode::SelectorType::COMPOUND:
            code = "CHTLSelector.query('" + selector + "')";
            break;
        case EnhancedSelectorNode::SelectorType::REFERENCE:
            code = "CHTLSelector.current()";
            break;
    }
    
    // 处理索引访问
    if (node->getIndex().has_value()) {
        code += "[" + std::to_string(node->getIndex().value()) + "]";
    }
    
    // 缓存结果
    currentState_.selectorCache[cacheKey] = code;
    
    return code;
}

std::string Generator::getSelectorFunction(EnhancedSelectorNode::SelectorType type) {
    switch (type) {
        case EnhancedSelectorNode::SelectorType::CLASS:
            return "CHTLSelector.byClass";
        case EnhancedSelectorNode::SelectorType::ID:
            return "CHTLSelector.byId";
        case EnhancedSelectorNode::SelectorType::TAG:
            return "CHTLSelector.byTag";
        default:
            return "CHTLSelector.query";
    }
}

void Generator::visitListenNode(ListenNode* node) {
    // listen块生成为事件监听器对象
    write("{");
    indent();
    writeLine();
    
    bool first = true;
    for (const auto& [event, handler] : node->getEventHandlers()) {
        if (!first) {
            writeLine(",");
        }
        write(getIndent() + event + ": ");
        handler->accept(this);
        first = false;
    }
    
    writeLine();
    dedent();
    write(getIndent() + "}");
}

void Generator::visitArrowAccessNode(ArrowAccessNode* node) {
    node->getObject()->accept(this);
    write(".");
    node->getProperty()->accept(this);
}

void Generator::visitEventBindingNode(EventBindingNode* node) {
    // &-> 事件绑定
    node->getSelector()->accept(this);
    write(".addEventListener('");
    write(node->getEvent());
    write("', ");
    node->getHandler()->accept(this);
    write(")");
}

void Generator::visitBinaryExpressionNode(BinaryExpressionNode* node) {
    bool needParens = node->getOperator() != BinaryExpressionNode::Operator::DOT;
    
    if (needParens) write("(");
    node->getLeft()->accept(this);
    generateBinaryOperator(node->getOperator());
    node->getRight()->accept(this);
    if (needParens) write(")");
}

void Generator::generateBinaryOperator(BinaryExpressionNode::Operator op) {
    switch (op) {
        case BinaryExpressionNode::Operator::ADD: write(" + "); break;
        case BinaryExpressionNode::Operator::SUBTRACT: write(" - "); break;
        case BinaryExpressionNode::Operator::MULTIPLY: write(" * "); break;
        case BinaryExpressionNode::Operator::DIVIDE: write(" / "); break;
        case BinaryExpressionNode::Operator::MODULO: write(" % "); break;
        case BinaryExpressionNode::Operator::EQUAL: write(" === "); break;
        case BinaryExpressionNode::Operator::NOT_EQUAL: write(" !== "); break;
        case BinaryExpressionNode::Operator::LESS_THAN: write(" < "); break;
        case BinaryExpressionNode::Operator::GREATER_THAN: write(" > "); break;
        case BinaryExpressionNode::Operator::LESS_EQUAL: write(" <= "); break;
        case BinaryExpressionNode::Operator::GREATER_EQUAL: write(" >= "); break;
        case BinaryExpressionNode::Operator::AND: write(" && "); break;
        case BinaryExpressionNode::Operator::OR: write(" || "); break;
        case BinaryExpressionNode::Operator::DOT: write("."); break;
    }
}

void Generator::visitVariableDeclarationNode(VariableDeclarationNode* node) {
    switch (node->getDeclarationType()) {
        case VariableDeclarationNode::DeclarationType::CONST:
            write("const ");
            break;
        case VariableDeclarationNode::DeclarationType::LET:
            write("let ");
            break;
        case VariableDeclarationNode::DeclarationType::VAR:
            write("var ");
            break;
    }
    
    write(node->getName());
    
    if (node->getInitializer()) {
        write(" = ");
        node->getInitializer()->accept(this);
    }
}

void Generator::visitObjectLiteralNode(ObjectLiteralNode* node) {
    write("{");
    
    const auto& properties = node->getProperties();
    if (!properties.empty() && !config_.minify) {
        writeLine();
        indent();
    }
    
    bool first = true;
    for (const auto& [key, value] : properties) {
        if (!first) {
            write(",");
            if (!config_.minify) writeLine();
        }
        
        if (!config_.minify) write(getIndent());
        write(key + ": ");
        value->accept(this);
        
        first = false;
    }
    
    if (!properties.empty() && !config_.minify) {
        writeLine();
        dedent();
        write(getIndent());
    }
    
    write("}");
}

void Generator::generateSelectorHelpers() {
    writeLine("// CHTL JS Selector Helpers");
    writeLine("var CHTLSelector = {");
    indent();
    
    writeLine("byClass: function(className) {");
    indent();
    writeLine("return document.getElementsByClassName(className.substring(1));");
    dedent();
    writeLine("},");
    
    writeLine("byId: function(id) {");
    indent();
    writeLine("return document.getElementById(id.substring(1));");
    dedent();
    writeLine("},");
    
    writeLine("byTag: function(tag) {");
    indent();
    writeLine("return document.getElementsByTagName(tag);");
    dedent();
    writeLine("},");
    
    writeLine("query: function(selector) {");
    indent();
    writeLine("return document.querySelectorAll(selector);");
    dedent();
    writeLine("},");
    
    writeLine("current: function() {");
    indent();
    writeLine("return this._currentElement || document.body;");
    dedent();
    writeLine("}");
    
    dedent();
    writeLine("};");
    writeLine();
}

void Generator::generateEventDelegationSystem() {
    writeLine("// CHTL JS Event Delegation System");
    writeLine("var CHTLEventDelegation = (function() {");
    indent();
    
    writeLine("var delegations = new Map();");
    writeLine();
    
    writeLine("function delegate(parent, target, event, handler) {");
    indent();
    writeLine("var key = parent + '_' + event;");
    writeLine("if (!delegations.has(key)) {");
    indent();
    writeLine("var parentEl = typeof parent === 'string' ? ");
    writeLine("  document.querySelector(parent) : parent;");
    writeLine("parentEl.addEventListener(event, function(e) {");
    indent();
    writeLine("var targetEl = e.target.closest(target);");
    writeLine("if (targetEl) {");
    indent();
    writeLine("var handlers = delegations.get(key);");
    writeLine("handlers.forEach(function(h) {");
    indent();
    writeLine("if (h.target === target) h.handler.call(targetEl, e);");
    dedent();
    writeLine("});");
    dedent();
    writeLine("}");
    dedent();
    writeLine("});");
    writeLine("delegations.set(key, []);");
    dedent();
    writeLine("}");
    writeLine("delegations.get(key).push({target: target, handler: handler});");
    dedent();
    writeLine("}");
    writeLine();
    
    writeLine("return { delegate: delegate };");
    dedent();
    writeLine("})();");
    writeLine();
}

void Generator::generateAnimationHelpers() {
    writeLine("// CHTL JS Animation Helpers");
    writeLine("var CHTLAnimation = (function() {");
    indent();
    
    writeLine("function animate(options) {");
    indent();
    writeLine("// Animation implementation");
    writeLine("var start = performance.now();");
    writeLine("var duration = options.duration || 1000;");
    writeLine("var target = options.target;");
    writeLine("var easing = options.easing || 'linear';");
    writeLine();
    writeLine("function step(timestamp) {");
    indent();
    writeLine("var progress = (timestamp - start) / duration;");
    writeLine("if (progress >= 1) progress = 1;");
    writeLine("// Apply animation");
    writeLine("if (progress < 1) requestAnimationFrame(step);");
    dedent();
    writeLine("}");
    writeLine();
    writeLine("requestAnimationFrame(step);");
    dedent();
    writeLine("}");
    writeLine();
    
    writeLine("return { animate: animate };");
    dedent();
    writeLine("})();");
    writeLine();
}

void Generator::generateVirtualObjectSystem() {
    writeLine("// CHTL JS Virtual Object System");
    writeLine("var CHTLVirtualObjects = {};");
    writeLine();
}

std::string Generator::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

void Generator::write(const std::string& text) {
    output_ << text;
}

void Generator::writeLine(const std::string& text) {
    if (!config_.minify) {
        output_ << getIndent();
    }
    output_ << text;
    if (!config_.minify) {
        output_ << config_.lineEnding;
    }
}

void Generator::indent() {
    indentLevel_++;
}

void Generator::dedent() {
    if (indentLevel_ > 0) {
        indentLevel_--;
    }
}

std::string Generator::getIndent() const {
    if (config_.minify) {
        return "";
    }
    return std::string(indentLevel_ * config_.indentSize, ' ');
}

void Generator::pushState() {
    stateStack_.push(currentState_);
    currentState_ = GeneratorState();
}

void Generator::popState() {
    if (!stateStack_.empty()) {
        currentState_ = stateStack_.top();
        stateStack_.pop();
    }
}

// 其他访问者方法的空实现
void Generator::visitDelegateNode(DelegateNode* node) {}
void Generator::visitAnimateNode(AnimateNode* node) {}
void Generator::visitAnimateStateNode(AnimateStateNode* node) {}
void Generator::visitVirtualObjectNode(VirtualObjectNode* node) {}
void Generator::visitINeverAwayNode(INeverAwayNode* node) {}
void Generator::visitUnaryExpressionNode(UnaryExpressionNode* node) {}
void Generator::visitFunctionDeclarationNode(FunctionDeclarationNode* node) {}
void Generator::visitArrayLiteralNode(ArrayLiteralNode* node) {}
void Generator::visitCallExpressionNode(CallExpressionNode* node) {}

} // namespace CHTLJS