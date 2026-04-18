#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <variant>
#include <cstdint>
#include <iostream>

namespace darija {

// Token type enumeration
enum class TokenType {
    // End of file
    TOKEN_EOF,
    TOKEN_ERROR,
    
    // Literals
    LITERAL_INTEGER,
    LITERAL_FLOAT,
    LITERAL_STRING,
    LITERAL_CHAR,
    LITERAL_IDENTIFIER,
    
    // Keywords - Control Flow
    KW_IF,           // إلا
    KW_ELSE,         // غير
    KW_ELSE_IF,      // إلا إذا
    KW_FOR,          // عاود
    KW_FOR_EACH,     // لكل
    KW_IN,           // في
    KW_TO,           // تال
    KW_WHILE,        // ما دام
    KW_DO,           // دير
    KW_BREAK,        // وقف
    KW_CONTINUE,     // تابع
    
    // Keywords - Declarations
    KW_VAR,          // اعرف
    KW_DECLARE,      // اعرف
    KW_CONST,        // ثابت
    KW_FUNCTION,     // دالة
    KW_CLASS,        // صنف
    KW_MAIN,         // الرئيسية
    KW_IMPORT,       // استورد
    KW_FROM,         // من
    KW_LIBRARY,      // مكتبة
    KW_MODULE,       // وحدة
    
    // Keywords - Types
    TYPE_INT,        // عدد
    TYPE_LONG,       // عدد_طويل
    TYPE_FLOAT,      // فاصلة
    TYPE_DOUBLE,     // فاصلة_مزدوجة
    TYPE_BOOL,       // منطقي
    TYPE_CHAR,       // حرف
    TYPE_STRING,     // نص
    TYPE_ARRAY,      // مصفوفة
    TYPE_OBJECT,     // كائن
    KW_VOID,         // فارغ
    
    // Keywords - Boolean literals
    TYPE_TRUE,       // صحيح
    TYPE_FALSE,      // غلط
    
    // Keywords - OOP
    KW_EXTENDS,      // يمدد
    KW_PUBLIC,       // عام
    KW_PRIVATE,      // خاص
    KW_PROTECTED,    // محمي
    KW_STATIC,       // ثابت_عام
    KW_NEW,          // جديد
    KW_THIS,         // هذا
    KW_SUPER,        // أب
    
    // Keywords - Exception handling
    KW_TRY,          // جرب
    KW_CATCH,        // شد
    KW_FINALLY,      // أخيراً
    KW_THROW,        // رمى
    KW_AS_EXCEPTION, // كاستثناء
    
    // Keywords - Other
    KW_RETURN,       // رجع
    KW_PRINT,        // اطبع
    KW_CAST,         // كاست
    
    // Delimiters
    DELIM_LPAREN,    // (
    DELIM_RPAREN,    // )
    DELIM_LBRACE,    // {
    DELIM_RBRACE,    // }
    DELIM_LBRACKET,  // [
    DELIM_RBRACKET,  // ]
    DELIM_COMMA,     // ,
    DELIM_SEMICOLON, // ;
    DELIM_DOT,       // .
    DELIM_COLON,     // :
    DELIM_ARROW,     // ->
    
    // Operators - Arithmetic
    OP_PLUS,         // +
    OP_MINUS,        // -
    OP_MULTIPLY,     // *
    OP_DIVIDE,       // /
    OP_MODULO,       // %
    OP_INCREMENT,    // ++
    OP_DECREMENT,    // --
    
    // Operators - Comparison
    OP_EQUAL,        // ==
    OP_NOT_EQUAL,    // !=
    OP_LESS,         // <
    OP_GREATER,      // >
    OP_LESS_EQUAL,   // <=
    OP_GREATER_EQUAL,// >=
    
    // Operators - Logical
    OP_LOGICAL_AND,  // &&
    OP_LOGICAL_OR,   // ||
    OP_LOGICAL_NOT,  // !
    
    // Operators - Assignment
    OP_ASSIGN,       // =
    OP_PLUS_ASSIGN,  // +=
    OP_MINUS_ASSIGN, // -=
    OP_MUL_ASSIGN,   // *=
    OP_DIV_ASSIGN,   // /=
    OP_MOD_ASSIGN,   // %=
    
