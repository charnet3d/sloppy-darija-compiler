#include "lexer.hpp"
#include <cctype>
#include <stdexcept>
#include <algorithm>

namespace darija {

// Constructor
Lexer::Lexer(const std::string& source)
    : source_(source), pos_(0), line_(1), column_(1), has_peeked_(false) {
    // Skip UTF-8 BOM if present
    if (source_.size() >= 3 && 
        static_cast<unsigned char>(source_[0]) == 0xEF &&
        static_cast<unsigned char>(source_[1]) == 0xBB &&
        static_cast<unsigned char>(source_[2]) == 0xBF) {
        pos_ = 3;
    }
}

// Advance position
void Lexer::advance() {
    if (pos_ < source_.length()) {
        if (source_[pos_] == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        pos_++;
    }
}

// Skip whitespace
void Lexer::skipWhitespace() {
    while (pos_ < source_.length() && 
           (source_[pos_] == ' ' || source_[pos_] == '\t' || 
            source_[pos_] == '\n' || source_[pos_] == '\r')) {
        advance();
    }
}

// Skip comment
void Lexer::skipComment() {
    if (pos_ + 1 < source_.length() && source_[pos_] == '/' && source_[pos_ + 1] == '/') {
        // Single line comment
        while (pos_ < source_.length() && source_[pos_] != '\n') {
            advance();
        }
    } else if (pos_ + 2 < source_.length() && source_[pos_] == '/' && 
               source_[pos_ + 1] == '/' && source_[pos_ + 2] == '/') {
        // Multi-line comment
        while (pos_ + 2 < source_.length() && 
               !(source_[pos_] == '/' && source_[pos_ + 1] == '/' && source_[pos_ + 2] == '/')) {
            advance();
        }
        if (pos_ + 2 < source_.length()) {
            pos_ += 3; // Skip closing ///
        }
    }
}

// Check if character is Arabic letter (UTF-8 encoded)
bool Lexer::isArabicLetter(unsigned char c, const std::string& source, size_t pos) {
    // Check if this is a UTF-8 lead byte for a 2-byte sequence
    if ((c >= 0xC2 && c <= 0xDF) && pos + 1 < source.length()) {
        unsigned char b1 = static_cast<unsigned char>(source[pos + 1]);
        if ((b1 & 0xC0) == 0x80) {
            // Decode UTF-8 2-byte sequence
            uint32_t cp = ((c & 0x1F) << 6) | (b1 & 0x3F);
            // Check Arabic blocks for letters only (not punctuation)
            if (cp >= 0x0621 && cp <= 0x063A) return true; // Arabic Letter Range
            if (cp >= 0x0641 && cp <= 0x064A) return true; // Arabic Letter Range
            if (cp >= 0x066E && cp <= 0x066F) return true; // Arabic Letter Yeh Arabic End
            if (cp >= 0x0671 && cp <= 0x06D3) return true; // Arabic Letter Alef Wasla to Yeh
            if (cp >= 0x06EE && cp <= 0x06EF) return true; // Arabic Letter Dal
            if (cp >= 0x06FA && cp <= 0x06FC) return true; // Arabic Letter Feh
            if (cp >= 0x06FE && cp <= 0x06FF) return true; // Arabic Letter Qaf
            if (cp >= 0x0750 && cp <= 0x077F) return true; // Arabic Supplement Letters
            if (cp >= 0x08A0 && cp <= 0x08FF) return true; // Arabic Extended-A Letters
            return false;
        }
    }
    return false;
}

// Check if character is valid in identifier
bool Lexer::isIdentifierChar(unsigned char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || (c >= 0x80 && c <= 0xBF);
}

// Check if character is digit
bool Lexer::isDigit(char c) {
    return std::isdigit(static_cast<unsigned char>(c));
}

// Check if character is alphabetic (ASCII or Arabic)
bool Lexer::isAlpha(char c) {
    return std::isalpha(static_cast<unsigned char>(c));
}

// Check if character is alphanumeric
bool Lexer::isAlphanumeric(char c) {
    return std::isalnum(static_cast<unsigned char>(c));
}

// Check if character is whitespace
bool Lexer::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// Check if we've reached end of input
bool Lexer::isAtEnd() const {
    return pos_ >= source_.length();
}

// Get current position
size_t Lexer::currentPosition() const {
    return pos_;
}

// Get current line
size_t Lexer::currentLine() const {
    return line_;
}

// Get current column
size_t Lexer::currentColumn() const {
    return column_;
}

// Get source code
const std::string& Lexer::getSource() const {
    return source_;
}

// Check if current character matches expected
bool Lexer::match(char expected) {
    if (isAtEndInternal()) return false;
    if (source_[pos_] != expected) return false;
    advance();
    return true;
}

// Read an operator
TokenType Lexer::readOperator() {
    char c = currentChar();
    advance();

    switch (c) {
        case '+': 
            if (match('+')) return TokenType::OP_INCREMENT;
            if (match('=')) return TokenType::OP_PLUS_ASSIGN;
            return TokenType::OP_PLUS;
        case '-': 
            if (match('>')) return TokenType::DELIM_ARROW;
            if (match('-')) return TokenType::OP_DECREMENT;
            if (match('=')) return TokenType::OP_MINUS_ASSIGN;
            return TokenType::OP_MINUS;
        case '*': 
            if (match('=')) return TokenType::OP_MUL_ASSIGN;
            return TokenType::OP_MULTIPLY;
        case '/': 
            if (match('=')) return TokenType::OP_DIV_ASSIGN;
            return TokenType::OP_DIVIDE;
        case '%': 
            if (match('=')) return TokenType::OP_MOD_ASSIGN;
            return TokenType::OP_MODULO;
        case '=': 
            if (match('=')) return TokenType::OP_EQUAL;
            return TokenType::OP_ASSIGN;
        case '<': 
            if (match('=')) return TokenType::OP_LESS_EQUAL;
            return TokenType::OP_LESS;
        case '>': 
            if (match('=')) return TokenType::OP_GREATER_EQUAL;
            return TokenType::OP_GREATER;
        case '!': 
            if (match('=')) return TokenType::OP_NOT_EQUAL;
            return TokenType::OP_LOGICAL_NOT;
        case '&': 
            if (match('&')) return TokenType::OP_LOGICAL_AND;
            return TokenType::TOKEN_ERROR;
        case '|': 
            if (match('|')) return TokenType::OP_LOGICAL_OR;
            return TokenType::TOKEN_ERROR;
        case '?': return TokenType::OP_QUESTION;
        case ':': return TokenType::OP_COLON;
        default: return TokenType::TOKEN_ERROR;
    }
}

// Read a delimiter
TokenType Lexer::readDelimiter() {
    char c = currentChar();
    advance();

    switch (c) {
        case '(': return TokenType::DELIM_LPAREN;
        case ')': return TokenType::DELIM_RPAREN;
        case '{': return TokenType::DELIM_LBRACE;
        case '}': return TokenType::DELIM_RBRACE;
        case '[': return TokenType::DELIM_LBRACKET;
        case ']': return TokenType::DELIM_RBRACKET;
        case ',': return TokenType::DELIM_COMMA;
        case ';': return TokenType::DELIM_SEMICOLON;
        case '.': return TokenType::DELIM_DOT;
        case ':': return TokenType::DELIM_COLON;
        default: return TokenType::TOKEN_ERROR;
    }
}

// Check if current character is at end
bool Lexer::isAtEndInternal() const {
    return pos_ >= source_.length();
}

// Get current character
char Lexer::currentChar() const {
    if (isAtEndInternal()) return '\0';
    return source_[pos_];
}

// Peek at next character
char Lexer::peekChar() const {
    if (pos_ + 1 >= source_.length()) return '\0';
    return source_[pos_ + 1];
}

// Reset lexer
void Lexer::reset() {
    pos_ = 0;
    line_ = 1;
    column_ = 1;
    has_peeked_ = false;
}

// Get next token
Token Lexer::nextToken() {
    if (has_peeked_) {
        has_peeked_ = false;
        return peeked_token_;
    }
    
    size_t start_pos = pos_;
    size_t line = line_;
    size_t column = column_;
    
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::TOKEN_EOF, std::string(""), line_, column_);
    }
    
