#include "CHTLParser.h"
#include "CHTLNode/ImportNode.h"
#include "CHTLNode/NamespaceNode.h"
#include "CHTLNode/OriginNode.h"
#include "CHTLNode/OperatorNode.h"

namespace CHTL {

// 导入解析实现
NodePtr CHTLParser::ParseImport() {
    Consume(TokenType::IMPORT, "期望 '[Import]'");
    
    auto importNode = std::make_shared<ImportNode>();
    importNode->SetPosition(previousToken.line, previousToken.column);
    
    // 检查导入类型
    if (Match(TokenType::TYPE_HTML)) {
        importNode->SetTargetType(ImportTargetType::HTML);
    } else if (Match(TokenType::TYPE_STYLE)) {
        importNode->SetTargetType(ImportTargetType::STYLE);
    } else if (Match(TokenType::TYPE_JAVASCRIPT)) {
        importNode->SetTargetType(ImportTargetType::JAVASCRIPT);
    } else if (Match(TokenType::TYPE_CHTL)) {
        importNode->SetTargetType(ImportTargetType::CHTL);
    } else if (Match(TokenType::TYPE_CJMOD)) {
        importNode->SetTargetType(ImportTargetType::CJMOD);
    } else if (Match(TokenType::TYPE_CONFIG)) {
        importNode->SetTargetType(ImportTargetType::CONFIG);
        
        // 可能有配置名称
        if (Check(TokenType::IDENTIFIER)) {
            importNode->SetItemName(currentToken.value);
            Advance();
        }
    } else if (Match(TokenType::CUSTOM)) {
        importNode->SetTargetType(ImportTargetType::CUSTOM);
        
        // 必须有类型和可能的名称
        if (Match(TokenType::TYPE_STYLE)) {
            importNode->SetItemType("@Style");
        } else if (Match(TokenType::TYPE_ELEMENT)) {
            importNode->SetItemType("@Element");
        } else if (Match(TokenType::TYPE_VAR)) {
            importNode->SetItemType("@Var");
        } else {
            throw ParseError("期望自定义类型", currentToken.line, currentToken.column);
        }
        
        // 可能有具体名称
        if (Check(TokenType::IDENTIFIER)) {
            importNode->SetItemName(currentToken.value);
            Advance();
        }
    } else if (Match(TokenType::TEMPLATE)) {
        importNode->SetTargetType(ImportTargetType::TEMPLATE);
        
        // 可能有类型和名称
        if (Match(TokenType::TYPE_STYLE)) {
            importNode->SetItemType("@Style");
        } else if (Match(TokenType::TYPE_ELEMENT)) {
            importNode->SetItemType("@Element");
        } else if (Match(TokenType::TYPE_VAR)) {
            importNode->SetItemType("@Var");
        }
        
        if (Check(TokenType::IDENTIFIER)) {
            importNode->SetItemName(currentToken.value);
            Advance();
        }
    } else if (Match(TokenType::ORIGIN)) {
        importNode->SetTargetType(ImportTargetType::ORIGIN);
        
        // 必须有类型和可能的名称
        if (Check(TokenType::AT)) {
            Advance();
            if (Check(TokenType::IDENTIFIER)) {
                importNode->SetItemType("@" + currentToken.value);
                Advance();
            }
        }
        
        if (Check(TokenType::IDENTIFIER)) {
            importNode->SetItemName(currentToken.value);
            Advance();
        }
    } else if (Match(TokenType::CONFIGURATION)) {
        // [Import] [Configuration] @Config ...
        importNode->SetTargetType(ImportTargetType::CONFIG);
        Consume(TokenType::TYPE_CONFIG, "期望 '@Config'");
        
        if (Check(TokenType::IDENTIFIER)) {
            importNode->SetItemName(currentToken.value);
            Advance();
        }
    } else {
        throw ParseError("期望导入类型", currentToken.line, currentToken.column);
    }
    
    // 解析 from 路径
    Consume(TokenType::FROM, "期望 'from'");
    
    if (Check(TokenType::STRING_LITERAL) || Check(TokenType::UNQUOTED_LITERAL) ||
        Check(TokenType::IDENTIFIER)) {
        importNode->SetFromPath(currentToken.value);
        Advance();
    } else {
        throw ParseError("期望导入路径", currentToken.line, currentToken.column);
    }
    
    // 可选的 as 重命名
    if (Match(TokenType::AS)) {
        if (Check(TokenType::IDENTIFIER)) {
            importNode->SetAsName(currentToken.value);
            Advance();
        } else {
            throw ParseError("期望重命名标识符", currentToken.line, currentToken.column);
        }
    }
    
    // 分号可选
    if (Check(TokenType::SEMICOLON)) {
        Advance();
    }
    
    // 验证导入节点
    if (!importNode->IsValid()) {
        if (importNode->RequiresAsName() && importNode->GetAsName().empty()) {
            throw ParseError("此类型的导入需要 'as' 名称", 
                           previousToken.line, previousToken.column);
        }
    }
    
    // 将导入路径添加到全局映射
    globalMap->AddImportedFile(importNode->GetFromPath());
    
    return importNode;
}

// 命名空间解析实现
NodePtr CHTLParser::ParseNamespace() {
    Consume(TokenType::NAMESPACE, "期望 '[Namespace]'");
    
    std::string namespaceName;
    if (Check(TokenType::IDENTIFIER)) {
        namespaceName = currentToken.value;
        Advance();
    } else {
        throw ParseError("期望命名空间名称", currentToken.line, currentToken.column);
    }
    
    auto namespaceNode = std::make_shared<NamespaceNode>(namespaceName);
    namespaceNode->SetPosition(previousToken.line, previousToken.column);
    
    // 设置父命名空间
    std::string currentNs = globalMap->GetCurrentNamespace();
    if (!currentNs.empty()) {
        namespaceNode->SetParentNamespace(currentNs);
    }
    
    // 可能没有花括号（单层嵌套时）
    bool hasBrace = false;
    if (Check(TokenType::LEFT_BRACE)) {
        hasBrace = true;
        Advance();
    }
    
    // 进入命名空间
    globalMap->EnterNamespace(namespaceName);
    
    // 解析命名空间内容
    if (hasBrace) {
        while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
            // 检查是否是except约束
            if (Check(TokenType::EXCEPT)) {
                auto exceptNode = ParseExcept();
                if (exceptNode) {
                    auto op = std::dynamic_pointer_cast<OperatorNode>(exceptNode);
                    if (op) {
                        // 将约束添加到命名空间
                        for (const auto& item : op->GetExceptItems()) {
                            namespaceNode->AddExceptItem(item);
                        }
                        for (const auto& type : op->GetExceptTypes()) {
                            namespaceNode->AddExceptType(type);
                        }
                    }
                }
                continue;
            }
            
            // 解析其他顶层元素
            auto node = ParseTopLevel();
            if (node) {
                namespaceNode->AddChild(node);
            }
        }
        
        Consume(TokenType::RIGHT_BRACE, "期望 '}'");
    } else {
        // 没有花括号，只解析下一个元素
        if (Check(TokenType::NAMESPACE)) {
            // 嵌套命名空间
            auto nestedNs = ParseNamespace();
            if (nestedNs) {
                namespaceNode->AddChild(nestedNs);
            }
        } else {
            // 其他元素
            auto node = ParseTopLevel();
            if (node) {
                namespaceNode->AddChild(node);
            }
        }
    }
    
