#include "test_common.hpp"
#include <iostream>

// Forward declarations of test runners
void run_lexer_tests();
void run_parser_tests();
void run_ast_tests();

int main() {
    std::cout << "Starting all unit tests..." << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    
    run_lexer_tests();
    run_parser_tests();
    run_ast_tests();
    
    std::cout << "-----------------------------------" << std::endl;
    
    if (darija_tests::failed) {
        std::cerr << "Some tests failed!" << std::endl;
        return 1;
    }
    
    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}
