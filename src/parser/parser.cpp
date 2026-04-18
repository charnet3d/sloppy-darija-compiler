#include "parser.hpp"
#include <algorithm>
#include <sstream>

namespace darija {

// Helper: convert vector<unique_ptr<Statement>> to vector<unique_ptr<ASTNode>>
static std::vector<std::unique_ptr<ASTNode>> to_ast_nodes(
    std::vector<std::unique_ptr<Statement>>& stmts) {
    std::vector<std::unique_ptr<ASTNode>> result;
    result.reserve(stmts.size());
    for (auto& s : stmts) result.push_back(std::move(s));
    return result;
}

// Constructor
Parser::Parser(std::unique_ptr<Lexer> lexer)
    : lexer_(std::move(lexer)), at_end_(false) {
    current_token_ = lexer_->nextToken();
}

// Advance to next token
Token Parser::advance() {
    if (!isAtEnd()) {
        current_token_ = lexer_->nextToken();
    }
    return current_token_;
}

// Check if current token matches type
bool Parser::check(TokenType type) {
    return current_token_.type == type;
}

// Match and consume token if type matches
bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

// Match and return the current token's value if type matches
TokenValue Parser::match(TokenType type, bool& matched) {
    matched = false;
    if (check(type)) {
        auto value = current_token_.value;
        advance();
        matched = true;
        return value;
    }
    return current_token_.value;
}

// Match and return current token's string value if type matches
bool Parser::matchString(TokenType type, std::string& out) {
    if (check(type)) {
        out = std::get<std::string>(current_token_.value);
        advance();
        return true;
    }
    return false;
}

// Expect token of specific type
void Parser::expect(TokenType type, const std::string& message) {
    if (!check(type)) {
        throw ParseError(current_token_.line, current_token_.column, message);
    }
    advance();
}

// Check if at end of input
bool Parser::isAtEnd() {
    return current_token_.type == TokenType::TOKEN_EOF;
}

// Error recovery - synchronize to next statement
void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (current_token_.type == TokenType::DELIM_SEMICOLON) {
            return;
        }
        if (current_token_.isKeyword() || 
            current_token_.type == TokenType::DELIM_LBRACE ||
            current_token_.type == TokenType::DELIM_RBRACE) {
            return;
        }
        advance();
    }
}

// Parse entire program
std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>(
        current_token_.line, current_token_.column,
        current_token_.start_pos, current_token_.end_pos
    );
    
    // Parse module declaration
    if (match(TokenType::KW_MODULE)) {
        std::string module_name;
        matchString(TokenType::LITERAL_IDENTIFIER, module_name);
        program->module_name = module_name;
        expect(TokenType::DELIM_LBRACE, "Expected '{' after module name");
    }
    
    // Parse imports and declarations
    while (!isAtEnd() && !check(TokenType::DELIM_RBRACE)) {
        if (check(TokenType::KW_IMPORT)) {
            program->imports.push_back(parseImport());
        } else if (check(TokenType::KW_CLASS)) {
            program->type_declarations.push_back(parseClass());
        } else if (check(TokenType::KW_FUNCTION)) {
            program->function_declarations.push_back(parseFunction());
        } else if (check(TokenType::KW_MAIN)) {
            program->main_function = parseMain();
            break; // Main should be last
        } else {
            advance(); // Skip unknown
        }
    }
    
    if (check(TokenType::DELIM_RBRACE)) {
        advance();
    }
    
    return program;
}

// Parse import statement
std::unique_ptr<ImportNode> Parser::parseImport() {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    advance(); // consume 'استورد'
    
    std::string module_name;
    matchString(TokenType::LITERAL_IDENTIFIER, module_name);
    advance();
    
    match(TokenType::KW_LIBRARY);
    std::string path;
    matchString(TokenType::LITERAL_STRING, path);
    advance();
    
    match(TokenType::DELIM_SEMICOLON);
    
    auto import = std::make_unique<ImportNode>(line, column, start_pos, lexer_->currentPosition());
    import->module_name = module_name;
    import->path = path;
    
    return import;
}

