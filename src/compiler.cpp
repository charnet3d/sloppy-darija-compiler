#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "ast/ast.hpp"
#include "x64_codegen.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <windows.h>

using namespace darija;

// Simple AST printer for debugging
void printAST(const ASTNode&, int) {
    // Simplified - full implementation would use visitor pattern
}

// Compiler class
class Compiler {
public:
    Compiler() = default;
    
    // Compile source file to binary
    bool compile(const std::string& input_file, const std::string& output_file);
    
    // Compile source string to assembly
    std::string compileString(const std::string& source);
    
private:
    // Read file contents
    std::string readFile(const std::string& path);
    
    // Package assembly into binary
    bool packageBinary(const std::string& asm_file);
};

// Read file contents
std::string Compiler::readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Compile source string to assembly
std::string Compiler::compileString(const std::string& source) {
    try {
        // Create lexer
        std::cerr << "[COMP] Creating lexer..." << std::endl;
        auto lexer = std::make_unique<Lexer>(source);
        
        // Create parser
        std::cerr << "[COMP] Creating parser..." << std::endl;
        Parser parser(std::move(lexer));
        
        // Parse into AST
        std::cerr << "[COMP] Parsing..." << std::endl;
        std::unique_ptr<ProgramNode> ast;
        try {
            ast = parser.parse();
        } catch (const std::bad_variant_access&) {
            std::cerr << "[COMP] BAD VARIANT ACCESS during parse()" << std::endl;
            throw;
        } catch (const std::exception& ex) {
            std::cerr << "[COMP] EXCEPTION during parse(): " << ex.what() << std::endl;
            throw;
        }
        std::cerr << "[COMP] Parse complete, module=" << ast->module_name << std::endl;
        
    // Generate assembly
    X64CodeGen codegen;
    std::cerr << "[COMP] Generating assembly..." << std::endl;
    std::cerr << "[COMP] About to call accept()..." << std::endl;
    bool accept_done = false;
    try {
        ast->accept(codegen);
        accept_done = true;
    } catch (const std::bad_variant_access&) {
        std::cerr << "[COMP] BAD VARIANT ACCESS during accept() — accept_done=" << accept_done << std::endl;
        throw;
    } catch (const std::bad_cast&) {
        std::cerr << "[COMP] BAD CAST" << std::endl;
        throw;
    } catch (const std::exception& ex) {
        std::cerr << "[COMP] EXCEPTION during accept(): " << ex.what() << std::endl;
        throw;
    }
    std::cerr << "[COMP] Assembly generated successfully" << std::endl;
    return codegen.getAssembly();

        
    } catch (const ParseError& e) {
        std::cerr << "Parse Error at line " << e.line << ", column " << e.column << ": " << e.message << std::endl;
        return "";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }
}

// Package assembly into binary
bool Compiler::packageBinary(const std::string& asm_file) {
    std::string exe_file = asm_file.substr(0, asm_file.find_last_of('.')) + ".exe";
    std::string obj_file = asm_file.substr(0, asm_file.find_last_of('.')) + ".obj";
    std::wstring exe_path;
    DWORD len;
    do {
        exe_path.resize(exe_path.size() + 1024);
        len = GetModuleFileNameW(NULL, exe_path.data(), static_cast<DWORD>(exe_path.size()));
    } while (len == exe_path.size() - 1);
    exe_path.resize(len);
    std::string exe_dir = std::filesystem::path(exe_path).parent_path().string();
    std::string runtime_lib = exe_dir + "/darija-runtime.lib";
    
    // Create a batch file to orchestrate the build
    std::string batch_file = "_build_temp.bat";
    std::ofstream batch(batch_file);
    if (!batch.is_open()) {
        std::cerr << "Cannot create batch file" << std::endl;
        return false;
    }
    
    batch << "@echo off\n";
    batch << "echo === DEBUG: asm=" << asm_file << " obj=" << obj_file << " lib=" << runtime_lib << " exe=" << exe_file << " ===\n";
    batch << "ml64.exe \"" << asm_file << "\" /c\n";
    batch << "if errorlevel 1 (\n";
    batch << "    echo Assembly failed\n";
    batch << "    exit /b 1\n";
    batch << ")\n";
    batch << "link.exe /ENTRY:_start /SUBSYSTEM:CONSOLE \"" << obj_file << "\" \"" << runtime_lib << "\" kernel32.lib user32.lib vcruntime.lib ucrt.lib legacy_stdio_definitions.lib /OUT:" << exe_file << "\n";
    batch << "exit /b %errorlevel%\n";
    batch.close();
    
    // Debug: print batch file content
    std::ifstream dbg(batch_file);
    std::string line;
    std::cout << "=== BATCH FILE CONTENT ===" << std::endl;
    while (std::getline(dbg, line)) {
        std::cout << line << std::endl;
    }
    dbg.close();
    std::cout << "=== END BATCH ===" << std::endl;
    
    std::cout << "Executing Build: " << batch_file << std::endl;
    int result = system(batch_file.c_str());
    
    // Cleanup
    std::filesystem::remove(batch_file);
    
    std::cout << "Build return value: " << result << std::endl;
    return result == 0;
}

// Compile source file to assembly
bool Compiler::compile(const std::string& input_file, const std::string& output_file) {
    try {
        // Read source file
        std::string source = readFile(input_file);
        
        // Compile to assembly
        std::string assembly = compileString(source);
        
        if (assembly.empty()) {
            return false;
        }
        
        // Write assembly to output file
        std::ofstream out(output_file);
        if (!out.is_open()) {
            std::cerr << "Cannot open output file: " << output_file << std::endl;
            return false;
        }
        
        out << assembly;
        out.close();
        
        // Package into binary
        if (!packageBinary(output_file)) {
            return false;
        }
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Compilation error: " << e.what() << std::endl;
        return false;
    }
}

// Print usage information
void printUsage(const char* program_name) {
    std::cerr << "Usage: " << program_name << " [options] <input_file>\n";
    std::cerr << "Options:\n";
    std::cerr << "  -o <file>    Output file (default: output.s)\n";
    std::cerr << "  -t           Test mode - compile from stdin\n";
    std::cerr << "  -h           Show this help message\n";
}

int main(int argc, char* argv[]) {
    std::string input_file;
    std::string output_file = "output.s";
    bool test_mode = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-o" && i + 1 < argc) {
            output_file = argv[++i];
        } else if (arg == "-t" || arg == "--test") {
            test_mode = true;
        } else if (arg[0] != '-') {
            input_file = arg;
        }
    }
    
    Compiler compiler;
    
    if (test_mode) {
        // Test mode - read from stdin
        std::cout << "Enter Darija code (Ctrl+D to compile):\n";
        std::stringstream buffer;
        buffer << std::cin.rdbuf();
        std::string source = buffer.str();
        
        std::string assembly = compiler.compileString(source);
        if (!assembly.empty()) {
            std::cout << "\nGenerated Assembly:\n" << assembly;
        }
    } else if (!input_file.empty()) {
        // Compile file
        if (compiler.compile(input_file, output_file)) {
            std::cout << "Successfully compiled " << input_file << " to " << output_file << std::endl;
            return 0;
        } else {
            std::cerr << "Compilation failed" << std::endl;
            return 1;
        }
    } else {
        printUsage(argv[0]);
        return 1;
    }
    
    return 0;
}