    // Operators - Other
    OP_QUESTION,     // ?
    OP_COLON,        // :
};

// Token value type - can hold different literal types
using TokenValue = std::variant<int64_t, double, std::string, char, bool>;

// Token structure
struct Token {
    TokenType type;
    TokenValue value;
    size_t line;
    size_t column;
    size_t start_pos;
    size_t end_pos;
    
    Token(TokenType t = TokenType::TOKEN_EOF,
          TokenValue v = int64_t(0),
          size_t l = 0,
          size_t c = 0,
          size_t sp = 0,
          size_t ep = 0)
        : type(t), value(v), line(l), column(c), start_pos(sp), end_pos(ep) {}
    
    // Check if token is a keyword
    bool isKeyword() const {
        switch (type) {
            case TokenType::KW_IF: case TokenType::KW_ELSE: case TokenType::KW_ELSE_IF: 
            case TokenType::KW_FOR: case TokenType::KW_FOR_EACH:
            case TokenType::KW_IN: case TokenType::KW_WHILE: case TokenType::KW_DO: 
            case TokenType::KW_BREAK: case TokenType::KW_CONTINUE:
            case TokenType::KW_VAR: case TokenType::KW_CONST: case TokenType::KW_FUNCTION: 
            case TokenType::KW_CLASS: case TokenType::KW_MAIN:
            case TokenType::KW_IMPORT: case TokenType::KW_FROM: case TokenType::KW_LIBRARY: 
            case TokenType::KW_MODULE:
            case TokenType::TYPE_INT: case TokenType::TYPE_LONG: case TokenType::TYPE_FLOAT: 
            case TokenType::TYPE_DOUBLE:
            case TokenType::TYPE_BOOL: case TokenType::TYPE_CHAR: case TokenType::TYPE_STRING: 
            case TokenType::TYPE_ARRAY:
            case TokenType::TYPE_OBJECT: case TokenType::KW_VOID: case TokenType::TYPE_TRUE: 
            case TokenType::TYPE_FALSE:
            case TokenType::KW_EXTENDS: case TokenType::KW_PUBLIC: case TokenType::KW_PRIVATE: 
            case TokenType::KW_PROTECTED:
            case TokenType::KW_STATIC: case TokenType::KW_NEW: case TokenType::KW_THIS: 
            case TokenType::KW_SUPER:
            case TokenType::KW_TRY: case TokenType::KW_CATCH: case TokenType::KW_FINALLY: 
            case TokenType::KW_THROW:
            case TokenType::KW_AS_EXCEPTION: case TokenType::KW_RETURN: case TokenType::KW_PRINT: 
            case TokenType::KW_CAST:
                return true;
            default:
                return false;
        }
    }
    
    // Check if token is an operator
    bool isOperator() const {
        return type >= TokenType::OP_PLUS && type <= TokenType::OP_COLON;
    }
    