// Parse function declaration
std::unique_ptr<FunctionNode> Parser::parseFunction() {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    advance(); // consume 'دالة'
    
    std::string name;
    matchString(TokenType::LITERAL_IDENTIFIER, name);
    advance();
    
    expect(TokenType::DELIM_LPAREN, "Expected '(' for parameters");
    auto parameters = parseParameters();
    expect(TokenType::DELIM_RPAREN, "Expected ')' after parameters");
    
    expect(TokenType::OP_COLON, "Expected ':' before return type");
    TokenType return_type = parseType();
    
    expect(TokenType::DELIM_LBRACE, "Expected '{' for function body");
    auto body = parseBlockStatements();
    expect(TokenType::DELIM_RBRACE, "Expected '}' at end of function");
    
    auto func = std::make_unique<FunctionNode>(line, column, start_pos, lexer_->currentPosition());
    func->name = name;
    func->parameters = std::move(parameters);
    func->return_type = return_type;
    func->body = to_ast_nodes(body);
    
    return func;
}

// Parse main function
std::unique_ptr<FunctionNode> Parser::parseMain() {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    advance(); // consume 'الرئيسية'
    
    expect(TokenType::OP_COLON, "Expected ':' after 'الرئيسية'");
    
    auto body = parseBlockStatements();
    
    auto func = std::make_unique<FunctionNode>(line, column, start_pos, lexer_->currentPosition());
    func->name = "main";
    func->return_type = TokenType::KW_VOID;
    func->body = to_ast_nodes(body);
    func->is_main = true;
    
    return func;
}

// Parse class declaration
std::unique_ptr<ClassNode> Parser::parseClass() {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    advance(); // consume 'صنف'
    
    std::string name;
    matchString(TokenType::LITERAL_IDENTIFIER, name);
    advance();
    
    // Check for inheritance
    std::string parent_class;
    if (match(TokenType::KW_EXTENDS)) {
        matchString(TokenType::LITERAL_IDENTIFIER, parent_class);
        advance();
    }
    
    expect(TokenType::DELIM_LBRACE, "Expected '{' for class body");
    auto members = parseClassMembers();
    expect(TokenType::DELIM_RBRACE, "Expected '}' at end of class");
    
    auto cls = std::make_unique<ClassNode>(line, column, start_pos, lexer_->currentPosition());
    cls->name = name;
    cls->parent_class = parent_class;
    cls->members = std::move(members);
    
    return cls;
}

// Parse class members
std::vector<std::unique_ptr<ASTNode>> Parser::parseClassMembers() {
    std::vector<std::unique_ptr<ASTNode>> members;
    
    while (!isAtEnd() && !check(TokenType::DELIM_RBRACE)) {
        members.push_back(parseClassMember());
    }
    
    return members;
}

// Parse single class member
std::unique_ptr<ASTNode> Parser::parseClassMember() {
    // Check for access modifier
    TokenType access_modifier = TokenType::KW_PUBLIC;
    if (match(TokenType::KW_PUBLIC)) {
        access_modifier = TokenType::KW_PUBLIC;
    } else if (match(TokenType::KW_PRIVATE)) {
        access_modifier = TokenType::KW_PRIVATE;
    } else if (match(TokenType::KW_PROTECTED)) {
        access_modifier = TokenType::KW_PROTECTED;
    }
    
    if (check(TokenType::KW_FUNCTION)) {
        return parseMethod(access_modifier);
    } else {
        return parseField(access_modifier);
    }
}

// Parse field declaration
std::unique_ptr<FieldNode> Parser::parseField(TokenType access_modifier) {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    TokenType type = parseType();
    
    std::string name;
    matchString(TokenType::LITERAL_IDENTIFIER, name);
    advance();
    
    std::unique_ptr<Expression> initializer = nullptr;
    if (match(TokenType::OP_ASSIGN)) {
        initializer = parseExpression();
    }
    
    match(TokenType::DELIM_SEMICOLON);
    
    auto field = std::make_unique<FieldNode>(line, column, start_pos, lexer_->currentPosition());
    field->access_modifier = access_modifier;
    field->type = type;
    field->name = name;
    field->initializer = std::move(initializer);
    
    return field;
}

