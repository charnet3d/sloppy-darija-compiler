#include "../src/ast/ast.hpp"
#include "test_common.hpp"
#include <iostream>
#include <memory>

using namespace darija;
using namespace darija_tests;

// Test: Program node creation
void test_program_node() {
    auto program = std::make_unique<ProgramNode>(1, 1, 0, 100);
    ASSERT_NE_NULL(program);
    ASSERT_EQ("", program->module_name);
    ASSERT_EQ(0u, program->imports.size());
    ASSERT_EQ(0u, program->type_declarations.size());
    ASSERT_EQ(0u, program->function_declarations.size());
}

// Test: Import node creation
void test_import_node() {
    auto import = std::make_unique<ImportNode>(1, 1, 0, 20);
    import->module_name = "math";
    import->path = "standard/math";
    
    ASSERT_EQ("math", import->module_name);
    ASSERT_EQ("standard/math", import->path);
}

// Test: Function node creation
void test_function_node() {
    auto func = std::make_unique<FunctionNode>(1, 1, 0, 100);
    func->name = "add";
    func->parameters.push_back({"a", TokenType::TYPE_INT});
    func->parameters.push_back({"b", TokenType::TYPE_INT});
    func->return_type = TokenType::TYPE_INT;
    
    ASSERT_EQ("add", func->name);
    ASSERT_EQ(2u, func->parameters.size());
    ASSERT_EQ(TokenType::TYPE_INT, func->return_type);
}

// Test: Class node creation
void test_class_node() {
    auto cls = std::make_unique<ClassNode>(1, 1, 0, 200);
    cls->name = "Calculator";
    cls->parent_class = "Object";
    
    ASSERT_EQ("Calculator", cls->name);
    ASSERT_EQ("Object", cls->parent_class);
}

// Test: Field node creation
void test_field_node() {
    auto field = std::make_unique<FieldNode>(1, 1, 0, 30);
    field->access_modifier = TokenType::KW_PUBLIC;
    field->type = TokenType::TYPE_INT;
    field->name = "value";
    
    ASSERT_EQ(TokenType::KW_PUBLIC, field->access_modifier);
    ASSERT_EQ(TokenType::TYPE_INT, field->type);
    ASSERT_EQ("value", field->name);
}

// Test: Method node creation
void test_method_node() {
    auto method = std::make_unique<MethodNode>(1, 1, 0, 100);
    method->access_modifier = TokenType::KW_PUBLIC;
    method->name = "calculate";
    method->return_type = TokenType::TYPE_INT;
    method->is_constructor = false;
    
    ASSERT_EQ(TokenType::KW_PUBLIC, method->access_modifier);
    ASSERT_EQ("calculate", method->name);
    ASSERT_EQ(TokenType::TYPE_INT, method->return_type);
}

// Test: Variable node creation
void test_variable_node() {
    auto var = std::make_unique<VariableNode>(1, 1, 0, 20);
    var->type = TokenType::TYPE_INT;
    var->name = "x";
    var->is_constant = false;
    
    ASSERT_EQ(TokenType::TYPE_INT, var->type);
    ASSERT_EQ("x", var->name);
    ASSERT_TRUE(!var->is_constant);
}

// Test: Literal expression creation
void test_literal_expression() {
    auto lit = std::make_unique<LiteralExpression>(1, 1, 0, 2, TokenType::LITERAL_INTEGER, std::variant<int64_t, double, std::string, char, bool>(42));
    ASSERT_NE_NULL(lit);
    ASSERT_EQ(TokenType::LITERAL_INTEGER, lit->literal_type);
    ASSERT_EQ(TokenType::LITERAL_INTEGER, lit->getType());
}

// Test: Identifier expression creation
void test_identifier_expression() {
    auto ident = std::make_unique<IdentifierExpression>(1, 1, 0, 1, "myVar");
    ASSERT_NE_NULL(ident);
    ASSERT_EQ("myVar", ident->name);
}

