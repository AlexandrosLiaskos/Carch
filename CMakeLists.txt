cmake_minimum_required(VERSION 3.15)

project(Carch VERSION 0.0.1 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_BENCHMARKS "Build performance benchmarks" OFF)
option(BUILD_TOOLS "Build additional tools (linter, formatter, validator)" ON)
option(BUILD_FUZZING "Build fuzzing targets" OFF)
option(UPDATE_GOLDEN_FILES "Update golden test files" OFF)
option(ENABLE_WERROR "Treat warnings as errors" OFF)
option(ENABLE_STRESS_TESTS "Enable stress tests" ON)
option(ENABLE_EDGE_CASE_TESTS "Enable edge case tests" ON)
option(ENABLE_ERROR_MESSAGE_TESTS "Enable error message tests" ON)

# Compiler flags
if(MSVC)
    add_compile_options(/W4)
    if(ENABLE_WERROR)
        add_compile_options(/WX)
    endif()
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(ENABLE_WERROR)
        add_compile_options(-Werror)
    endif()
endif()

# Source files for the Carch compiler
set(CARCH_SOURCES
    src/lexer/token.cpp
    src/lexer/lexer.cpp
    src/parser/ast.cpp
    src/parser/parser.cpp
    src/semantic/type_checker.cpp
    src/codegen/cpp_generator.cpp
    src/main.cpp
)

# Library sources (without main)
set(CARCH_LIB_SOURCES
    src/lexer/token.cpp
    src/lexer/lexer.cpp
    src/parser/ast.cpp
    src/parser/parser.cpp
    src/semantic/type_checker.cpp
    src/codegen/cpp_generator.cpp
)

# Headers (for IDE organization)
set(CARCH_HEADERS
    src/lexer/token.h
    src/lexer/lexer.h
    src/parser/ast.h
    src/parser/parser.h
    src/semantic/type_checker.h
    src/codegen/cpp_generator.h
)

# Carch compiler executable
add_executable(carch ${CARCH_SOURCES} ${CARCH_HEADERS})

# Include directories
target_include_directories(carch PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Installation
install(TARGETS carch DESTINATION bin)

# Tests
if(BUILD_TESTS)
    enable_testing()
    
    # Create a library for test linking
    add_library(carch_lib STATIC ${CARCH_LIB_SOURCES})
    target_include_directories(carch_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
    
    # Lexer tests
    add_executable(lexer_tests tests/lexer_tests.cpp)
    target_link_libraries(lexer_tests PRIVATE carch_lib)
    add_test(NAME lexer_tests COMMAND lexer_tests)
    
    # Parser tests
    add_executable(parser_tests tests/parser_tests.cpp)
    target_link_libraries(parser_tests PRIVATE carch_lib)
    add_test(NAME parser_tests COMMAND parser_tests)
    
    # Semantic tests
    add_executable(semantic_tests tests/semantic_tests.cpp)
    target_link_libraries(semantic_tests PRIVATE carch_lib)
    add_test(NAME semantic_tests COMMAND semantic_tests)
    
    # Codegen tests
    add_executable(codegen_tests tests/codegen_tests.cpp)
    target_link_libraries(codegen_tests PRIVATE carch_lib)
    add_test(NAME codegen_tests COMMAND codegen_tests)
    
    # Integration tests
    add_executable(integration_tests tests/integration_tests.cpp)
    target_link_libraries(integration_tests PRIVATE carch_lib)
    add_test(NAME integration_tests COMMAND integration_tests)
    
    # Example compilation tests
    add_executable(example_compilation_tests tests/example_compilation_tests.cpp)
    target_link_libraries(example_compilation_tests PRIVATE carch_lib)
    add_test(NAME example_compilation_tests COMMAND example_compilation_tests)
    
    # Stress tests
    if(ENABLE_STRESS_TESTS)
        add_executable(stress_tests tests/stress_tests.cpp)
        target_link_libraries(stress_tests PRIVATE carch_lib)
        add_test(NAME stress_tests COMMAND stress_tests)
    endif()
    
    # Edge case tests
    if(ENABLE_EDGE_CASE_TESTS)
        add_executable(edge_case_tests tests/edge_case_tests.cpp)
        target_link_libraries(edge_case_tests PRIVATE carch_lib)
        add_test(NAME edge_case_tests COMMAND edge_case_tests)
    endif()
    
    # Error message tests
    if(ENABLE_ERROR_MESSAGE_TESTS)
        add_executable(error_message_tests tests/error_message_tests.cpp)
        target_link_libraries(error_message_tests PRIVATE carch_lib)
        add_test(NAME error_message_tests COMMAND error_message_tests)
    endif()
    
    # Golden file tests
    add_executable(golden_file_tests tests/golden_file_tests.cpp)
    target_link_libraries(golden_file_tests PRIVATE carch_lib)
    add_test(NAME golden_file_tests COMMAND golden_file_tests)
    if(UPDATE_GOLDEN_FILES)
        set_tests_properties(golden_file_tests PROPERTIES ENVIRONMENT "UPDATE_GOLDEN=1")
    endif()
    
    # Performance benchmarks (optional)
    if(BUILD_BENCHMARKS)
        add_executable(performance_tests tests/performance_tests.cpp)
        target_link_libraries(performance_tests PRIVATE carch_lib)
        # Don't add to CTest by default, run manually
        message(STATUS "Performance benchmarks enabled")
    endif()
    
    # Custom target to run all tests
    set(TEST_TARGETS lexer_tests parser_tests semantic_tests codegen_tests integration_tests example_compilation_tests golden_file_tests)
    if(ENABLE_STRESS_TESTS)
        list(APPEND TEST_TARGETS stress_tests)
    endif()
    if(ENABLE_EDGE_CASE_TESTS)
        list(APPEND TEST_TARGETS edge_case_tests)
    endif()
    if(ENABLE_ERROR_MESSAGE_TESTS)
        list(APPEND TEST_TARGETS error_message_tests)
    endif()
    add_custom_target(test_all
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS ${TEST_TARGETS}
        COMMENT "Running all test suites"
    )
    
    message(STATUS "Tests enabled and configured")
endif()

# Examples
if(BUILD_EXAMPLES)
    message(STATUS "Examples will be available in the examples/ directory")
    message(STATUS "Compile them manually with: ./carch examples/*.carch")
endif()

# Tools
if(BUILD_TOOLS)
    add_subdirectory(tools)
    message(STATUS "Building additional tools (linter, formatter, validator)")
endif()

# Fuzzing
if(BUILD_FUZZING)
    add_subdirectory(fuzzing)
    message(STATUS "Building fuzzing targets")
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "Carch IDL Compiler Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
message(STATUS "  Build Benchmarks: ${BUILD_BENCHMARKS}")
message(STATUS "  Build Tools: ${BUILD_TOOLS}")
message(STATUS "  Build Fuzzing: ${BUILD_FUZZING}")
message(STATUS "  Build Examples: ${BUILD_EXAMPLES}")
message(STATUS "  Update Golden Files: ${UPDATE_GOLDEN_FILES}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")