// Parse method declaration
std::unique_ptr<MethodNode> Parser::parseMethod(TokenType access_modifier) {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    advance(); // consume 'دالة'
    
    std::string name;
    matchString(TokenType::LITERAL_IDENTIFIER, name);
    advance();
    
    expect(TokenType::DELIM_LPAREN, "Expected '(' for parameters");
    auto parameters = parseParameters();
    expect(TokenType::DELIM_RPAREN, "Expected ')' after parameters");
    
    TokenType return_type = TokenType::KW_VOID;
    if (match(TokenType::OP_COLON)) {
        return_type = parseType();
    }
    
    expect(TokenType::DELIM_LBRACE, "Expected '{' for method body");
    auto body = parseBlockStatements();
    expect(TokenType::DELIM_RBRACE, "Expected '}' at end of method");
    
    auto method = std::make_unique<MethodNode>(line, column, start_pos, lexer_->currentPosition());
    method->access_modifier = access_modifier;
    method->name = name;
    method->parameters = std::move(parameters);
    method->return_type = return_type;
    method->body = to_ast_nodes(body);
    method->is_constructor = (name == "constructor");
    
    return method;
}

// Parse parameters
std::vector<std::pair<std::string, TokenType>> Parser::parseParameters() {
    std::vector<std::pair<std::string, TokenType>> parameters;
    
    if (!check(TokenType::DELIM_RPAREN)) {
        do {
            TokenType type = parseType();
            
            std::string name;
            matchString(TokenType::LITERAL_IDENTIFIER, name);
            advance();
            
            parameters.emplace_back(name, type);
        } while (match(TokenType::DELIM_COMMA));
    }
    
    return parameters;
}

// Parse type
TokenType Parser::parseType() {
    TokenType type = current_token_.type;
    
    if (type == TokenType::TYPE_INT || type == TokenType::TYPE_LONG ||
        type == TokenType::TYPE_FLOAT || type == TokenType::TYPE_DOUBLE ||
        type == TokenType::TYPE_BOOL || type == TokenType::TYPE_CHAR ||
        type == TokenType::TYPE_ARRAY || type == TokenType::TYPE_OBJECT ||
        type == TokenType::KW_VOID) {
        advance();
        return type;
    }
    
    // Check for multi-word types
    if (check(TokenType::LITERAL_IDENTIFIER)) {
        std::string ident = std::get<std::string>(current_token_.value);
        advance();
        return TokenType::TYPE_OBJECT; // Default to object for unknown types
    }
    
    throw ParseError(current_token_.line, current_token_.column, "Expected type");
}

// Parse block statements
std::vector<std::unique_ptr<Statement>> Parser::parseBlockStatements() {
    std::vector<std::unique_ptr<Statement>> statements;
    
    while (!isAtEnd() && !check(TokenType::DELIM_RBRACE)) {
        statements.push_back(parseStatement());
    }
    
    return statements;
}

// Parse statement
std::unique_ptr<Statement> Parser::parseStatement() {
    if (check(TokenType::KW_DECLARE) || check(TokenType::KW_CONST)) {
        return parseVariableDeclaration();
    } else if (check(TokenType::KW_IF)) {
        return parseIf();
    } else if (check(TokenType::KW_FOR)) {
        if (peekNextToken().type == TokenType::KW_FOR_EACH) {
            return parseForEach();
        }
        return parseFor();
    } else if (check(TokenType::KW_WHILE)) {
        return parseWhile();
    } else if (check(TokenType::KW_DO)) {
        return parseDoWhile();
    } else if (check(TokenType::KW_TRY)) {
        return parseTry();
    } else if (check(TokenType::KW_RETURN)) {
        return parseReturn();
    } else if (check(TokenType::KW_PRINT)) {
        return parsePrint();
    } else if (check(TokenType::DELIM_LBRACE)) {
        return parseBlock();
    } else {
        return parseExpressionStatement();
    }
}

// Peek at next token without consuming
Token Parser::peekNextToken() {
    return lexer_->peekToken();
}

