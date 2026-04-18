#include "../src/lexer/lexer.hpp"
#include "../src/parser/parser.hpp"
#include "../src/ast/ast.hpp"
#include "test_common.hpp"
#include <iostream>
#include <cassert>

using namespace darija;
using namespace darija_tests;

// Helper function to create a parser from source string
Parser createParser(const std::string& source) {
    auto lexer = std::make_unique<Lexer>(source);
    return Parser(std::move(lexer));
}

// Test: Parse integer literal
void test_parse_integer_literal() {
    std::string source = "42";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    auto* lit = dynamic_cast<LiteralExpression*>(expr.get());
    ASSERT_NE_NULL(lit);
    ASSERT_EQ(TokenType::LITERAL_INTEGER, lit->literal_type);
}

// Test: Parse float literal
void test_parse_float_literal() {
    std::string source = "3.14";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    auto* lit = dynamic_cast<LiteralExpression*>(expr.get());
    ASSERT_NE_NULL(lit);
    ASSERT_EQ(TokenType::LITERAL_FLOAT, lit->literal_type);
}

// Test: Parse string literal
void test_parse_string_literal() {
    std::string source = "\"hello\"";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    auto* lit = dynamic_cast<LiteralExpression*>(expr.get());
    ASSERT_NE_NULL(lit);
    ASSERT_EQ(TokenType::LITERAL_STRING, lit->literal_type);
}

// Test: Parse identifier
void test_parse_identifier() {
    std::string source = "x";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    auto* ident = dynamic_cast<IdentifierExpression*>(expr.get());
    ASSERT_NE_NULL(ident);
    ASSERT_EQ("x", ident->name);
}

// Test: Parse binary expression
void test_parse_binary_expression() {
    std::string source = "1 + 2";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    auto* bin = dynamic_cast<BinaryExpression*>(expr.get());
    ASSERT_NE_NULL(bin);
    ASSERT_EQ(TokenType::OP_PLUS, bin->operator_type);
}

// Test: Parse unary expression
void test_parse_unary_expression() {
    std::string source = "-42";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    auto* unary = dynamic_cast<UnaryExpression*>(expr.get());
    ASSERT_NE_NULL(unary);
    ASSERT_EQ(TokenType::OP_MINUS, unary->operator_type);
}

// Test: Parse logical AND
void test_parse_logical_and() {
    std::string source = "true && false";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    auto* bin = dynamic_cast<BinaryExpression*>(expr.get());
    ASSERT_NE_NULL(bin);
    ASSERT_EQ(TokenType::OP_LOGICAL_AND, bin->operator_type);
}

// Test: Parse logical OR
void test_parse_logical_or() {
    std::string source = "true || false";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    auto* bin = dynamic_cast<BinaryExpression*>(expr.get());
    ASSERT_NE_NULL(bin);
    ASSERT_EQ(TokenType::OP_LOGICAL_OR, bin->operator_type);
}

// Test: Parse logical NOT
void test_parse_logical_not() {
    std::string source = "!true";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    auto* unary = dynamic_cast<UnaryExpression*>(expr.get());
    ASSERT_NE_NULL(unary);
    ASSERT_EQ(TokenType::OP_LOGICAL_NOT, unary->operator_type);
}

// Test: Parse comparison
void test_parse_comparison() {
    std::string source = "x < y";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    auto* bin = dynamic_cast<BinaryExpression*>(expr.get());
    ASSERT_NE_NULL(bin);
    ASSERT_EQ(TokenType::OP_LESS, bin->operator_type);
}

// Test: Parse equality
void test_parse_equality() {
    std::string source = "x == y";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    auto* bin = dynamic_cast<BinaryExpression*>(expr.get());
    ASSERT_NE_NULL(bin);
    ASSERT_EQ(TokenType::OP_EQUAL, bin->operator_type);
}

// Test: Parse function call
void test_parse_function_call() {
    std::string source = "foo(1, 2, 3)";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    auto* call = dynamic_cast<CallExpression*>(expr.get());
    ASSERT_NE_NULL(call);
    ASSERT_EQ(3u, call->arguments.size());
}

