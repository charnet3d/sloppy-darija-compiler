#ifndef X64_CODEGEN_HPP
#define X64_CODEGEN_HPP

#include "ast/ast.hpp"
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <variant>

namespace darija {

class X64CodeGen : public Visitor {
public:
    X64CodeGen() : label_count(0), stack_offset(0) {}

    std::string getAssembly() {
        std::stringstream full_asm;
        full_asm << ".data\n";
        full_asm << data_section.str() << "\n";
        full_asm << ".code\n";
        full_asm << "extern print_wrapper:proc\n";
        full_asm << "extern ExitProcess:proc\n";
        full_asm << "; Module: " << module_name << "\n";
        full_asm << "_start PROC\n";
        full_asm << "    call main\n";
        full_asm << "    xor rcx, rcx\n";
        full_asm << "    call ExitProcess\n";
        full_asm << "_start ENDP\n";
        full_asm << "main PROC\n";
        full_asm << code_section.str();
        full_asm << "main ENDP\n";
        full_asm << "END\n";
        return full_asm.str();
    }

    void visit(ProgramNode& node) override {
        module_name = node.module_name;
        
        for (auto& func : node.function_declarations) {
            if (auto* f_node = dynamic_cast<FunctionNode*>(func.get())) {
                visit(*f_node);
            }
        }
        
        if (node.main_function) {
            if (auto* m_node = dynamic_cast<FunctionNode*>(node.main_function.get())) {
                m_node->is_main = true;
                visit(*m_node);
            }
        }
    }

    void visit(FunctionNode& node) override {
        if (node.is_main) {
            // main is handled by the getAssembly wrapper
        } else {
            code_section << node.name << " PROC\n";
        }
        code_section << "    push rbp\n";
        code_section << "    mov rbp, rsp\n";
        
        // Reset symbol table and stack for each function
        symbol_table.clear();
        stack_offset = 0;

        for (auto& stmt : node.body) {
            stmt->accept(*this);
        }
        
        code_section << "    mov rsp, rbp\n";
        code_section << "    pop rbp\n";
        code_section << "    ret\n";
        if (!node.is_main) {
            code_section << node.name << " ENDP\n";
        }
    }

    void visit(VariableNode& node) override {
        try {
            if (node.initializer) {
                node.initializer->accept(*this);
                int64_t offset = allocateStack();
                symbol_table[node.name] = {offset, node.type};
                code_section << "    mov [" << offset << "(rbp)], rax\n";
            } else {
                int64_t offset = allocateStack();
                symbol_table[node.name] = {offset, node.type};
            }
        } catch (const std::bad_variant_access&) {
            std::cerr << "[CODEGEN] BAD VARIANT ACCESS in visit(VariableNode) name=" << node.name << std::endl;
            throw;
        }
    }

    void visit(PrintStatement& node) override {
        try {
            node.expression->accept(*this);
        } catch (const std::bad_variant_access&) {
            std::cerr << "[CODEGEN] BAD VARIANT ACCESS in visit(PrintStatement)" << std::endl;
            throw;
        }
        
        if (auto* lit = dynamic_cast<LiteralExpression*>(node.expression.get())) {
            if (lit->value.index() == 2) {
                try {
                    std::string s = std::get<std::string>(lit->value);
                    code_section << "    mov rcx, rax\n";
                    code_section << "    mov rdx, " << s.size() << "\n";
                    code_section << "    ; Call print_wrapper(rax, rdx)\n";
                    code_section << "    call print_wrapper\n";
                    return;
                } catch (...) {
                    std::cerr << "[CODEGEN] Failed to get string value" << std::endl;
                }
            }
        }
        
        TokenType type = determineType(*node.expression);
        code_section << "    mov rcx, rax\n";
        code_section << "    mov rdx, " << static_cast<int>(type) << "\n";
        code_section << "    ; Call print_wrapper(rax, rdx)\n";
        code_section << "    call print_wrapper\n";
    }

    TokenType determineType(Expression& expr) {
        if (auto* lit = dynamic_cast<LiteralExpression*>(&expr)) {
            return lit->literal_type;
        }
        if (auto* id = dynamic_cast<IdentifierExpression*>(&expr)) {
            if (symbol_table.count(id->name)) {
                return symbol_table[id->name].type;
            }
        }
        if (auto* bin = dynamic_cast<BinaryExpression*>(&expr)) {
            return determineType(*bin->left);
        }
        if (auto* un = dynamic_cast<UnaryExpression*>(&expr)) {
            return determineType(*un->operand);
        }
        if (auto* cast = dynamic_cast<CastExpression*>(&expr)) {
            return cast->target_type;
        }
        if (auto* nexp = dynamic_cast<NewExpression*>(&expr)) {
            return TokenType::TYPE_OBJECT;
        }
        if (auto* call = dynamic_cast<CallExpression*>(&expr)) {
            return TokenType::TYPE_INT; // Default for calls
        }
        if (auto* mem = dynamic_cast<MemberExpression*>(&expr)) {
            return TokenType::TYPE_OBJECT; // Default for member access
        }
        if (auto* arr = dynamic_cast<ArrayAccessExpression*>(&expr)) {
            return TokenType::TYPE_INT; // Default for array access
        }
        return TokenType::TYPE_INT;
    }