// Parse variable declaration
std::unique_ptr<VariableNode> Parser::parseVariableDeclaration() {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    bool is_constant = check(TokenType::KW_CONST);
    advance(); // consume 'اعرف' or 'ثابت'
    
    TokenType type = TokenType::TYPE_OBJECT; // Default
    if (match(TokenType::OP_COLON)) {
        type = parseType();
    }
    
    std::string name;
    matchString(TokenType::LITERAL_IDENTIFIER, name);
    advance();
    
    std::unique_ptr<Expression> initializer = nullptr;
    if (match(TokenType::OP_ASSIGN)) {
        initializer = parseExpression();
    }
    
    match(TokenType::DELIM_SEMICOLON);
    
    auto var = std::make_unique<VariableNode>(line, column, start_pos, lexer_->currentPosition());
    var->type = type;
    var->name = name;
    var->initializer = std::move(initializer);
    var->is_constant = is_constant;
    
    return var;
}

// Parse if statement
std::unique_ptr<IfStatement> Parser::parseIf() {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    advance(); // consume 'إلا'
    
    expect(TokenType::DELIM_LPAREN, "Expected '(' after 'إلا'");
    auto condition = parseExpression();
    expect(TokenType::DELIM_RPAREN, "Expected ')' after condition");
    
    auto then_branch = parseBlockStatements();
    
    std::vector<std::unique_ptr<Statement>> else_branch;
    std::vector<std::unique_ptr<IfStatement>> else_if_branches;
    
    while (match(TokenType::KW_ELSE)) {
        if (match(TokenType::KW_IF)) {
            // else-if
            expect(TokenType::DELIM_LPAREN, "Expected '(' after else-if");
            auto else_if_cond = parseExpression();
            expect(TokenType::DELIM_RPAREN, "Expected ')' after else-if condition");
            auto else_if_body = parseBlockStatements();
            
            auto else_if = std::make_unique<IfStatement>(
                current_token_.line, current_token_.column,
                current_token_.start_pos, lexer_->currentPosition()
            );
            else_if->condition = std::move(else_if_cond);
            else_if->then_branch = std::move(else_if_body);
            else_if_branches.push_back(std::move(else_if));
        } else {
            // else
            else_branch = parseBlockStatements();
            break;
        }
    }
    
    auto if_stmt = std::make_unique<IfStatement>(line, column, start_pos, lexer_->currentPosition());
    if_stmt->condition = std::move(condition);
    if_stmt->then_branch = std::move(then_branch);
    if_stmt->else_branch = std::move(else_branch);
    if_stmt->else_if_branches = std::move(else_if_branches);
    
    return if_stmt;
}

// Parse for loop
std::unique_ptr<ForStatement> Parser::parseFor() {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    advance(); // consume 'عاود'
    
    expect(TokenType::DELIM_LPAREN, "Expected '(' after 'عاود'");
    
    // Initializer
    std::unique_ptr<ASTNode> initializer = nullptr;
    if (!check(TokenType::DELIM_SEMICOLON)) {
        if (check(TokenType::KW_DECLARE)) {
            initializer = parseVariableDeclaration();
        } else {
            initializer = std::make_unique<ExpressionStatement>(
                current_token_.line, current_token_.column,
                current_token_.start_pos, lexer_->currentPosition(),
                parseExpression()
            );
        }
    }
    match(TokenType::DELIM_SEMICOLON);
    
    // Condition
    std::unique_ptr<Expression> condition = nullptr;
    if (!check(TokenType::DELIM_SEMICOLON)) {
        condition = parseExpression();
    }
    match(TokenType::DELIM_SEMICOLON);
    
    // Increment
    std::unique_ptr<Expression> increment = nullptr;
    if (!check(TokenType::DELIM_RPAREN)) {
        increment = parseExpression();
    }
    expect(TokenType::DELIM_RPAREN, "Expected ')' after for header");
    
    auto body = parseBlockStatements();
    
    auto for_stmt = std::make_unique<ForStatement>(line, column, start_pos, lexer_->currentPosition());
    for_stmt->initializer = std::move(initializer);
    for_stmt->condition = std::move(condition);
    for_stmt->increment = std::move(increment);
    for_stmt->body = std::move(body);
    
    return for_stmt;
}

// Parse for-each loop
std::unique_ptr<ForEachStatement> Parser::parseForEach() {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    advance(); // consume 'عاود'
    advance(); // consume 'لكل'
    
    std::string var_name;
    matchString(TokenType::LITERAL_IDENTIFIER, var_name);
    advance();
    
    advance(); // consume 'في'
    
    auto collection = parseExpression();
    
    auto body = parseBlockStatements();
    
    auto for_each = std::make_unique<ForEachStatement>(line, column, start_pos, lexer_->currentPosition());
    for_each->variable_name = var_name;
    for_each->collection = std::move(collection);
    for_each->body = std::move(body);
    
    return for_each;
}

