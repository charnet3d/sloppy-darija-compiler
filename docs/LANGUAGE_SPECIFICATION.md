# Moroccan Arabic Programming Language Specification

## Version 1.0

---

## Table of Contents

1. [Introduction](#introduction)
2. [Language Overview](#language-overview)
3. [Lexical Structure](#lexical-structure)
4. [Type System](#type-system)
5. [Grammar (EBNF)](#grammar-ebnf)
6. [Object-Oriented Programming](#object-oriented-programming)
7. [Memory Model](#memory-model)
8. [Exception Handling](#exception-handling)
9. [File I/O](#file-io)
10. [Standard Library](#standard-library)

---

## Compilation Target
The compiler must take a source file (.darija) and generate a standalone Windows x64 binary. This process must be fully independent, including all necessary linking and runtime support.

This document specifies the Moroccan Arabic Programming Language, a practical application language designed for Moroccan Arabic speakers. The language combines modern programming concepts with Arabic script and Moroccan Arabic (Darija) keywords.

### Design Goals

- **Readability**: Full RTL Arabic script with natural Darija keywords
- **Simplicity**: Easier than C++ while maintaining practical utility
- **Type Safety**: Strong static typing with type inference
- **Modern Features**: OOP, exception handling, automatic memory management

---

## Language Overview

### Example Program

```arabic
// برنامج "مرحبا بالعالم"

// تعريف دالة
دالة greet(نص name): نص
    إلا name == "":
        رجع "مستخدم غير محدد"
    إلا:
        رجع "مرحبا " + name

// الدالة الرئيسية
الرئيسية:
    اطبع greet("أحمد")
```

---

## Lexical Structure

### Keywords

#### Core Language Keywords

| Keyword | English Equivalent | Description |
|---------|-------------------|-------------|
| `وحدة` | module | Module declaration |
| `دالة` | function | Function declaration |
| `الرئيسية` | main | Main entry point |
| `صنف` | class | Class declaration |
| `استورد` | import | Import statement |
| `مكتبة` | library | Library reference |
| `اطبع` | print | Print statement |
| `فراغ` | void | Void type |
| `رجع` | return | Return statement |

#### Control Flow Keywords

| Keyword | English Equivalent | Description |
|---------|-------------------|-------------|
| `إلا` | if | Conditional statement |
| `أولاً` | else | Else clause |
| `عاود ... من ... تال ...` | for | For loop |
| `عاود لكل ... في ...` | for each | For-each loop |
| `ما دام ...` | while | While loop |
| `دير ... ما دام ...` | do-while | Do-while loop |

#### Declaration Keywords

| Keyword | English Equivalent | Description |
|---------|-------------------|-------------|
| `اعرف` | declare | Variable declaration |
| `ثابت` | const | Constant declaration |
| `حول` | cast | Type casting |

#### Exception Handling Keywords

| Keyword | English Equivalent | Description |
|---------|-------------------|-------------|
| `جرب` | try | Try block |
| `شد` | catch | Catch block |
| `كاستثناء` | as exception | Exception type specifier |
| `أخيرا` | finally | Finally block |

### Type Keywords

| Keyword | English Equivalent | Description |
|---------|-------------------|-------------|
| `عدد` | int | 32-bit integer |
| `عدد كبير` | long | 64-bit integer |
| `عدد فاصلة` | float | Single precision float |
| `عدد فاصلة كبير` | double | Double precision float |
| `حقيقة` | bool | Boolean |
| `صحيح` | true | True literal |
| `غلط` | false | False literal |
| `رمز` | char | Character |
| `تسلسل` | array | Array type |
| `شئ` | object | Object type |

### Operators

#### Arithmetic Operators

| Operator | Description |
|----------|-------------|
| `+` | Addition |
| `-` | Subtraction |
| `*` | Multiplication |
| `/` | Division |
| `%` | Modulo |

#### Comparison Operators

| Operator | Description |
|----------|-------------|
| `==` | Equal |
| `!=` | Not equal |
| `<` | Less than |
| `>` | Greater than |
| `<=` | Less than or equal |
| `>=` | Greater than or equal |

#### Logical Operators

| Operator | Description |
|----------|-------------|
| `&&` | Logical AND |
| `||` | Logical OR |
| `!` | Logical NOT |

#### Assignment Operators

| Operator | Description |
|----------|-------------|
| `=` | Assignment |
| `+=` | Add and assign |
| `-=` | Subtract and assign |
| `*=` | Multiply and assign |
| `/=` | Divide and assign |
| `%=` | Modulo and assign |

### Comments

```arabic
// تعليق سطر واحد

/// تعليق متعدد الأسطر
/// يستمر هنا
/// وينتهي هنا
```

### Identifiers

- Must start with an Arabic letter or underscore
- Can contain Arabic letters, numbers, and underscores
- Case-sensitive
- Cannot be a reserved keyword

---

## Type System

### Primitive Types

| Type | Size | Range/Description |
|------|------|------------------|
| `عدد` | 32-bit | -2,147,483,648 to 2,147,483,647 |
| `عدد كبير` | 64-bit | -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807 |
| `عدد فاصلة` | 32-bit | Single precision IEEE 754 |
| `عدد فاصلة كبير` | 64-bit | Double precision IEEE 754 |
| `حقيقة` | 1-bit | true or false |
| `رمز` | 16-bit | Unicode character |

### Type Inference

```arabic
// Type inference with `اعرف`
اعرف x = 42          // x is inferred as `عدد`
اعرف name = "أحمد"   // name is inferred as `نص`
اعرف pi = 3.14       // pi is inferred as `عدد فاصلة كبير`
```

### Type Casting

```arabic
// Explicit type casting
اعرف num = حول<عدد>(3.14)  // Cast float to int
اعرف str = حول<نص>(42)     // Cast int to string
```

---

## Grammar (EBNF)

### Program Structure

```ebnf
program         → module_declaration {import_statement} {type_declaration} {function_declaration} main_function

module_declaration → "وحدة" identifier "{" program_body "}"
program_body      → {statement}

import_statement → "استورد" identifier "من" "مكتبة" path ";"
path              → string_literal
```

### Main Function

```ebnf
main_function   → "الرئيسية" ":" main_body
main_body       → {statement}
```

### Function Declaration

```ebnf
function_declaration → "دالة" identifier parameter_list ":" return_type function_body
parameter_list      → "(" {parameter} ")"
parameter           → identifier ":" type
return_type         → type | "فراغ"
function_body       → "{" statement_list "}"
statement_list      → {statement}
```

### Variable Declaration

```ebnf
variable_declaration → "اعرف" identifier [":" type] ["=" expression] ";"
constant_declaration → "ثابت" identifier ":" type "=" expression ";"
```

### Control Flow

```ebnf
if_statement    → "إلا" "(" condition ")" block [else_clause]
else_clause     → "أولاً" block | "أولاً" "إلا" "(" condition ")" block else_clause

for_loop        → "عاود" "(" init_statement condition_statement increment_statement ")" block
init_statement  → variable_declaration | expression ";"
condition_statement → condition ";"
increment_statement → expression ";"

for_each_loop   → "عاود" "لكل" identifier "في" expression block

while_loop      → "ما دام" "(" condition ")" block

do_while_loop   → "دير" block "ما دام" "(" condition ")" ";"
```

### Exception Handling

```ebnf
try_statement   → "جرب" try_block catch_block [finally_block]
try_block       → block
catch_block     → "شد" "(" identifier "كاستثناء" ")" block
finally_block   → "أخيرا" block
```

### Expressions

```ebnf
expression      → assignment_expression
assignment_expression → identifier "=" expression | conditional_expression
conditional_expression → logical_or_expression ["?" expression ":" expression]
logical_or_expression → logical_and_expression {"||" logical_and_expression}
logical_and_expression → equality_expression {"&&" equality_expression}
equality_expression    → relational_expression {("==" | "!=") relational_expression}
relational_expression  → additive_expression {("<" | ">" | "<=" | ">=") additive_expression}
additive_expression    → multiplicative_expression {("+" | "-") multiplicative_expression}
multiplicative_expression → unary_expression {("*" | "/" | "%") unary_expression}
unary_expression   → "!" unary_expression | "-" unary_expression | postfix_expression
postfix_expression → primary_expression ["++" | "--"]
primary_expression → literal | identifier | "(" expression ")" | function_call | member_access

literal           → integer_literal | float_literal | string_literal | char_literal | boolean_literal
integer_literal   → digit {digit}
float_literal     → digit {digit} "." digit {digit}
string_literal    → "\"" {character} "\""
char_literal      → "'" character "'"
boolean_literal   → "صحيح" | "غلط"
```

### Class Declaration

```ebnf
class_declaration → "صنف" identifier "{" class_body "}"
class_body        → {member_declaration}
member_declaration → field_declaration | method_declaration | constructor_declaration
field_declaration → [access_modifier] type identifier ["=" expression] ";"
method_declaration → [access_modifier] "دالة" identifier parameter_list ":" return_type block
constructor_declaration → "دالة" identifier parameter_list block
access_modifier   → "عام" | "خاص" | "محمي"
```

### Statements

```ebnf
statement         → expression_statement | declaration_statement | control_statement | return_statement | block
expression_statement → expression ";"
declaration_statement → variable_declaration | constant_declaration
control_statement   → if_statement | for_loop | for_each_loop | while_loop | do_while_loop | try_statement
return_statement    → "رجع" [expression] ";"
block               → "{" statement_list "}"
```

---

## Object-Oriented Programming

### Class Definition

```arabic
صنف Calculator
    // حقول
    عدد sum
    عدد product
    
    // بناء
    دالة Calculator():
        this.sum = 0
        this.product = 0
    
    // طرق
    دالة add(عدد a, عدد b): عدد
        this.sum = a + b
        رجع this.sum
    
    دالة multiply(عدد a, عدد b): عدد
        this.product = a * b
        رجع this.product
```

### Access Modifiers

| Modifier | Arabic | Description |
|----------|--------|-------------|
| public | `عام` | Accessible from anywhere |
| private | `خاص` | Accessible only within class |
| protected | `محمي` | Accessible within class and subclasses |

### Inheritance

```arabic
صنف Animal
    نص name
    
    دالة speak(): فراغ
        اطبع "Animal sound"

صنف Dog يرث Animal
    دالة speak(): فراغ
        اطبع "Woof!"
```

---

## Memory Model

### Automatic Memory Management

The language uses automatic memory management with garbage collection:

```arabic
// Objects are automatically managed
دالة createObject(): شئ
    شئ obj = new Object()
    رجع obj  // Object will be garbage collected when no longer referenced
```

### Stack vs Heap

- **Stack**: Local variables, function parameters
- **Heap**: Objects, arrays, dynamically allocated memory

---

## Exception Handling

### Try-Catch-Finally

```arabic
الرئيسية:
    جرب
        اعرف result = divide(10, 0)
        اطبع result
    شد (e كاستثناء)
        اطبع "خطأ: " + e.message
    أخيرا
        اطبع "تم التنفيذ"
```

### Built-in Exceptions

| Exception | Description |
|-----------|-------------|
| `استثناء_تقسيم_صفر` | Division by zero |
| `استثناء_فهرس_خارج_النطاق` | Index out of bounds |
| `استثناء_تحويل_غير_صالح` | Invalid cast |
| `استثناء_ملف_غير_موجود` | File not found |

---

## File I/O

### Reading Files

```arabic
الرئيسية:
    جرب
        نص content = فتح("data.txt", "r")
        اطبع content
    شد (e كاستثناء)
        اطبع "خطأ في قراءة الملف"
```

### Writing Files

```arabic
الرئيسية:
    جرب
        كتابة("output.txt", "مرحبا بالعالم")
    شد (e كاستثناء)
        اطبع "خطأ في كتابة الملف"
```

---

## Standard Library

### Math Library

```arabic
استورد math من مكتبة/standard

الرئيسية:
    اطبع math.sqrt(16)      // 4.0
    اطبع math.pow(2, 3)     // 8.0
    اطبع math.abs(-5)       // 5
```

### String Library

```arabic
استورد string من مكتبة/standard

الرئيسية:
    نص s = "مرحبا"
    اطبع string.length(s)   // 5
    اطبع string.upper(s)    // "مرحبا" (uppercase)
```

---

## Appendix: Complete Keyword Reference

### Core Keywords

```
وحدة، دالة، الرئيسية، صنف، استورد، مكتبة، اطبع، فراغ، رجع
```

### Control Flow

```
إلا، أولاً، عاود، من، تال، لكل، في، ما دام، دير
```

### Declarations

```
اعرف، ثابت، حول
```

### Exception Handling

```
جرب، شد، كاستثناء، أخيرا
```

### Types

```
عدد، عدد كبير، عدد فاصلة، عدد فاصلة كبير، حقيقة، صحيح، غلط، رمز، تسلسل، شئ
```