// Test: Parse member access
void test_parse_member_access() {
    std::string source = "obj.member";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    auto* member = dynamic_cast<MemberExpression*>(expr.get());
    ASSERT_NE_NULL(member);
    ASSERT_EQ("member", member->member);
}

// Test: Parse array access
void test_parse_array_access() {
    std::string source = "arr[0]";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    auto* array = dynamic_cast<ArrayAccessExpression*>(expr.get());
    ASSERT_NE_NULL(array);
}

// Test: Parse parenthesized expression
void test_parse_parenthesized_expression() {
    std::string source = "(1 + 2) * 3";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    // Should be a multiplication with parenthesized addition
    auto* bin = dynamic_cast<BinaryExpression*>(expr.get());
    ASSERT_NE_NULL(bin);
    ASSERT_EQ(TokenType::OP_MULTIPLY, bin->operator_type);
}

// Test: Parse expression statement
void test_parse_expression_statement() {
    std::string source = "x = 42;";
    Parser parser = createParser(source);
    
    auto stmt = parser.parseExpressionStatement();
    ASSERT_NE_NULL(stmt);
}

// Test: Parse return statement
void test_parse_return_statement() {
    std::string source = "return 42;";
    Parser parser = createParser(source);
    
    auto stmt = parser.parseReturn();
    ASSERT_NE_NULL(stmt);
}

// Test: Parse block
void test_parse_block() {
    std::string source = "{ x = 1; y = 2; }";
    Parser parser = createParser(source);
    
    auto stmt = parser.parseBlock();
    ASSERT_NE_NULL(stmt);
}

// Test: Operator precedence
void test_operator_precedence() {
    // Multiplication should bind tighter than addition
    std::string source = "1 + 2 * 3";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    // Top level should be addition
    auto* top = dynamic_cast<BinaryExpression*>(expr.get());
    ASSERT_NE_NULL(top);
    ASSERT_EQ(TokenType::OP_PLUS, top->operator_type);
    
    // Right side should be multiplication
    auto* right = dynamic_cast<BinaryExpression*>(top->right.get());
    ASSERT_NE_NULL(right);
    ASSERT_EQ(TokenType::OP_MULTIPLY, right->operator_type);
}

// Test: Chained binary operations
void test_chained_binary_operations() {
    std::string source = "1 + 2 + 3 + 4";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
    
    // Should be left-associative: ((1 + 2) + 3) + 4
    auto* bin1 = dynamic_cast<BinaryExpression*>(expr.get());
    ASSERT_NE_NULL(bin1);
    ASSERT_EQ(TokenType::OP_PLUS, bin1->operator_type);
    
    auto* bin2 = dynamic_cast<BinaryExpression*>(bin1->left.get());
    ASSERT_NE_NULL(bin2);
    ASSERT_EQ(TokenType::OP_PLUS, bin2->operator_type);
}

// Test: Complex expression
void test_complex_expression() {
    std::string source = "(a + b) * c - d / e";
    Parser parser = createParser(source);
    
    auto expr = parser.parseExpression();
    ASSERT_NE_NULL(expr);
}

// Test: Parse error recovery
void test_parse_error_recovery() {
    std::string source = "42 + + 10"; // Invalid: double plus
    Parser parser = createParser(source);
    
    try {
        auto expr = parser.parseExpression();
        // Should handle gracefully or throw
    } catch (...) {
        // Expected to throw on invalid input
    }
}

// Run all tests
void run_parser_tests() {
    std::cout << "Running parser tests..." << std::endl;
    
    test_parse_integer_literal();
    test_parse_float_literal();
    test_parse_string_literal();
    test_parse_identifier();
    test_parse_binary_expression();
    test_parse_unary_expression();
    test_parse_logical_and();
    test_parse_logical_or();
    test_parse_logical_not();
    test_parse_comparison();
    test_parse_equality();
    test_parse_function_call();
    test_parse_member_access();
    test_parse_array_access();
    test_parse_parenthesized_expression();
    test_parse_expression_statement();
    test_parse_return_statement();
    test_parse_block();
    test_operator_precedence();
    test_chained_binary_operations();
    test_complex_expression();
    test_parse_error_recovery();
}
