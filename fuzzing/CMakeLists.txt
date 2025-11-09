cmake_minimum_required(VERSION 3.15)

# Fuzzing targets (requires Clang with fuzzer support)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_executable(fuzz_lexer fuzz_lexer.cpp)
    target_link_libraries(fuzz_lexer PRIVATE carch_lib)
    target_compile_options(fuzz_lexer PRIVATE -fsanitize=fuzzer,address)
    target_link_options(fuzz_lexer PRIVATE -fsanitize=fuzzer,address)
    
    add_executable(fuzz_parser fuzz_parser.cpp)
    target_link_libraries(fuzz_parser PRIVATE carch_lib)
    target_compile_options(fuzz_parser PRIVATE -fsanitize=fuzzer,address)
    target_link_options(fuzz_parser PRIVATE -fsanitize=fuzzer,address)
    
    add_executable(fuzz_full_pipeline fuzz_full_pipeline.cpp)
    target_link_libraries(fuzz_full_pipeline PRIVATE carch_lib)
    target_compile_options(fuzz_full_pipeline PRIVATE -fsanitize=fuzzer,address)
    target_link_options(fuzz_full_pipeline PRIVATE -fsanitize=fuzzer,address)
    
    message(STATUS "Fuzzing targets configured (use Clang to build)")
else()
    message(STATUS "Fuzzing requires Clang compiler with -fsanitize=fuzzer support")
endif()