// Parse while loop
std::unique_ptr<WhileStatement> Parser::parseWhile() {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    advance(); // consume 'ما دام'
    
    expect(TokenType::DELIM_LPAREN, "Expected '(' after 'ما دام'");
    auto condition = parseExpression();
    expect(TokenType::DELIM_RPAREN, "Expected ')' after condition");
    
    auto body = parseBlockStatements();
    
    auto while_stmt = std::make_unique<WhileStatement>(line, column, start_pos, lexer_->currentPosition());
    while_stmt->condition = std::move(condition);
    while_stmt->body = std::move(body);
    
    return while_stmt;
}

// Parse do-while loop
std::unique_ptr<DoWhileStatement> Parser::parseDoWhile() {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    advance(); // consume 'دير'
    
    auto body = parseBlockStatements();
    
    advance(); // consume 'ما دام'
    expect(TokenType::DELIM_LPAREN, "Expected '(' after 'ما دام'");
    auto condition = parseExpression();
    expect(TokenType::DELIM_RPAREN, "Expected ')' after condition");
    match(TokenType::DELIM_SEMICOLON);
    
    auto do_while = std::make_unique<DoWhileStatement>(line, column, start_pos, lexer_->currentPosition());
    do_while->body = std::move(body);
    do_while->condition = std::move(condition);
    
    return do_while;
}

// Parse try-catch-finally
std::unique_ptr<TryStatement> Parser::parseTry() {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    advance(); // consume 'جرب'
    
    auto try_block = parseBlockStatements();
    
    std::string catch_var;
    TokenType catch_type = TokenType::TYPE_OBJECT;
    std::vector<std::unique_ptr<Statement>> catch_block;
    std::vector<std::unique_ptr<Statement>> finally_block;
    
    if (match(TokenType::KW_CATCH)) {
        expect(TokenType::DELIM_LPAREN, "Expected '(' after 'شد'");
        matchString(TokenType::LITERAL_IDENTIFIER, catch_var);
        advance();
        
        advance(); // consume 'كاستثناء'
        
        catch_type = parseType();
        expect(TokenType::DELIM_RPAREN, "Expected ')' after catch header");
        
        catch_block = parseBlockStatements();
    }
    
    if (match(TokenType::KW_FINALLY)) {
        finally_block = parseBlockStatements();
    }
    
    auto try_stmt = std::make_unique<TryStatement>(line, column, start_pos, lexer_->currentPosition());
    try_stmt->try_block = std::move(try_block);
    try_stmt->catch_variable = catch_var;
    try_stmt->catch_type = catch_type;
    try_stmt->catch_block = std::move(catch_block);
    try_stmt->finally_block = std::move(finally_block);
    
    return try_stmt;
}

// Parse return statement
std::unique_ptr<ReturnStatement> Parser::parseReturn() {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    advance(); // consume 'رجع'
    
    std::unique_ptr<Expression> value = nullptr;
    if (!check(TokenType::DELIM_SEMICOLON) && !check(TokenType::DELIM_RBRACE)) {
        value = parseExpression();
    }
    match(TokenType::DELIM_SEMICOLON);
    
    return std::make_unique<ReturnStatement>(line, column, start_pos, lexer_->currentPosition(), std::move(value));
}

// Parse print statement
std::unique_ptr<PrintStatement> Parser::parsePrint() {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    advance(); // consume 'اطبع'
    
    auto expression = parseExpression();
    match(TokenType::DELIM_SEMICOLON);
    
    return std::make_unique<PrintStatement>(line, column, start_pos, lexer_->currentPosition(), std::move(expression));
}

// Parse block statement
std::unique_ptr<BlockStatement> Parser::parseBlock() {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    advance(); // consume '{'
    auto statements = parseBlockStatements();
    expect(TokenType::DELIM_RBRACE, "Expected '}' at end of block");
    
    auto block = std::make_unique<BlockStatement>(line, column, start_pos, lexer_->currentPosition());
    block->statements = to_ast_nodes(statements);
    return block;
}

