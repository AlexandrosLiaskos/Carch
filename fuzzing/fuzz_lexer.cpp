// Fuzzing harness for Carch lexer

#include "../src/lexer/lexer.h"
#include <cstdint>
#include <cstddef>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    try {
        std::string input(reinterpret_cast<const char*>(data), size);
        
        carch::lexer::Lexer lexer(input);
        
        // Tokenize entire input
        while (true) {
            auto token = lexer.next_token();
            if (token.type == carch::lexer::TokenType::EndOfFile) {
                break;
            }
        }
        
        return 0;
    } catch (...) {
        // Catch all exceptions to prevent fuzzer from stopping
        return 0;
    }
}