    // Check for comments
    if (source_[pos_] == '/' && pos_ + 1 < source_.length() && source_[pos_ + 1] == '/') {
        skipComment();
        return nextToken();
    }
    
    char c = source_[pos_];
    
    // Operators
    if (std::string("+-*/%=<>!&|?:").find(c) != std::string::npos) {
        return Token(readOperator(), int64_t(0), line, column, start_pos, pos_);
    }
    
    // Delimiters
    if (std::string("()[]{}.,;").find(c) != std::string::npos) {
        return Token(readDelimiter(), int64_t(0), line, column, start_pos, pos_);
    }
    
    // String literal
    if (c == '"') {
        advance(); // Skip opening delimiter
        std::string result;
        while (pos_ < source_.length() && source_[pos_] != '"') {
            if (source_[pos_] == '\\') {
                advance(); // Skip backslash
                if (pos_ < source_.length()) {
                    switch (source_[pos_]) {
                        case 'n': result += '\n'; break;
                        case 't': result += '\t'; break;
                        case 'r': result += '\r'; break;
                        case '\\': result += '\\'; break;
                        case '"': result += '"'; break;
                        case '\'' : result += '\''; break;
                        default: result += source_[pos_]; break;
                    }
                }
            } else {
                result += source_[pos_];
            }
            advance();
        }
        if (pos_ < source_.length()) {
            advance(); // Skip closing delimiter
        }
        return Token(TokenType::LITERAL_STRING, result, line, column, start_pos, pos_);
    }
    