// Parse expression statement
std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    size_t line = current_token_.line;
    size_t column = current_token_.column;
    size_t start_pos = current_token_.start_pos;
    
    auto expression = parseExpression();
    match(TokenType::DELIM_SEMICOLON);
    
    return std::make_unique<ExpressionStatement>(line, column, start_pos, lexer_->currentPosition(), std::move(expression));
}

// Parse expression (entry point)
std::unique_ptr<Expression> Parser::parseExpression() {
    return parseAssignment();
}

// Parse assignment
std::unique_ptr<Expression> Parser::parseAssignment() {
    auto expr = parseLogicalOr();
    
    if (match(TokenType::OP_ASSIGN)) {
        // This would be a more complex assignment handling
        // For now, return the left side
    }
    
    return expr;
}

// Parse logical OR
std::unique_ptr<Expression> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    
    while (check(TokenType::OP_LOGICAL_OR)) {
        auto op = current_token_;
        advance();
        auto right = parseLogicalAnd();
        expr = std::make_unique<BinaryExpression>(
            op.line, op.column, op.start_pos, lexer_->currentPosition(),
            op.type, std::move(expr), std::move(right)
        );
    }
    
    return expr;
}

// Parse logical AND
std::unique_ptr<Expression> Parser::parseLogicalAnd() {
    auto expr = parseEquality();
    
    while (check(TokenType::OP_LOGICAL_AND)) {
        auto op = current_token_;
        advance();
        auto right = parseEquality();
        expr = std::make_unique<BinaryExpression>(
            op.line, op.column, op.start_pos, lexer_->currentPosition(),
            op.type, std::move(expr), std::move(right)
        );
    }
    
    return expr;
}

// Parse equality
std::unique_ptr<Expression> Parser::parseEquality() {
    auto expr = parseComparison();
    
    while (check(TokenType::OP_EQUAL) || check(TokenType::OP_NOT_EQUAL)) {
        auto op = current_token_;
        advance();
        auto right = parseComparison();
        expr = std::make_unique<BinaryExpression>(
            op.line, op.column, op.start_pos, lexer_->currentPosition(),
            op.type, std::move(expr), std::move(right)
        );
    }
    
    return expr;
}

// Parse comparison
std::unique_ptr<Expression> Parser::parseComparison() {
    auto expr = parseTerm();
    
    while (check(TokenType::OP_LESS) || check(TokenType::OP_GREATER) ||
           check(TokenType::OP_LESS_EQUAL) || check(TokenType::OP_GREATER_EQUAL)) {
        auto op = current_token_;
        advance();
        auto right = parseTerm();
        expr = std::make_unique<BinaryExpression>(
            op.line, op.column, op.start_pos, lexer_->currentPosition(),
            op.type, std::move(expr), std::move(right)
        );
    }
    
    return expr;
}

// Parse term (additive)
std::unique_ptr<Expression> Parser::parseTerm() {
    auto expr = parseFactor();
    
    while (check(TokenType::OP_PLUS) || check(TokenType::OP_MINUS)) {
        auto op = current_token_;
        advance();
        auto right = parseFactor();
        expr = std::make_unique<BinaryExpression>(
            op.line, op.column, op.start_pos, lexer_->currentPosition(),
            op.type, std::move(expr), std::move(right)
        );
    }
    
    return expr;
}

// Parse factor (multiplicative)
std::unique_ptr<Expression> Parser::parseFactor() {
    auto expr = parseUnary();
    
    while (check(TokenType::OP_MULTIPLY) || check(TokenType::OP_DIVIDE) ||
           check(TokenType::OP_MODULO)) {
        auto op = current_token_;
        advance();
        auto right = parseUnary();
        expr = std::make_unique<BinaryExpression>(
            op.line, op.column, op.start_pos, lexer_->currentPosition(),
            op.type, std::move(expr), std::move(right)
        );
    }
    
    return expr;
}

// Parse unary
std::unique_ptr<Expression> Parser::parseUnary() {
    if (check(TokenType::OP_LOGICAL_NOT) || check(TokenType::OP_MINUS)) {
        auto op = current_token_;
        advance();
        auto operand = parseUnary();
        return std::make_unique<UnaryExpression>(
            op.line, op.column, op.start_pos, lexer_->currentPosition(),
            op.type, std::move(operand)
        );
    }
    
    return parseCall();
}