    // 退出命名空间
    globalMap->ExitNamespace();
    
    // 添加到全局映射
    globalMap->AddNamespace(namespaceNode->GetFullName(), namespaceNode);
    
    return namespaceNode;
}

// 原始嵌入解析实现
NodePtr CHTLParser::ParseOrigin() {
    Consume(TokenType::ORIGIN, "期望 '[Origin]'");
    
    std::string originType;
    std::string originName;
    
    // 解析类型
    if (Match(TokenType::TYPE_HTML)) {
        originType = "@Html";
    } else if (Match(TokenType::TYPE_STYLE)) {
        originType = "@Style";
    } else if (Match(TokenType::TYPE_JAVASCRIPT)) {
        originType = "@JavaScript";
    } else if (Match(TokenType::AT)) {
        // 自定义类型
        if (Check(TokenType::IDENTIFIER)) {
            originType = "@" + currentToken.value;
            Advance();
        } else {
            throw ParseError("期望原始嵌入类型名称", currentToken.line, currentToken.column);
        }
    } else {
        throw ParseError("期望原始嵌入类型", currentToken.line, currentToken.column);
    }
    
    // 可能有名称
    if (Check(TokenType::IDENTIFIER)) {
        originName = currentToken.value;
        Advance();
    }
    
    auto originNode = std::make_shared<OriginNode>(originType, originName);
    originNode->SetPosition(previousToken.line, previousToken.column);
    
    Consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 收集原始内容直到匹配的右大括号
    std::string content;
    int braceCount = 1;
    
    // 保存当前位置用于原始内容提取
    int startPos = lexer->GetPosition();
    
    while (braceCount > 0 && !Check(TokenType::EOF_TOKEN)) {
        Token token = lexer->NextToken();  // 直接获取Token，包括空白
        
        if (token.type == TokenType::LEFT_BRACE) {
            braceCount++;
        } else if (token.type == TokenType::RIGHT_BRACE) {
            braceCount--;
            if (braceCount == 0) {
                // 不包含最后的右大括号
                break;
            }
        }
        
        content += token.value;
    }
    
    // 更新当前Token
    currentToken = lexer->NextToken();
    while (currentToken.type == TokenType::WHITESPACE ||
           currentToken.type == TokenType::SINGLE_COMMENT ||
           currentToken.type == TokenType::MULTI_COMMENT) {
        currentToken = lexer->NextToken();
    }
    
    originNode->SetContent(content);
    
    // 如果有名称，添加到全局映射
    if (!originName.empty()) {
        globalMap->AddOrigin(originType, originName, originNode);
    }
    
    return originNode;
}

