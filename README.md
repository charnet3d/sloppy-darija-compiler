# Darija Programming Language Compiler

A modern programming language based on Moroccan Arabic (Darija) dialect, written in Arabic script with minimal Latin letters in keywords. The language is designed to be similar to modern C++ but simpler and more beginner-friendly.

## Features

- **Arabic Script Syntax**: Keywords and identifiers in Arabic/Darija
- **Modern C++-like Features**: Classes, inheritance, functions, templates
- **Beginner-Friendly**: Simplified syntax and clear error messages
- **Type Safety**: Static typing with type inference
- **Memory Management**: Automatic memory management with optional manual control
- **Standard Library**: Rich standard library for common tasks

## Language Keywords

### Control Flow
| Arabic | Meaning | Equivalent |
|--------|---------|------------|
| إلا | if | if |
| إلا إذا | else if | else if |
| غير | else | else |
| عاود | for | for |
| لكل | each | (for-each) |
| في | in | in |
| ما دام | while | while |
| دير | do | do |

### Declarations
| Arabic | Meaning | Equivalent |
|--------|---------|------------|
| اعرف | declare | var/let |
| ثابت | constant | const |
| دالة | function | function |
| صنف | class | class |
| الرئيسية | main | main |
| استورد | import | import |
| من مكتبة | from library | from |

### Operators
| Arabic | Meaning | Equivalent |
|--------|---------|------------|
| صحيح | true | true |
| غلط | false | false |
| كاستثناء | as exception | as |
| يمدد | extends | extends |

### Statements
| Arabic | Meaning | Equivalent |
|--------|---------|------------|
| رجع | return | return |
| اطبع | print | print |
| جرب | try | try |
| شد | catch | catch |
| أخيراً | finally | finally |

## Project Structure

```
darija-compiler/
├── src/
│   ├── lexer/
│   │   ├── token.hpp       # Token definitions
│   │   └── lexer.cpp       # Lexer implementation
│   ├── parser/
│   │   ├── parser.hpp      # Parser interface
│   │   └── parser.cpp      # Parser implementation
│   ├── ast/
│   │   └── ast.hpp         # AST node definitions
│   └── compiler.cpp        # Main compiler entry point
├── tests/
│   ├── test_lexer.cpp      # Lexer unit tests
│   ├── test_parser.cpp     # Parser unit tests
│   └── test_ast.cpp        # AST unit tests
├── examples/
│   ├── hello_world.darija  # Hello World example
│   ├── variables.darija    # Variables and types
│   ├── control_flow.darija # Control flow examples
│   ├── functions.darija    # Function examples
│   └── classes.darija      # Class examples
├── CMakeLists.txt          # Build configuration
└── README.md               # This file
```

## Building

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)

### Build Commands

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# Run tests
cmake --build . --target unit_tests
./unit_tests
```

## Usage

### Compiling a Program

```bash
# Basic compilation
./darija-compiler program.darija

# Specify output file
./darija-compiler program.darija -o output.s

# Test mode (compile from stdin)
./darija-compiler -t
```

### Example Program

```darija
module hello {
    import io from "standard/io";
    
    الرئيسية:
        اطبع "مرحبا بالعالم!";
}
```

## Type System

### Primitive Types

| Type | Size | Description |
|------|------|-------------|
| int | 32-bit | Signed integer |
| long | 64-bit | Signed long integer |
| float | 32-bit | Floating point |
| double | 64-bit | Double precision float |
| bool | 1-bit | Boolean (صحيح/غلط) |
| char | 8-bit | Character |
| string | variable | String |
| array | variable | Array |
| object | variable | Object reference |
| void | - | No type |

### Type Operators

```darija
// Array type
int[] numbers;      // Array of integers
string[] names;     // Array of strings

// Multi-dimensional arrays
int[][] matrix;     // 2D array
```

## Sample Programs

See the `examples/` directory for complete sample programs:

1. **hello_world.darija** - Basic Hello World program
2. **variables_and_types.darija** - Variable declarations and type system
3. **control_flow.darija** - If statements, loops, and control flow
4. **functions.darija** - Function declarations and recursion
5. **classes.darija** - Classes, inheritance, and OOP

## Design Philosophy

1. **Accessibility**: Make programming accessible to Arabic speakers
2. **Simplicity**: Reduce complexity compared to C++ while maintaining power
3. **Modern Features**: Include modern language features from the start
4. **Safety**: Memory safety and type safety by default
5. **Performance**: Compile to native code for good performance

## License

MIT License - See LICENSE file for details.

## Contributing

Contributions are welcome! Please read the contributing guidelines before submitting pull requests.