// Test: Binary expression creation
void test_binary_expression() {
    auto left = std::make_unique<IdentifierExpression>(1, 1, 0, 1, "a");
    auto right = std::make_unique<IdentifierExpression>(1, 1, 3, 4, "b");
    auto bin = std::make_unique<BinaryExpression>(1, 1, 0, 4, TokenType::OP_PLUS, std::move(left), std::move(right));
    
    ASSERT_NE_NULL(bin);
    ASSERT_EQ(TokenType::OP_PLUS, bin->operator_type);
}

// Test: Unary expression creation
void test_unary_expression() {
    auto operand = std::make_unique<IdentifierExpression>(1, 1, 1, 2, "x");
    auto unary = std::make_unique<UnaryExpression>(1, 1, 0, 2, TokenType::OP_MINUS, std::move(operand));
    
    ASSERT_NE_NULL(unary);
    ASSERT_EQ(TokenType::OP_MINUS, unary->operator_type);
}

// Test: Call expression creation
void test_call_expression() {
    auto callee = std::make_unique<IdentifierExpression>(1, 1, 0, 3, "foo");
    std::vector<std::unique_ptr<Expression>> args;
    args.push_back(std::make_unique<LiteralExpression>(1, 1, 4, 5, TokenType::LITERAL_INTEGER, std::variant<int64_t, double, std::string, char, bool>(42)));
    auto call = std::make_unique<CallExpression>(1, 1, 0, 6, std::move(callee), std::move(args));
    
    ASSERT_NE_NULL(call);
    ASSERT_EQ(1u, call->arguments.size());
}

// Test: Member expression creation
void test_member_expression() {
    auto obj = std::make_unique<IdentifierExpression>(1, 1, 0, 3, "obj");
    auto member = std::make_unique<MemberExpression>(1, 1, 0, 10, std::move(obj), "property");
    
    ASSERT_NE_NULL(member);
    ASSERT_EQ("property", member->member);
}

// Test: Array access expression creation
void test_array_access_expression() {
    auto arr = std::make_unique<IdentifierExpression>(1, 1, 0, 3, "arr");
    auto idx = std::make_unique<LiteralExpression>(1, 1, 5, 6, TokenType::LITERAL_INTEGER, std::variant<int64_t, double, std::string, char, bool>(0));
    auto access = std::make_unique<ArrayAccessExpression>(1, 1, 0, 7, std::move(arr), std::move(idx));
    
    ASSERT_NE_NULL(access);
}

// Test: Return statement creation
void test_return_statement() {
    auto value = std::make_unique<LiteralExpression>(1, 1, 7, 9, TokenType::LITERAL_INTEGER, std::variant<int64_t, double, std::string, char, bool>(42));
    auto ret = std::make_unique<ReturnStatement>(1, 1, 0, 9, std::move(value));
    
    ASSERT_NE_NULL(ret);
}

// Test: If statement creation
void test_if_statement() {
    auto condition = std::make_unique<LiteralExpression>(1, 1, 3, 4, TokenType::TYPE_BOOL, std::variant<int64_t, double, std::string, char, bool>(true));
    std::vector<std::unique_ptr<Statement>> then_branch;
    std::vector<std::unique_ptr<Statement>> else_branch;
    
    auto if_stmt = std::make_unique<IfStatement>(1, 1, 0, 20);
    if_stmt->condition = std::move(condition);
    if_stmt->then_branch = std::move(then_branch);
    if_stmt->else_branch = std::move(else_branch);
    
    ASSERT_NE_NULL(if_stmt);
}

// Test: For statement creation
void test_for_statement() {
    auto init = std::make_unique<VariableNode>(1, 1, 0, 10);
    init->type = TokenType::TYPE_INT;
    init->name = "i";
    
    auto condition = std::make_unique<IdentifierExpression>(1, 1, 13, 14, "i");
    auto increment = std::make_unique<IdentifierExpression>(1, 1, 17, 18, "i");
    std::vector<std::unique_ptr<Statement>> body;
    
    auto for_stmt = std::make_unique<ForStatement>(1, 1, 0, 25);
    for_stmt->initializer = std::move(init);
    for_stmt->condition = std::move(condition);
    for_stmt->increment = std::move(increment);
    for_stmt->body = std::move(body);
    
    ASSERT_NE_NULL(for_stmt);
}