    void visit(LiteralExpression& node) override {
        std::cerr << "[CODEGEN] visit(LiteralExpression) literal_type=" << (int)node.literal_type << std::endl;
        try {
            if (std::holds_alternative<int64_t>(node.value)) {
                std::cerr << "[CODEGEN]   variant holds int64_t" << std::endl;
                code_section << "    mov rax, " << std::get<int64_t>(node.value) << "\n";
            } else if (std::holds_alternative<std::string>(node.value)) {
                std::cerr << "[CODEGEN]   variant holds std::string" << std::endl;
                std::string s = std::get<std::string>(node.value);
                std::string label = "str_" + std::to_string(label_count++);
                data_section << label << " db \"" << s << "\", 0\n";
                code_section << "    mov rax, OFFSET " << label << "\n";
            } else if (std::holds_alternative<char>(node.value)) {
                std::cerr << "[CODEGEN]   variant holds char" << std::endl;
                code_section << "    mov rax, " << static_cast<int>(std::get<char>(node.value)) << "\n";
            } else if (std::holds_alternative<double>(node.value)) {
                std::cerr << "[CODEGEN]   variant holds double" << std::endl;
                // For now, convert double to integer for MASM
                code_section << "    mov rax, " << static_cast<int64_t>(std::get<double>(node.value)) << "\n";
            } else if (std::holds_alternative<bool>(node.value)) {
                std::cerr << "[CODEGEN]   variant holds bool" << std::endl;
                code_section << "    mov rax, " << (std::get<bool>(node.value) ? 1 : 0) << "\n";
            } else {
                std::cerr << "[CODEGEN]   variant holds UNKNOWN type, index=" << node.value.index() << std::endl;
                code_section << "    xor rax, rax\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "[CODEGEN] EXCEPTION in visit(LiteralExpression): " << e.what() << std::endl;
            throw;
        }
    }

    void visit(IdentifierExpression& node) override {
        if (symbol_table.find(node.name) == symbol_table.end()) {
            code_section << "    ; Error: undefined variable " << node.name << "\n";
            code_section << "    xor rax, rax\n";
            return;
        }
        int64_t offset = symbol_table[node.name].offset;
        code_section << "    mov rax, [" << offset << "(rbp)]\n";
    }

    void visit(ExpressionStatement& node) override {
        node.expression->accept(*this);
    }

    void visit(BlockStatement& node) override {
        for (auto& stmt : node.statements) {
            stmt->accept(*this);
        }
    }

    void visit(IfStatement& node) override {
        std::string label_else = "else_" + std::to_string(label_count++);
        std::string label_end = "end_if_" + std::to_string(label_count++);
        
        node.condition->accept(*this);
        code_section << "    cmp rax, 0\n";
        code_section << "    je " << label_else << "\n";
        
        for (auto& stmt : node.then_branch) {
            stmt->accept(*this);
        }
        code_section << "    jmp " << label_end << "\n";
        
        code_section << label_else << ":\n";
        for (auto& stmt : node.else_branch) {
            stmt->accept(*this);
        }
        
        code_section << label_end << ":\n";
    }

    void visit(WhileStatement& node) override {
        std::string label_start = "while_start_" + std::to_string(label_count++);
        std::string label_end = "while_end_" + std::to_string(label_count++);
        
        code_section << label_start << ":\n";
        node.condition->accept(*this);
        code_section << "    cmp rax, 0\n";
        code_section << "    je " << label_end << "\n";
        
        for (auto& stmt : node.body) {
            stmt->accept(*this);
        }
        
        code_section << "    jmp " << label_start << "\n";
        code_section << label_end << ":\n";
    }

    void visit(ForStatement& node) override {
        if (node.initializer) {
            node.initializer->accept(*this);
        }
        
        std::string label_start = "for_start_" + std::to_string(label_count++);
        std::string label_end = "for_end_" + std::to_string(label_count++);
        
        code_section << label_start << ":\n";
        node.condition->accept(*this);
        code_section << "    cmp rax, 0\n";
        code_section << "    je " << label_end << "\n";
        
        for (auto& stmt : node.body) {
            stmt->accept(*this);
        }
        
        if (node.increment) {
            node.increment->accept(*this);
        }
        
        code_section << "    jmp " << label_start << "\n";
        code_section << label_end << ":\n";
    }

    void visit(ForEachStatement& node) override {
        std::string label_start = "foreach_start_" + std::to_string(label_count++);
        std::string label_end = "foreach_end_" + std::to_string(label_count++);
        
        // 1. Evaluate the collection
        node.collection->accept(*this);
        // rax now contains the collection pointer
        
        // 2. Allocate stack slots for collection pointer and index
        int64_t coll_ptr_offset = allocateStack();
        int64_t index_offset = allocateStack();
        
        // Store collection pointer and initialize index
        code_section << "    mov [" << coll_ptr_offset << "(rbp)], rax\n";
        code_section << "    mov qword ptr [" << index_offset << "(rbp)], 0\n";
        
        // 3. Allocate local variable for the loop variable
        int64_t var_offset = allocateStack();
        symbol_table[node.variable_name] = {var_offset, TokenType::TYPE_INT};
        
        code_section << label_start << ":\n";
        
        // Load collection pointer and index for comparison
        code_section << "    mov rax, [" << coll_ptr_offset << "(rbp)]\n";
        code_section << "    mov rbx, [" << index_offset << "(rbp)]\n";
        
        // Check if index < length (assume length is at [rax])
        code_section << "    mov rcx, [rax]\n"; 
        code_section << "    cmp rbx, rcx\n";
        code_section << "    jge " << label_end << "\n";
        
        // Get element: collection + 8 (length) + index * 8
        code_section << "    lea rdx, [rax + 8]\n";
        code_section << "    mov rax, [rdx + rbx * 8]\n";
        
        // Store in loop variable
        code_section << "    mov [" << var_offset << "(rbp)], rax\n";
        
        // Execute body
        for (auto& stmt : node.body) {
            stmt->accept(*this);
        }
        
        // Increment index
        code_section << "    inc qword ptr [" << index_offset << "(rbp)]\n";
        
        code_section << "    jmp " << label_start << "\n";
        code_section << label_end << ":\n";
    }

    void visit(TryStatement& node) override {
        std::string label_try = "try_body_" + std::to_string(label_count++);
        std::string label_catch = "try_catch_" + std::to_string(label_count++);
        std::string label_finally = "try_finally_" + std::to_string(label_count++);
        std::string label_end = "try_end_" + std::to_string(label_count++);
        
        code_section << "    call runtime_push_context\n";
        code_section << "    call runtime_setjmp\n";
        code_section << "    cmp rax, 0\n";
        code_section << "    je " << label_try << "\n";
        
        code_section << label_catch << ":\n";
        code_section << "    call runtime_get_exception\n";
        
        // Allocate stack slot for catch variable
        int64_t catch_var_offset = allocateStack();
        symbol_table[node.catch_variable] = {catch_var_offset, TokenType::TYPE_OBJECT};
        code_section << "    mov [" << catch_var_offset << "(rbp)], rax\n";
        
        for (auto& stmt : node.catch_block) {
            stmt->accept(*this);
        }
        code_section << "    jmp " << label_finally << "\n";
        
        code_section << label_try << ":\n";
        for (auto& stmt : node.try_block) {
            stmt->accept(*this);
        }
        code_section << "    call runtime_pop_context\n";
        code_section << "    jmp " << label_finally << "\n";
        
        code_section << label_finally << ":\n";
        for (auto& stmt : node.finally_block) {
            stmt->accept(*this);
        }
        code_section << label_end << ":\n";
    }

    void visit(ThrowStatement& node) override {
        node.expression->accept(*this);
        // rax contains the exception object
        code_section << "    mov rcx, rax\n";
        code_section << "    call runtime_throw\n";
    }

    void visit(BinaryExpression& node) override {
        node.left->accept(*this);
        code_section << "    push rax\n";
        node.right->accept(*this);
        code_section << "    pop rbx\n";
        switch (node.operator_type) {
            case TokenType::OP_PLUS: code_section << "    add rax, rbx\n"; break;
            case TokenType::OP_MINUS: code_section << "    sub rax, rbx\n"; break;
            case TokenType::OP_MULTIPLY: code_section << "    imul rax, rbx\n"; break;
            case TokenType::OP_DIVIDE: code_section << "    cqo\n    idiv rbx\n"; break;
            default: break;
        }
    }

    void visit(UnaryExpression& node) override {
        node.operand->accept(*this);
        if (node.operator_type == TokenType::OP_MINUS) {
            code_section << "    neg rax\n";
        } else if (node.operator_type == TokenType::OP_LOGICAL_NOT) {
            code_section << "    cmp rax, 0\n";
            code_section << "    setz al\n";
            code_section << "    movzx rax, al\n";
        }
    }

    void visit(ReturnStatement& node) override {
        if (node.value) {
            node.value->accept(*this);
        } else {
            code_section << "    xor rax, rax\n";
        }
    }

    void visit(CallExpression& node) override {
        // Simple implementation: arguments in reverse order
        for (auto it = node.arguments.rbegin(); it != node.arguments.rend(); ++it) {
            (*it)->accept(*this);
            code_section << "    push rax\n";
        }
        
        node.callee->accept(*this);
        code_section << "    call rax\n";
        
        for (size_t i = 0; i < node.arguments.size(); ++i) {
            code_section << "    add rsp, 8\n";
        }
    }

    void visit(MemberExpression& node) override {
        node.object->accept(*this);
        code_section << "    ; Member access " << node.member << "\n";
        code_section << "    add rax, 0\n"; // Simplified
    }

private:
    int64_t allocateStack() {
        stack_offset -= 8;
        return stack_offset;
    }

    std::string module_name;
    std::stringstream code_section;
    std::stringstream data_section;
    struct Symbol {
        int64_t offset;
        TokenType type;
    };

    std::map<std::string, Symbol> symbol_table;
    int label_count;
    int64_t stack_offset;
};

} // namespace darija

#endif