// Parse call
std::unique_ptr<Expression> Parser::parseCall() {
    auto expr = parsePrimary();
    
    while (true) {
        if (match(TokenType::DELIM_LPAREN)) {
            // Function call
            std::vector<std::unique_ptr<Expression>> arguments;
            if (!check(TokenType::DELIM_RPAREN)) {
                do {
                    arguments.push_back(parseExpression());
                } while (match(TokenType::DELIM_COMMA));
            }
            expect(TokenType::DELIM_RPAREN, "Expected ')' after arguments");
            expr = std::make_unique<CallExpression>(
                expr->getLine(), expr->getColumn(), expr->getStartPos(), lexer_->currentPosition(),
                std::move(expr), std::move(arguments)
            );
        } else if (match(TokenType::DELIM_DOT)) {
            // Member access
            std::string member;
            matchString(TokenType::LITERAL_IDENTIFIER, member);
            advance();
            expr = std::make_unique<MemberExpression>(
                expr->getLine(), expr->getColumn(), expr->getStartPos(), lexer_->currentPosition(),
                std::move(expr), member
            );
        } else if (match(TokenType::DELIM_LBRACKET)) {
            // Array access
            auto index = parseExpression();
            expect(TokenType::DELIM_RBRACKET, "Expected ']' after index");
            expr = std::make_unique<ArrayAccessExpression>(
                expr->getLine(), expr->getColumn(), expr->getStartPos(), lexer_->currentPosition(),
                std::move(expr), std::move(index)
            );
        } else {
            break;
        }
    }
    
    return expr;
}

// Parse primary expression
std::unique_ptr<Expression> Parser::parsePrimary() {
    if (check(TokenType::LITERAL_INTEGER)) {
        auto token = current_token_;
        advance();
        return std::make_unique<LiteralExpression>(
            token.line, token.column, token.start_pos, token.end_pos,
            TokenType::LITERAL_INTEGER, std::get<int64_t>(token.value)
        );
    }
    
    if (check(TokenType::LITERAL_FLOAT)) {
        auto token = current_token_;
        advance();
        return std::make_unique<LiteralExpression>(
            token.line, token.column, token.start_pos, token.end_pos,
            TokenType::LITERAL_FLOAT, std::get<double>(token.value)
        );
    }
    
    if (check(TokenType::LITERAL_STRING)) {
        auto token = current_token_;
        advance();
        return std::make_unique<LiteralExpression>(
            token.line, token.column, token.start_pos, token.end_pos,
            TokenType::LITERAL_STRING, std::get<std::string>(token.value)
        );
    }
    
    if (check(TokenType::LITERAL_CHAR)) {
        auto token = current_token_;
        advance();
        return std::make_unique<LiteralExpression>(
            token.line, token.column, token.start_pos, token.end_pos,
            TokenType::LITERAL_CHAR, std::get<char>(token.value)
        );
    }
    
    if (check(TokenType::TYPE_TRUE)) {
        advance();
        return std::make_unique<LiteralExpression>(
            current_token_.line, current_token_.column, current_token_.start_pos, current_token_.end_pos,
            TokenType::TYPE_BOOL, std::string("true")
        );
    }
    
    if (check(TokenType::TYPE_FALSE)) {
        advance();
        return std::make_unique<LiteralExpression>(
            current_token_.line, current_token_.column, current_token_.start_pos, current_token_.end_pos,
            TokenType::TYPE_BOOL, std::string("false")
        );
    }
    
    if (check(TokenType::LITERAL_IDENTIFIER)) {
        auto token = current_token_;
        advance();
        return std::make_unique<IdentifierExpression>(
            token.line, token.column, token.start_pos, token.end_pos,
            std::get<std::string>(token.value)
        );
    }
    
    if (match(TokenType::DELIM_LPAREN)) {
        auto expr = parseExpression();
        expect(TokenType::DELIM_RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    throw ParseError(current_token_.line, current_token_.column, "Expected expression");
}

// Main parse entry point
std::unique_ptr<ProgramNode> Parser::parse() {
    return parseProgram();
}

} // namespace darija