// 操作符解析实现
NodePtr CHTLParser::ParseDelete() {
    Consume(TokenType::DELETE, "期望 'delete'");
    
    auto deleteNode = std::make_shared<OperatorNode>(OperatorType::DELETE);
    deleteNode->SetPosition(previousToken.line, previousToken.column);
    
    // 解析删除目标
    do {
        if (Check(TokenType::IDENTIFIER)) {
            deleteNode->AddDeleteTarget(currentToken.value);
            Advance();
        } else if (Check(TokenType::AT)) {
            // 删除模板/自定义继承
            Advance();
            std::string target = "@";
            if (Check(TokenType::IDENTIFIER)) {
                target += currentToken.value;
                Advance();
            }
            deleteNode->AddDeleteTarget(target);
        } else if (Check(TokenType::TEMPLATE) || Check(TokenType::CUSTOM)) {
            // 删除[Template]或[Custom]继承
            std::string prefix = currentToken.value;
            Advance();
            
            if (Check(TokenType::AT)) {
                Advance();
                if (Check(TokenType::IDENTIFIER)) {
                    std::string type = "@" + currentToken.value;
                    Advance();
                    
                    if (Check(TokenType::IDENTIFIER)) {
                        deleteNode->AddDeleteTarget(prefix + " " + type + " " + currentToken.value);
                        Advance();
                    }
                }
            }
        } else {
            throw ParseError("期望删除目标", currentToken.line, currentToken.column);
        }
    } while (Match(TokenType::COMMA));
    
    Consume(TokenType::SEMICOLON, "期望 ';'");
    
    return deleteNode;
}

