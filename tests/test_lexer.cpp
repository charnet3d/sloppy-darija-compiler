#include "../src/lexer/lexer.hpp"
#include "test_common.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <cmath>

using namespace darija;
using namespace darija_tests;

// Test: Integer literal
void test_integer_literal() {
    std::string source = "42";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::LITERAL_INTEGER, token.type);
    ASSERT_EQ(42, std::get<int64_t>(token.value));
}

// Test: Float literal
void test_float_literal() {
    std::string source = "3.14";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::LITERAL_FLOAT, token.type);
    ASSERT_TRUE(std::abs(std::get<double>(token.value) - 3.14) < 0.0001);
}

// Test: String literal
void test_string_literal() {
    std::string source = "\"مرحبا\"";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::LITERAL_STRING, token.type);
}

// Test: Identifier
void test_identifier() {
    std::string source = "myVariable";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::LITERAL_IDENTIFIER, token.type);
}

// Test: Plus operator
void test_plus_operator() {
    std::string source = "+";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_PLUS, token.type);
}

// Test: Minus operator
void test_minus_operator() {
    std::string source = "-";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_MINUS, token.type);
}

// Test: Equal operator
void test_equal_operator() {
    std::string source = "==";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_EQUAL, token.type);
}

// Test: Left parenthesis
void test_left_parenthesis() {
    std::string source = "(";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::DELIM_LPAREN, token.type);
}

// Test: Right parenthesis
void test_right_parenthesis() {
    std::string source = ")";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::DELIM_RPAREN, token.type);
}

// Test: Left brace
void test_left_brace() {
    std::string source = "{";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::DELIM_LBRACE, token.type);
}

// Test: Right brace
void test_right_brace() {
    std::string source = "}";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::DELIM_RBRACE, token.type);
}

// Test: Semicolon
void test_semicolon() {
    std::string source = ";";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::DELIM_SEMICOLON, token.type);
}

// Test: Comma
void test_comma() {
    std::string source = ",";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::DELIM_COMMA, token.type);
}

// Test: EOF token
void test_eof_token() {
    std::string source = "42";
    Lexer lexer(source);
    lexer.nextToken(); // consume the number
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::TOKEN_EOF, token.type);
}

// Test: Whitespace skipping
void test_whitespace_skipping() {
    std::string source = "   42  \t\n  ";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::LITERAL_INTEGER, token.type);
    ASSERT_EQ(42, std::get<int64_t>(token.value));
}

// Test: Single line comment
void test_single_line_comment() {
    std::string source = "// This is a comment\n42";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::LITERAL_INTEGER, token.type);
    ASSERT_EQ(42, std::get<int64_t>(token.value));
}

// Test: Multiple tokens
void test_multiple_tokens() {
    std::string source = "x + y * 2";
    Lexer lexer(source);
    
    Token t1 = lexer.nextToken();
    ASSERT_EQ(TokenType::LITERAL_IDENTIFIER, t1.type);
    
    Token t2 = lexer.nextToken();
    ASSERT_EQ(TokenType::OP_PLUS, t2.type);
    
    Token t3 = lexer.nextToken();
    ASSERT_EQ(TokenType::LITERAL_IDENTIFIER, t3.type);
    
    Token t4 = lexer.nextToken();
    ASSERT_EQ(TokenType::OP_MULTIPLY, t4.type);
    
    Token t5 = lexer.nextToken();
    ASSERT_EQ(TokenType::LITERAL_INTEGER, t5.type);
    ASSERT_EQ(2, std::get<int64_t>(t5.value));
}

// Test: Peek token
void test_peek_token() {
    std::string source = "42 + 10";
    Lexer lexer(source);
    
    Token peeked = lexer.peekToken();
    ASSERT_EQ(TokenType::LITERAL_INTEGER, peeked.type);
    
    Token actual = lexer.nextToken();
    ASSERT_EQ(TokenType::LITERAL_INTEGER, actual.type);
    
    // Peeked token should be returned again
    Token peeked_again = lexer.peekToken();
    ASSERT_EQ(TokenType::OP_PLUS, peeked_again.type);
}

// Test: Reset lexer
void test_reset_lexer() {
    std::string source = "42";
    Lexer lexer(source);
    
    lexer.nextToken(); // consume
    lexer.nextToken(); // EOF
    
    lexer.reset();
    
    Token token = lexer.nextToken();
    ASSERT_EQ(TokenType::LITERAL_INTEGER, token.type);
    ASSERT_EQ(42, std::get<int64_t>(token.value));
}

