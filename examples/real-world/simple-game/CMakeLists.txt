cmake_minimum_required(VERSION 3.15)
project(SimpleGame CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Carch compiler
find_program(CARCH_COMPILER carch REQUIRED)

# Generate components header from schema
set(CARCH_SCHEMA ${CMAKE_CURRENT_SOURCE_DIR}/components.carch)
set(GENERATED_HEADER ${CMAKE_CURRENT_BINARY_DIR}/components.h)

add_custom_command(
    OUTPUT ${GENERATED_HEADER}
    COMMAND ${CARCH_COMPILER} ${CARCH_SCHEMA} -o ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CARCH_SCHEMA}
    COMMENT "Generating components from Carch schema"
)

add_custom_target(generate_components DEPENDS ${GENERATED_HEADER})

# Game executable
add_executable(simple_game main.cpp)
add_dependencies(simple_game generate_components)
target_include_directories(simple_game PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

message(STATUS "Simple game configured - components will be generated from ${CARCH_SCHEMA}")
