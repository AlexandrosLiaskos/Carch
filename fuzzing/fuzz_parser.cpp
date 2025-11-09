// Fuzzing harness for Carch parser

#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include <cstdint>
#include <cstddef>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    try {
        std::string input(reinterpret_cast<const char*>(data), size);
        
        carch::lexer::Lexer lexer(input);
        carch::parser::Parser parser(lexer);
        
        // Parse input
        auto schema = parser.parse();
        
        return 0;
    } catch (...) {
        return 0;
    }
}