// Test: Boolean true
void test_boolean_true() {
    std::string source = "صحيح";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::TYPE_TRUE, token.type);
}

// Test: Boolean false
void test_boolean_false() {
    std::string source = "غلط";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::TYPE_FALSE, token.type);
}

// Test: Logical AND
void test_logical_and() {
    std::string source = "&&";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_LOGICAL_AND, token.type);
}

// Test: Logical OR
void test_logical_or() {
    std::string source = "||";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_LOGICAL_OR, token.type);
}

// Test: Logical NOT
void test_logical_not() {
    std::string source = "!";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_LOGICAL_NOT, token.type);
}

// Test: Less than
void test_less_than() {
    std::string source = "<";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_LESS, token.type);
}

// Test: Greater than
void test_greater_than() {
    std::string source = ">";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_GREATER, token.type);
}

// Test: Less than or equal
void test_less_equal() {
    std::string source = "<=";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_LESS_EQUAL, token.type);
}

// Test: Greater than or equal
void test_greater_equal() {
    std::string source = ">=";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_GREATER_EQUAL, token.type);
}

// Test: Not equal
void test_not_equal() {
    std::string source = "!=";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_NOT_EQUAL, token.type);
}

// Test: Assignment
void test_assignment() {
    std::string source = "=";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_ASSIGN, token.type);
}

// Test: Increment
void test_increment() {
    std::string source = "++";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_INCREMENT, token.type);
}

// Test: Decrement
void test_decrement() {
    std::string source = "--";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_DECREMENT, token.type);
}

// Test: Modulo
void test_modulo() {
    std::string source = "%";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_MODULO, token.type);
}

// Test: Division
void test_division() {
    std::string source = "/";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_DIVIDE, token.type);
}

// Test: Multiplication
void test_multiplication() {
    std::string source = "*";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_MULTIPLY, token.type);
}

// Test: Dot
void test_dot() {
    std::string source = ".";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::DELIM_DOT, token.type);
}

// Test: Left bracket
void test_left_bracket() {
    std::string source = "[";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::DELIM_LBRACKET, token.type);
}

// Test: Right bracket
void test_right_bracket() {
    std::string source = "]";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::DELIM_RBRACKET, token.type);
}

// Test: Colon
void test_colon() {
    std::string source = ":";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_COLON, token.type);
}

// Test: Question mark
void test_question_mark() {
    std::string source = "?";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_QUESTION, token.type);
}

// Test: Plus equals
void test_plus_equals() {
    std::string source = "+=";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_PLUS_ASSIGN, token.type);
}

// Test: Minus equals
void test_minus_equals() {
    std::string source = "-=";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_MINUS_ASSIGN, token.type);
}

// Test: Star equals
void test_star_equals() {
    std::string source = "*=";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_MUL_ASSIGN, token.type);
}

// Test: Slash equals
void test_slash_equals() {
    std::string source = "/=";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_DIV_ASSIGN, token.type);
}

// Test: Percent equals
void test_percent_equals() {
    std::string source = "%=";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::OP_MOD_ASSIGN, token.type);
}

// Test: Arrow
void test_arrow() {
    std::string source = "->";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    
    ASSERT_EQ(TokenType::DELIM_ARROW, token.type);
}

void run_lexer_tests() {
    std::cout << "Running lexer tests..." << std::endl;
    
    test_integer_literal();
    test_float_literal();
    test_string_literal();
    test_identifier();
    test_plus_operator();
    test_minus_operator();
    test_equal_operator();
    test_left_parenthesis();
    test_right_parenthesis();
    test_left_brace();
    test_right_brace();
    test_semicolon();
    test_comma();
    test_eof_token();
    test_whitespace_skipping();
    test_single_line_comment();
    test_multiple_tokens();
    test_peek_token();
    test_reset_lexer();
    test_boolean_true();
    test_boolean_false();
    test_logical_and();
    test_logical_or();
    test_logical_not();
    test_less_than();
    test_greater_than();
    test_less_equal();
    test_greater_equal();
    test_not_equal();
    test_assignment();
    test_increment();
    test_decrement();
    test_modulo();
    test_division();
    test_multiplication();
    test_dot();
    test_left_bracket();
    test_right_bracket();
    test_colon();
    test_question_mark();
    test_plus_equals();
    test_minus_equals();
    test_star_equals();
    test_slash_equals();
    test_percent_equals();
    test_arrow();
}