// Test: While statement creation
void test_while_statement() {
    auto while_stmt = std::make_unique<WhileStatement>(1, 1, 0, 20);
    
    ASSERT_NE_NULL(while_stmt);
}

// Test: Block statement creation
void test_block_statement() {
    auto block = std::make_unique<BlockStatement>(1, 1, 0, 50);
    
    ASSERT_NE_NULL(block);
    ASSERT_EQ(0u, block->statements.size());
}

// Test: Expression statement creation
void test_expression_statement() {
    auto expr = std::make_unique<LiteralExpression>(1, 1, 0, 2, TokenType::LITERAL_INTEGER, std::variant<int64_t, double, std::string, char, bool>(42));
    auto stmt = std::make_unique<ExpressionStatement>(1, 1, 0, 2, std::move(expr));
    
    ASSERT_NE_NULL(stmt);
}

// Test: Print statement creation
void test_print_statement() {
    auto expr = std::make_unique<LiteralExpression>(1, 1, 6, 13, TokenType::LITERAL_STRING, std::variant<int64_t, double, std::string, char, bool>(std::string("hello")));
    auto print = std::make_unique<PrintStatement>(1, 1, 0, 13, std::move(expr));
    
    ASSERT_NE_NULL(print);
}

// Test: Try statement creation
void test_try_statement() {
    auto try_stmt = std::make_unique<TryStatement>(1, 1, 0, 100);
    
    ASSERT_NE_NULL(try_stmt);
}

// Test: Continue statement creation
void test_continue_statement() {
    auto cont = std::make_unique<ContinueStatement>(1, 1, 0, 9);
    ASSERT_NE_NULL(cont);
}

// Test: Break statement creation
void test_break_statement() {
    auto brk = std::make_unique<BreakStatement>(1, 1, 0, 5);
    ASSERT_NE_NULL(brk);
}

// Test: AST node source location
void test_ast_node_location() {
    auto program = std::make_unique<ProgramNode>(42, 10, 100, 200);
    
    ASSERT_EQ(42u, program->getLine());
    ASSERT_EQ(10u, program->getColumn());
    ASSERT_EQ(100u, program->getStartPos());
    ASSERT_EQ(200u, program->getEndPos());
}

// Test: Nested AST structure
void test_nested_ast_structure() {
    // Create a simple nested structure: Program -> Function -> Block -> Statement
    auto program = std::make_unique<ProgramNode>(1, 1, 0, 100);
    program->module_name = "test";
    
    auto func = std::make_unique<FunctionNode>(1, 1, 0, 50);
    func->name = "main";
    func->return_type = TokenType::KW_VOID;
    
    auto block = std::make_unique<BlockStatement>(1, 1, 10, 50);
    
    auto print = std::make_unique<PrintStatement>(1, 1, 12, 25, std::make_unique<LiteralExpression>(1, 1, 18, 25, TokenType::LITERAL_STRING, std::variant<int64_t, double, std::string, char, bool>(std::string("hello"))));
    
    block->statements.push_back(std::move(print));
    func->body.push_back(std::move(block));
    program->function_declarations.push_back(std::move(func));
    
    ASSERT_EQ("test", program->module_name);
    ASSERT_EQ(1u, program->function_declarations.size());
}

// Run all tests
void run_ast_tests() {
    std::cout << "Running AST tests..." << std::endl;
    
    test_program_node();
    test_import_node();
    test_function_node();
    test_class_node();
    test_field_node();
    test_method_node();
    test_variable_node();
    test_literal_expression();
    test_identifier_expression();
    test_binary_expression();
    test_unary_expression();
    test_call_expression();
    test_member_expression();
    test_array_access_expression();
    test_return_statement();
    test_if_statement();
    test_for_statement();
    test_while_statement();
    test_block_statement();
    test_expression_statement();
    test_print_statement();
    test_try_statement();
    test_continue_statement();
    test_break_statement();
    test_ast_node_location();
    test_nested_ast_structure();
}
