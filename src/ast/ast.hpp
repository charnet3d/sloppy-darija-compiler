#ifndef AST_HPP
#define AST_HPP

#include "../lexer/token.hpp"
#include <memory>
#include <vector>
#include <string>
#include <variant>

namespace darija {

// Forward declarations for all node types
class ASTNode;
class Visitor;

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    
    // Accept visitor (for visitor pattern)
    virtual void accept(Visitor& visitor) = 0;
    
    // Get source location
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    size_t getStartPos() const { return start_pos_; }
    size_t getEndPos() const { return end_pos_; }
    
protected:
    size_t line_;
    size_t column_;
    size_t start_pos_;
    size_t end_pos_;
    
    ASTNode(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : line_(line), column_(column), start_pos_(start_pos), end_pos_(end_pos) {}
};

// Expression base class
struct Expression : public ASTNode {
    Expression(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : ASTNode(line, column, start_pos, end_pos) {}
    
    virtual TokenType getType() const = 0;
};

// Statement base class
struct Statement : public ASTNode {
    Statement(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : ASTNode(line, column, start_pos, end_pos) {}
};

// All node type declarations
struct ProgramNode;
struct ImportNode;
struct FunctionNode;
struct ClassNode;
struct FieldNode;
struct MethodNode;
struct VariableNode;
struct LiteralExpression;
struct IdentifierExpression;
struct BinaryExpression;
struct UnaryExpression;
struct CallExpression;
struct MemberExpression;
struct CastExpression;
struct ArrayAccessExpression;
struct NewExpression;
struct ExpressionStatement;
struct ReturnStatement;
struct IfStatement;
struct ForStatement;
struct ForEachStatement;
struct WhileStatement;
struct DoWhileStatement;
struct TryStatement;
struct BlockStatement;
struct PrintStatement;
struct ContinueStatement;
struct BreakStatement;
struct ThrowStatement;

// Visitor base class
class Visitor {
public:
    virtual ~Visitor() = default;
    
    // Visit methods for each node type
    virtual void visit(ProgramNode&) {};
    virtual void visit(ImportNode&) {};
    virtual void visit(FunctionNode&) {};
    virtual void visit(ClassNode&) {};
    virtual void visit(FieldNode&) {};
    virtual void visit(MethodNode&) {};
    virtual void visit(VariableNode&) {};
    virtual void visit(LiteralExpression&) {};
    virtual void visit(IdentifierExpression&) {};
    virtual void visit(BinaryExpression&) {};
    virtual void visit(UnaryExpression&) {};
    virtual void visit(CallExpression&) {};
    virtual void visit(MemberExpression&) {};
    virtual void visit(CastExpression&) {};
    virtual void visit(ArrayAccessExpression&) {};
    virtual void visit(NewExpression&) {};
    virtual void visit(ExpressionStatement&) {};
    virtual void visit(ReturnStatement&) {};
    virtual void visit(IfStatement&) {};
    virtual void visit(ForStatement&) {};
    virtual void visit(ForEachStatement&) {};
    virtual void visit(WhileStatement&) {};
    virtual void visit(DoWhileStatement&) {};
    virtual void visit(TryStatement&) {};
    virtual void visit(BlockStatement&) {};
    virtual void visit(PrintStatement&) {};
    virtual void visit(ContinueStatement&) {};
    virtual void visit(BreakStatement&) {};
    virtual void visit(ThrowStatement&) {};
    
    // Generic visit method
    virtual void visit(ASTNode&) {};
};

// Program node
struct ProgramNode : public ASTNode {
    std::string module_name;
    std::vector<std::unique_ptr<ASTNode>> imports;
    std::vector<std::unique_ptr<ASTNode>> type_declarations;
    std::vector<std::unique_ptr<ASTNode>> function_declarations;
    std::unique_ptr<ASTNode> main_function;
    
    ProgramNode(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : ASTNode(line, column, start_pos, end_pos) {}
    
    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};

// Import statement node
struct ImportNode : public ASTNode {
    std::string module_name;
    std::string path;
    
    ImportNode(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : ASTNode(line, column, start_pos, end_pos) {}
    
    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};

// Function declaration node
struct FunctionNode : public ASTNode {
    std::string name;
    std::vector<std::pair<std::string, TokenType>> parameters;
    TokenType return_type;
    std::vector<std::unique_ptr<ASTNode>> body;
    bool is_main;
    
    FunctionNode(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : ASTNode(line, column, start_pos, end_pos), is_main(false) {}
    
    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};

// Class declaration node
struct ClassNode : public ASTNode {
    std::string name;
    std::string parent_class;
    std::vector<std::unique_ptr<ASTNode>> members;
    
