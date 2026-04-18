#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace darija {

// Forward declaration
class Lexer;

// Lexer class - converts source code into tokens
class Lexer {
public:
    // Constructor - initialize with source code
    explicit Lexer(const std::string& source);
    
    // Get next token
    Token nextToken();
    
    // Peek at next token without consuming
    Token peekToken();
    
    // Check if we've reached end of input
    bool isAtEnd() const;
    
    // Reset lexer to beginning
    void reset();
    
    // Get current position
    size_t currentPosition() const;
    
    // Get current line
    size_t currentLine() const;
    
    // Get current column
    size_t currentColumn() const;
    
    // Get source code
    const std::string& getSource() const;
    
private:
    std::string source_;           // Source code
    size_t pos_;                   // Current position in source
    size_t line_;                  // Current line number
    size_t column_;                // Current column number
    Token peeked_token_;           // Cached peeked token
    bool has_peeked_;              // Whether we have a peeked token
    
    // Skip whitespace and comments
    void skipWhitespace();
    
    // Skip single-line comment
    void skipComment();
    
    // Read a string literal
    Token readString();
    
    // Read a character literal
    Token readChar();
    
    // Read a number (integer or float)
    Token readNumber();
    
    // Read an identifier or keyword
    Token readIdentifier();
    
    // Read a single-character token
    Token readSingleCharToken(char c);
    
    // Read a two-character token
    Token readTwoCharToken(char c1, char c2);
    
    // Read an operator
    TokenType readOperator();
    
    // Read a delimiter
    TokenType readDelimiter();
    
    // Identify a keyword
    TokenType identifyKeyword(const std::string& text);
    
    // Identify a type
    TokenType identifyType(const std::string& text);
    
    // Check if current character matches expected
    bool match(char expected);
    
    // Check if current character is at end
    bool isAtEndInternal() const;
    
    // Get current character
    char currentChar() const;
    
    // Advance position
    void advance();
    
    // Peek at next character
    char peekChar() const;
    
    // Check if character is alphabetic (ASCII or Arabic)
    static bool isAlpha(char c);
    
    // Check if character is alphanumeric
    static bool isAlphanumeric(char c);
    
    // Check if character is digit
    static bool isDigit(char c);
    
    // Check if character is whitespace
    static bool isWhitespace(char c);
    
    // Check if character is Arabic letter
    static bool isArabicLetter(unsigned char c, const std::string& source, size_t pos);
    
    // Check if character is valid in identifier
    static bool isIdentifierChar(unsigned char c);
    
    // Keyword map - maps Arabic keywords to token types
    static const std::unordered_map<std::string, TokenType>& keywordMap();
};

} // namespace darija

#endif // LEXER_HPP
