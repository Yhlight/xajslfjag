#ifndef CHTL_BASIC_NODES_H
#define CHTL_BASIC_NODES_H

#include "Node.h"

namespace CHTL {

// 程序节点 - AST的根节点
class ProgramNode : public Node {
private:
    std::string fileName;
    std::string useStatement;  // use语句（如 "html5" 或 "@Config Basic"）
    
public:
    ProgramNode(const std::string& fileName);
    
    void setUseStatement(const std::string& stmt) { useStatement = stmt; }
    std::string getUseStatement() const { return useStatement; }
    std::string getFileName() const { return fileName; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// 注释节点
class CommentNode : public Node {
public:
    enum CommentType {
        LINE_COMMENT,        // //
        BLOCK_COMMENT,       // /* */
        GENERATOR_COMMENT    // --
    };
    
private:
    CommentType commentType;
    std::string content;
    
public:
    CommentNode(CommentType type, const std::string& content, const Token& token);
    
    CommentType getCommentType() const { return commentType; }
    std::string getContent() const { return content; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// 文本节点
class TextNode : public Node {
private:
    std::string content;
    bool isLiteral;  // 是否为字面量（无引号）
    
public:
    TextNode(const Token& token);
    
    void setContent(const std::string& text) { content = text; }
    std::string getContent() const { return content; }
    void setIsLiteral(bool literal) { isLiteral = literal; }
    bool getIsLiteral() const { return isLiteral; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// 字面量节点
class LiteralNode : public Node {
public:
    enum LiteralType {
        UNQUOTED,      // 无引号字面量
        DOUBLE_QUOTED, // 双引号字符串
        SINGLE_QUOTED  // 单引号字符串
    };
    
private:
    LiteralType literalType;
    std::string value;
    
public:
    LiteralNode(LiteralType type, const std::string& value, const Token& token);
    
    LiteralType getLiteralType() const { return literalType; }
    std::string getValue() const { return value; }
    std::string getUnquotedValue() const;  // 获取去除引号的值
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// 标识符节点
class IdentifierNode : public Node {
private:
    std::string name;
    
public:
    IdentifierNode(const std::string& name, const Token& token);
    
    std::string getName() const { return name; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// 块节点
class BlockNode : public Node {
private:
    bool hasBraces;  // 是否有花括号
    
public:
    BlockNode(const Token& token);
    
    void setHasBraces(bool braces) { hasBraces = braces; }
    bool getHasBraces() const { return hasBraces; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// 语句列表节点
class StatementListNode : public Node {
public:
    StatementListNode(const Token& token);
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

} // namespace CHTL

#endif // CHTL_BASIC_NODES_H