    ClassNode(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : ASTNode(line, column, start_pos, end_pos) {}
    
    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};

// Field declaration node
struct FieldNode : public ASTNode {
    TokenType access_modifier;
    TokenType type;
    std::string name;
    std::unique_ptr<ASTNode> initializer;
    
    FieldNode(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : ASTNode(line, column, start_pos, end_pos), access_modifier(TokenType::KW_PUBLIC) {}
    
    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};

// Method declaration node
struct MethodNode : public ASTNode {
    TokenType access_modifier;
    std::string name;
    std::vector<std::pair<std::string, TokenType>> parameters;
    TokenType return_type;
    std::vector<std::unique_ptr<ASTNode>> body;
    bool is_constructor;
    
    MethodNode(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : ASTNode(line, column, start_pos, end_pos), is_constructor(false) {}
    
    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};

// Variable declaration node
struct VariableNode : public Statement {
    TokenType type;
    std::string name;
    std::unique_ptr<Expression> initializer;
    bool is_constant;
    
    VariableNode(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : Statement(line, column, start_pos, end_pos), is_constant(false) {}
    
    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};


// Literal expression
struct LiteralExpression : public Expression {
    TokenType literal_type;
    std::variant<int64_t, double, std::string, char, bool> value;
    
    LiteralExpression(size_t line, size_t column, size_t start_pos, size_t end_pos,
                     TokenType type, std::variant<int64_t, double, std::string, char, bool> val)
        : Expression(line, column, start_pos, end_pos), literal_type(type), value(std::move(val)) {}
    
    TokenType getType() const override { return literal_type; }
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Identifier expression
struct IdentifierExpression : public Expression {
    std::string name;
    
    IdentifierExpression(size_t line, size_t column, size_t start_pos, size_t end_pos,
                        const std::string& ident)
        : Expression(line, column, start_pos, end_pos), name(ident) {}
    
