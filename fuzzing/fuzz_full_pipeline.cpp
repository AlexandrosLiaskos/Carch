// Fuzzing harness for full Carch pipeline

#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/semantic/type_checker.h"
#include "../src/codegen/cpp_generator.h"
#include <cstdint>
#include <cstddef>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    try {
        std::string input(reinterpret_cast<const char*>(data), size);
        
        carch::lexer::Lexer lexer(input);
        carch::parser::Parser parser(lexer);
        auto schema = parser.parse();
        
        if (!parser.has_errors()) {
            carch::semantic::TypeChecker checker(schema.get());
            
            if (checker.check()) {
                carch::codegen::GenerationOptions opts;
                opts.namespace_name = "fuzz";
                opts.output_basename = "test";
                
                carch::codegen::CppGenerator generator(schema.get(), opts);
                std::string output = generator.generate_header();
            }
        }
        
        return 0;
    } catch (...) {
        return 0;
    }
}