    // Character literal
    if (c == '\'') {
        advance(); // Skip opening quote
        char ch;
        if (pos_ < source_.length() && source_[pos_] == '\\') {
            advance();
            switch (source_[pos_]) {
                case 'n': ch = '\n'; break;
                case 't': ch = '\t'; break;
                case 'r': ch = '\r'; break;
                case '\\': ch = '\\'; break;
                case '\'' : ch = '\''; break;
                default: ch = source_[pos_]; break;
            }
        } else {
            ch = pos_ < source_.length() ? source_[pos_] : '\0';
        }
        advance(); // Skip character
        advance(); // Skip closing quote
        return Token(TokenType::LITERAL_CHAR, ch, line, column, start_pos, pos_);
    }
    
    // Number
    if (isDigit(c)) {
        size_t num_start = pos_;
        while (pos_ < source_.length() && isDigit(source_[pos_])) {
            advance();
        }
        // Check for float
        if (pos_ < source_.length() && source_[pos_] == '.') {
            advance();
            while (pos_ < source_.length() && isDigit(source_[pos_])) {
                advance();
            }
            std::string num = source_.substr(num_start, pos_ - num_start);
            return Token(TokenType::LITERAL_FLOAT, std::stod(num), line, column, start_pos, pos_);
        }
        std::string num = source_.substr(num_start, pos_ - num_start);
        return Token(TokenType::LITERAL_INTEGER, std::stoll(num), line, column, start_pos, pos_);
    }
    
    // Identifier or keyword
    if (isIdentifierChar(c) || isArabicLetter(static_cast<unsigned char>(c), source_, pos_)) {
        size_t ident_start = pos_;
        while (pos_ < source_.length() && (isIdentifierChar(static_cast<unsigned char>(source_[pos_])) || isArabicLetter(static_cast<unsigned char>(source_[pos_]), source_, pos_))) {
            advance();
        }
        std::string ident = source_.substr(ident_start, pos_ - ident_start);
        
        // Check if it's a keyword
        const auto& keywords = keywordMap();
        auto it = keywords.find(ident);
        if (it != keywords.end()) {
            return Token(it->second, ident, line, column, start_pos, pos_);
        }
        
        // Check if it's a type
        static const std::unordered_map<std::string, TokenType> types = {
            {"عدد", TokenType::TYPE_INT},
            {"عدد كبير", TokenType::TYPE_LONG},
            {"عدد فاصلة", TokenType::TYPE_FLOAT},
            {"عدد فاصلة كبير", TokenType::TYPE_DOUBLE},
            {"حقيقة", TokenType::TYPE_BOOL},
            {"صحيح", TokenType::TYPE_TRUE},
            {"غلط", TokenType::TYPE_FALSE},
            {"رمز", TokenType::TYPE_CHAR},
            {"تسلسل", TokenType::TYPE_ARRAY},
            {"شئ", TokenType::TYPE_OBJECT},
        };
        
        it = types.find(ident);
        if (it != types.end()) {
            return Token(it->second, ident, line, column, start_pos, pos_);
        }
        
        return Token(TokenType::LITERAL_IDENTIFIER, ident, line, column, start_pos, pos_);
    }
    
    // Unknown character - skip
    advance();
    return Token(TokenType::TOKEN_ERROR, std::string(""), line, column, start_pos, pos_);
}

// Peek at next token
Token Lexer::peekToken() {
    if (!has_peeked_) {
        peeked_token_ = nextToken();
        has_peeked_ = true;
    }
    return peeked_token_;
}

// Get keyword map
const std::unordered_map<std::string, TokenType>& Lexer::keywordMap() {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"وحدة", TokenType::KW_MODULE},
        {"دالة", TokenType::KW_FUNCTION},
        {"الرئيسية", TokenType::KW_MAIN},
        {"صنف", TokenType::KW_CLASS},
        {"استورد", TokenType::KW_IMPORT},
        {"مكتبة", TokenType::KW_LIBRARY},
        {"اطبع", TokenType::KW_PRINT},
        {"فراغ", TokenType::KW_VOID},
        {"رجع", TokenType::KW_RETURN},
        {"إلا", TokenType::KW_IF},
        {"أولاً", TokenType::KW_ELSE},
        {"عاود", TokenType::KW_FOR},
        {"من", TokenType::KW_FROM},
        {"تال", TokenType::KW_TO},
        {"لكل", TokenType::KW_FOR_EACH},
        {"في", TokenType::KW_IN},
        {"ما دام", TokenType::KW_WHILE},
        {"دير", TokenType::KW_DO},
        {"اعرف", TokenType::KW_DECLARE},
        {"ثابت", TokenType::KW_CONST},
        {"حول", TokenType::KW_CAST},
        {"جرب", TokenType::KW_TRY},
        {"شد", TokenType::KW_CATCH},
        {"كاستثناء", TokenType::KW_AS_EXCEPTION},
        {"أخيرا", TokenType::KW_FINALLY},
        {"يرث", TokenType::KW_EXTENDS},
        {"عام", TokenType::KW_PUBLIC},
        {"خاص", TokenType::KW_PRIVATE},
        {"محمي", TokenType::KW_PROTECTED},
    };
    return keywords;
}

} // namespace darija