    TokenType getType() const override { return TokenType::LITERAL_IDENTIFIER; }
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Binary expression
struct BinaryExpression : public Expression {
    TokenType operator_type;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    
    BinaryExpression(size_t line, size_t column, size_t start_pos, size_t end_pos,
                    TokenType op, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : Expression(line, column, start_pos, end_pos), operator_type(op),
          left(std::move(l)), right(std::move(r)) {}
    
    TokenType getType() const override { return TokenType::TYPE_OBJECT; }
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Unary expression
struct UnaryExpression : public Expression {
    TokenType operator_type;
    std::unique_ptr<Expression> operand;
    
    UnaryExpression(size_t line, size_t column, size_t start_pos, size_t end_pos,
                   TokenType op, std::unique_ptr<Expression> opnd)
        : Expression(line, column, start_pos, end_pos), operator_type(op), operand(std::move(opnd)) {}
    
    TokenType getType() const override { return TokenType::TYPE_OBJECT; }
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Function call expression
struct CallExpression : public Expression {
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> arguments;
    
    CallExpression(size_t line, size_t column, size_t start_pos, size_t end_pos,
                  std::unique_ptr<Expression> call,
                  std::vector<std::unique_ptr<Expression>> args)
        : Expression(line, column, start_pos, end_pos),
          callee(std::move(call)), arguments(std::move(args)) {}
    
    TokenType getType() const override { return TokenType::TYPE_OBJECT; }
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Member access expression
struct MemberExpression : public Expression {
    std::unique_ptr<Expression> object;
    std::string member;
    
    MemberExpression(size_t line, size_t column, size_t start_pos, size_t end_pos,
                    std::unique_ptr<Expression> obj, const std::string& mem)
        : Expression(line, column, start_pos, end_pos),
          object(std::move(obj)), member(mem) {}
    
    TokenType getType() const override { return TokenType::TYPE_OBJECT; }
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Type cast expression
struct CastExpression : public Expression {
    TokenType target_type;
    std::unique_ptr<Expression> expression;
    
    CastExpression(size_t line, size_t column, size_t start_pos, size_t end_pos,
                  TokenType type, std::unique_ptr<Expression> expr)
        : Expression(line, column, start_pos, end_pos), target_type(type), expression(std::move(expr)) {}
    
    TokenType getType() const override { return target_type; }
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Array access expression
struct ArrayAccessExpression : public Expression {
    std::unique_ptr<Expression> array;
    std::unique_ptr<Expression> index;
    
    ArrayAccessExpression(size_t line, size_t column, size_t start_pos, size_t end_pos,
                         std::unique_ptr<Expression> arr, std::unique_ptr<Expression> idx)
        : Expression(line, column, start_pos, end_pos),
          array(std::move(arr)), index(std::move(idx)) {}
    
    TokenType getType() const override { return TokenType::TYPE_OBJECT; }
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// New expression (object creation)
struct NewExpression : public Expression {
    std::string type_name;
    std::vector<std::unique_ptr<Expression>> arguments;
    
    NewExpression(size_t line, size_t column, size_t start_pos, size_t end_pos,
                 const std::string& type,
                 std::vector<std::unique_ptr<Expression>> args)
        : Expression(line, column, start_pos, end_pos), type_name(type), arguments(std::move(args)) {}
    
    TokenType getType() const override { return TokenType::TYPE_OBJECT; }
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Expression statement
struct ExpressionStatement : public Statement {
    std::unique_ptr<Expression> expression;
    
    ExpressionStatement(size_t line, size_t column, size_t start_pos, size_t end_pos,
                       std::unique_ptr<Expression> expr)
        : Statement(line, column, start_pos, end_pos), expression(std::move(expr)) {}
    
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Return statement
struct ReturnStatement : public Statement {
    std::unique_ptr<Expression> value;
    
    ReturnStatement(size_t line, size_t column, size_t start_pos, size_t end_pos,
                   std::unique_ptr<Expression> val = nullptr)
        : Statement(line, column, start_pos, end_pos), value(std::move(val)) {}
    
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// If statement
struct IfStatement : public Statement {
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> then_branch;
    std::vector<std::unique_ptr<Statement>> else_branch;
    std::vector<std::unique_ptr<IfStatement>> else_if_branches;
    
    IfStatement(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : Statement(line, column, start_pos, end_pos) {}
    
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// For loop statement
struct ForStatement : public Statement {
    std::unique_ptr<ASTNode> initializer;
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> increment;
    std::vector<std::unique_ptr<Statement>> body;
    
  ForStatement(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : Statement(line, column, start_pos, end_pos) {}
    
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// For-each loop statement
struct ForEachStatement : public Statement {
    std::string variable_name;
    std::unique_ptr<Expression> collection;
    std::vector<std::unique_ptr<Statement>> body;
    
 ForEachStatement(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : Statement(line, column, start_pos, end_pos) {}
    
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// While loop statement
struct WhileStatement : public Statement {
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> body;
    
 WhileStatement(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : Statement(line, column, start_pos, end_pos) {}
    
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Do-while loop statement
struct DoWhileStatement : public Statement {
    std::vector<std::unique_ptr<Statement>> body;
    std::unique_ptr<Expression> condition;
    
DoWhileStatement(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : Statement(line, column, start_pos, end_pos) {}
    
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Try-catch-finally statement
struct TryStatement : public Statement {
    std::vector<std::unique_ptr<Statement>> try_block;
    std::string catch_variable;
    TokenType catch_type;
    std::vector<std::unique_ptr<Statement>> catch_block;
    std::vector<std::unique_ptr<Statement>> finally_block;
    bool has_catch;
    bool has_finally;
    
TryStatement(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : Statement(line, column, start_pos, end_pos), has_catch(false), has_finally(false) {}
    
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Block statement
struct BlockStatement : public Statement {
    std::vector<std::unique_ptr<ASTNode>> statements;
    
 BlockStatement(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : Statement(line, column, start_pos, end_pos) {}
    
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Print statement
struct PrintStatement : public Statement {
    std::unique_ptr<Expression> expression;
    
    PrintStatement(size_t line, size_t column, size_t start_pos, size_t end_pos,
                  std::unique_ptr<Expression> expr)
        : Statement(line, column, start_pos, end_pos), expression(std::move(expr)) {}
    
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Continue statement
struct ContinueStatement : public Statement {
    ContinueStatement(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : Statement(line, column, start_pos, end_pos) {}
    
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Break statement
struct BreakStatement : public Statement {
    BreakStatement(size_t line, size_t column, size_t start_pos, size_t end_pos)
        : Statement(line, column, start_pos, end_pos) {}
    
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

// Throw statement
struct ThrowStatement : public Statement {
    std::unique_ptr<Expression> expression;
    
    ThrowStatement(size_t line, size_t column, size_t start_pos, size_t end_pos,
                  std::unique_ptr<Expression> expr)
        : Statement(line, column, start_pos, end_pos), expression(std::move(expr)) {}
    
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

} // namespace darija

#endif // AST_HPP
