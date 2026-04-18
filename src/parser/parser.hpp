#ifndef PARSER_HPP
#define PARSER_HPP

#include "../lexer/lexer.hpp"
#include "../ast/ast.hpp"
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>

namespace darija {

// Parser error exception
struct ParseError : public std::runtime_error {
    size_t line;
    size_t column;
    std::string message;
    
    ParseError(size_t line, size_t column, const std::string& msg)
        : std::runtime_error(msg), line(line), column(column), message(msg) {}
};

// Parser class
class Parser {
public:
    explicit Parser(std::unique_ptr<Lexer> lexer);
    
    // Parse the entire program
    std::unique_ptr<ProgramNode> parse();
    
    // Error recovery
    void synchronize();

    // Token methods
    Token advance();
    bool check(TokenType type);
    bool match(TokenType type);
    TokenValue match(TokenType type, bool& matched);
    bool matchString(TokenType type, std::string& out);
    void expect(TokenType type, const std::string& message);
    bool isAtEnd();
    Token peekNextToken();
    
    // Program structure
    std::unique_ptr<ImportNode> parseImport();
    std::unique_ptr<FunctionNode> parseFunction();
    std::unique_ptr<ClassNode> parseClass();
    std::unique_ptr<FunctionNode> parseMain();
    
    // Class members
    std::unique_ptr<ASTNode> parseClassMember();
    std::unique_ptr<FieldNode> parseField();
    std::unique_ptr<MethodNode> parseMethod();
    std::vector<std::unique_ptr<ASTNode>> parseClassMembers();
    
    // Statements
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<BlockStatement> parseBlock();
    std::unique_ptr<VariableNode> parseVariableDeclaration();
    std::unique_ptr<ExpressionStatement> parseExpressionStatement();
    std::unique_ptr<ReturnStatement> parseReturn();
    std::unique_ptr<IfStatement> parseIf();
    std::unique_ptr<ForStatement> parseFor();
    std::unique_ptr<ForEachStatement> parseForEach();
    std::unique_ptr<WhileStatement> parseWhile();
    std::unique_ptr<DoWhileStatement> parseDoWhile();
    std::unique_ptr<TryStatement> parseTry();
    std::unique_ptr<PrintStatement> parsePrint();
    std::vector<std::unique_ptr<Statement>> parseBlockStatements();
    std::unique_ptr<MethodNode> parseMethod(TokenType access_modifier);
    std::unique_ptr<FieldNode> parseField(TokenType access_modifier);

private:
    std::unique_ptr<Lexer> lexer_;
    Token current_token_;
    bool at_end_;
    
    // Internal parsing methods
    std::unique_ptr<ProgramNode> parseProgram();
    std::vector<std::pair<std::string, TokenType>> parseParameters();
    TokenType parseType();

public:
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseAssignment();
    std::unique_ptr<Expression> parseLogicalOr();
    std::unique_ptr<Expression> parseLogicalAnd();
    std::unique_ptr<Expression> parseEquality();
    std::unique_ptr<Expression> parseComparison();
    std::unique_ptr<Expression> parseTerm();
    std::unique_ptr<Expression> parseFactor();
    std::unique_ptr<Expression> parseUnary();
    std::unique_ptr<Expression> parseCall();
    std::unique_ptr<Expression> parsePrimary();
};

} // namespace darija

#endif // PARSER_HPP