    // Get token type name as string (for debugging)
    std::string typeName() const {
        switch (type) {
            case TokenType::TOKEN_EOF: return "EOF";
            case TokenType::TOKEN_ERROR: return "ERROR";
            case TokenType::LITERAL_INTEGER: return "INTEGER";
            case TokenType::LITERAL_FLOAT: return "FLOAT";
            case TokenType::LITERAL_STRING: return "STRING";
            case TokenType::LITERAL_CHAR: return "CHAR";
            case TokenType::LITERAL_IDENTIFIER: return "IDENTIFIER";
            case TokenType::KW_IF: return "IF";
            case TokenType::KW_ELSE: return "ELSE";
            case TokenType::KW_ELSE_IF: return "ELSE_IF";
            case TokenType::KW_FOR: return "FOR";
            case TokenType::KW_FOR_EACH: return "FOR_EACH";
            case TokenType::KW_IN: return "IN";
            case TokenType::KW_FROM: return "FROM";
            case TokenType::KW_TO: return "TO";
            case TokenType::KW_WHILE: return "WHILE";
            case TokenType::KW_DO: return "DO";
            case TokenType::KW_BREAK: return "BREAK";
            case TokenType::KW_CONTINUE: return "CONTINUE";
            case TokenType::KW_VAR: return "VAR";
            case TokenType::KW_DECLARE: return "DECLARE";
            case TokenType::KW_CONST: return "CONST";
            case TokenType::KW_FUNCTION: return "FUNCTION";
            case TokenType::KW_CLASS: return "CLASS";
            case TokenType::KW_MAIN: return "MAIN";
            case TokenType::KW_IMPORT: return "IMPORT";
            case TokenType::KW_LIBRARY: return "LIBRARY";
            case TokenType::KW_MODULE: return "MODULE";
            case TokenType::TYPE_INT: return "INT";
            case TokenType::TYPE_LONG: return "LONG";
            case TokenType::TYPE_FLOAT: return "FLOAT_TYPE";
            case TokenType::TYPE_DOUBLE: return "DOUBLE";
            case TokenType::TYPE_BOOL: return "BOOL";
            case TokenType::TYPE_CHAR: return "CHAR_TYPE";
            case TokenType::TYPE_STRING: return "STRING_TYPE";
            case TokenType::TYPE_ARRAY: return "ARRAY";
            case TokenType::TYPE_OBJECT: return "OBJECT";
            case TokenType::KW_VOID: return "VOID";
            case TokenType::TYPE_TRUE: return "TRUE";
            case TokenType::TYPE_FALSE: return "FALSE";
            case TokenType::KW_EXTENDS: return "EXTENDS";
            case TokenType::KW_PUBLIC: return "PUBLIC";
            case TokenType::KW_PRIVATE: return "PRIVATE";
            case TokenType::KW_PROTECTED: return "PROTECTED";
            case TokenType::KW_STATIC: return "STATIC";
            case TokenType::KW_NEW: return "NEW";
            case TokenType::KW_THIS: return "THIS";
            case TokenType::KW_SUPER: return "SUPER";
            case TokenType::KW_TRY: return "TRY";
            case TokenType::KW_CATCH: return "CATCH";
            case TokenType::KW_FINALLY: return "FINALLY";
            case TokenType::KW_THROW: return "THROW";
            case TokenType::KW_AS_EXCEPTION: return "AS_EXCEPTION";
            case TokenType::KW_RETURN: return "RETURN";
            case TokenType::KW_PRINT: return "PRINT";
            case TokenType::KW_CAST: return "CAST";
            case TokenType::DELIM_LPAREN: return "LPAREN";
            case TokenType::DELIM_RPAREN: return "RPAREN";
            case TokenType::DELIM_LBRACE: return "LBRACE";
            case TokenType::DELIM_RBRACE: return "RBRACE";
            case TokenType::DELIM_LBRACKET: return "LBRACKET";
            case TokenType::DELIM_RBRACKET: return "RBRACKET";
            case TokenType::DELIM_COMMA: return "COMMA";
            case TokenType::DELIM_SEMICOLON: return "SEMICOLON";
            case TokenType::DELIM_DOT: return "DOT";
            case TokenType::DELIM_COLON: return "COLON";
            case TokenType::DELIM_ARROW: return "ARROW";
            case TokenType::OP_PLUS: return "+";
            case TokenType::OP_MINUS: return "-";
            case TokenType::OP_MULTIPLY: return "*";
            case TokenType::OP_DIVIDE: return "/";
            case TokenType::OP_MODULO: return "%";
            case TokenType::OP_INCREMENT: return "++";
            case TokenType::OP_DECREMENT: return "--";
            case TokenType::OP_EQUAL: return "==";
            case TokenType::OP_NOT_EQUAL: return "!=";
            case TokenType::OP_LESS: return "<";
            case TokenType::OP_GREATER: return ">";
            case TokenType::OP_LESS_EQUAL: return "<=";
            case TokenType::OP_GREATER_EQUAL: return ">=";
            case TokenType::OP_LOGICAL_AND: return "&&";
            case TokenType::OP_LOGICAL_OR: return "||";
            case TokenType::OP_LOGICAL_NOT: return "!";
            case TokenType::OP_ASSIGN: return "=";
            case TokenType::OP_PLUS_ASSIGN: return "+=";
            case TokenType::OP_MINUS_ASSIGN: return "-=";
            case TokenType::OP_MUL_ASSIGN: return "*=";
            case TokenType::OP_DIV_ASSIGN: return "/=";
            case TokenType::OP_MOD_ASSIGN: return "%=";
            case TokenType::OP_QUESTION: return "?";
            case TokenType::OP_COLON: return ":";
            default: return "UNKNOWN";
        }
    }
};

// Stream operator for TokenType
inline std::ostream& operator<<(std::ostream& os, TokenType type) {
    os << static_cast<int>(type);
    return os;
}

} // namespace darija

#endif // TOKEN_HPP