NodePtr CHTLParser::ParseInsert() {
    Consume(TokenType::INSERT, "期望 'insert'");
    
    auto insertNode = std::make_shared<OperatorNode>(OperatorType::INSERT);
    insertNode->SetPosition(previousToken.line, previousToken.column);
    
    // 解析插入位置
    if (Match(TokenType::AFTER)) {
        insertNode->SetInsertPosition(InsertPosition::AFTER);
    } else if (Match(TokenType::BEFORE)) {
        insertNode->SetInsertPosition(InsertPosition::BEFORE);
    } else if (Match(TokenType::REPLACE)) {
        insertNode->SetInsertPosition(InsertPosition::REPLACE);
    } else if (Match(TokenType::AT_TOP)) {
        insertNode->SetInsertPosition(InsertPosition::AT_TOP);
    } else if (Match(TokenType::AT_BOTTOM)) {
        insertNode->SetInsertPosition(InsertPosition::AT_BOTTOM);
    } else {
        throw ParseError("期望插入位置", currentToken.line, currentToken.column);
    }
    
    // 解析选择器（除了at top/bottom）
    if (insertNode->GetInsertPosition() != InsertPosition::AT_TOP &&
        insertNode->GetInsertPosition() != InsertPosition::AT_BOTTOM) {
        
        std::string selector;
        if (Check(TokenType::IDENTIFIER) || Check(TokenType::HTML_TAG)) {
            selector = currentToken.value;
            Advance();
            
            // 可能有索引
            if (Match(TokenType::LEFT_BRACKET)) {
                selector += "[";
                if (Check(TokenType::NUMBER)) {
                    selector += currentToken.value;
                    Advance();
                }
                Consume(TokenType::RIGHT_BRACKET, "期望 ']'");
                selector += "]";
            }
        }
        
        insertNode->SetInsertSelector(selector);
    }
    
    // 解析插入内容
    Consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    auto contentNode = std::make_shared<BaseNode>(NodeType::ELEMENT);
    
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::HTML_TAG) || Check(TokenType::IDENTIFIER)) {
            auto element = ParseElement();
            if (element) {
                contentNode->AddChild(element);
            }
        } else if (Check(TokenType::AT)) {
            // 使用模板/自定义
            Advance();
            // TODO: 解析模板/自定义使用
        } else {
            throw ParseError("插入内容中的意外Token", currentToken.line, currentToken.column);
        }
    }
    
    Consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    insertNode->SetInsertContent(contentNode);
    
    return insertNode;
}

NodePtr CHTLParser::ParseExcept() {
    Consume(TokenType::EXCEPT, "期望 'except'");
    
    auto exceptNode = std::make_shared<OperatorNode>(OperatorType::EXCEPT);
    exceptNode->SetPosition(previousToken.line, previousToken.column);
    
    // 解析排除项
    do {
        if (Check(TokenType::IDENTIFIER) || Check(TokenType::HTML_TAG)) {
            // 具体项
            exceptNode->AddExceptItem(currentToken.value);
            Advance();
        } else if (Check(TokenType::AT)) {
            // 类型排除 @Html等
            Advance();
            if (Check(TokenType::IDENTIFIER)) {
                exceptNode->AddExceptType("@" + currentToken.value);
                Advance();
            }
        } else if (Check(TokenType::TEMPLATE) || Check(TokenType::CUSTOM)) {
            // [Template]或[Custom]类型排除
            std::string type = "[" + currentToken.value + "]";
            Advance();
            
            // 可能有具体类型
            if (Check(TokenType::AT)) {
                Advance();
                if (Check(TokenType::IDENTIFIER)) {
                    type += " @" + currentToken.value;
                    Advance();
                }
            }
            
            exceptNode->AddExceptType(type);
        } else {
            throw ParseError("期望排除目标", currentToken.line, currentToken.column);
        }
    } while (Match(TokenType::COMMA));
    
    Consume(TokenType::SEMICOLON, "期望 ';'");
    
    return exceptNode;
}

NodePtr CHTLParser::ParseInherit() {
    Consume(TokenType::INHERIT, "期望 'inherit'");
    
    auto inheritNode = std::make_shared<OperatorNode>(OperatorType::INHERIT);
    inheritNode->SetPosition(previousToken.line, previousToken.column);
    
    // 解析继承目标
    if (Check(TokenType::AT)) {
        Advance();
        if (Check(TokenType::IDENTIFIER)) {
            std::string type = "@" + currentToken.value;
            Advance();
            
            if (Check(TokenType::IDENTIFIER)) {
                std::string name = currentToken.value;
                Advance();
                inheritNode->AddInheritTarget(type, name);
            }
        }
    } else {
        throw ParseError("期望继承目标", currentToken.line, currentToken.column);
    }
    
    Consume(TokenType::SEMICOLON, "期望 ';'");
    
    return inheritNode;
}

} // namespace CHTL