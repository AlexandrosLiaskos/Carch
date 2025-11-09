cmake_minimum_required(VERSION 3.15)

# Tool executables
add_executable(carch-lint carch-lint.cpp)
target_link_libraries(carch-lint PRIVATE carch_lib)
target_include_directories(carch-lint PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(carch-fmt carch-fmt.cpp)
target_link_libraries(carch-fmt PRIVATE carch_lib)
target_include_directories(carch-fmt PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(carch-validate carch-validate.cpp)
target_link_libraries(carch-validate PRIVATE carch_lib)
target_include_directories(carch-validate PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Installation
install(TARGETS carch-lint carch-fmt carch-validate DESTINATION bin)

# Custom target to build all tools
add_custom_target(tools
    DEPENDS carch-lint carch-fmt carch-validate
    COMMENT "Building all Carch tools"
)

message(STATUS "Configured Carch tools: linter, formatter, validator")
