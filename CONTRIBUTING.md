# Contributing to Carch

Thank you for your interest in contributing to Carch! This document provides guidelines for contributing to the project.

## Table of Contents

1. [How to Report Bugs](#how-to-report-bugs)
2. [How to Suggest Features](#how-to-suggest-features)
3. [Development Setup](#development-setup)
4. [Coding Standards](#coding-standards)
5. [Pull Request Process](#pull-request-process)
6. [Testing Requirements](#testing-requirements)
7. [Documentation Requirements](#documentation-requirements)

## How to Report Bugs

Before creating a bug report, please check existing issues to avoid duplicates.

When filing a bug report, include:

- **Clear title**: Summarize the issue concisely
- **Description**: Detailed explanation of the problem
- **Steps to reproduce**: Minimal steps to reproduce the behavior
- **Expected behavior**: What you expected to happen
- **Actual behavior**: What actually happened
- **Environment**: OS, compiler version, Carch version
- **Sample code**: Minimal `.carch` file demonstrating the issue
- **Error messages**: Full error output if applicable

**Example issue template:**

```
Title: Parser fails on nested variant types

Description: The parser crashes when parsing a variant type nested inside a struct field.

Steps to reproduce:
1. Create a .carch file with the following content:
   Test : struct { field: variant { a: u32, b: str } }
2. Run: carch test.carch
3. Parser crashes with segmentation fault

Expected: Should parse successfully and generate C++ code
Actual: Segmentation fault

Environment: Windows 11, MSVC 2022, Carch 0.0.1
```

## How to Suggest Features

Feature suggestions are welcome! Please:

- Check the [design decisions document](docs/design-decisions.md) to understand design philosophy
- Review the [language specification](docs/specification.md) to see if it fits the language model
- Search existing issues for similar requests
- Provide a clear use case for the feature
- Explain how it aligns with ECS architecture patterns

**Example feature request:**

```
Title: Add support for generic types

Description: Allow defining reusable generic container types like Pair<T, U>.

Use case: Reduce duplication when defining similar structures (e.g., Vec2<f32>, Vec2<i32>)

Proposed syntax:
Pair<T, U> : struct { first: T, second: U }
Vec2<T> : struct { x: T, y: T }

Alignment with ECS: Supports reusable component patterns without code duplication.
```

## Development Setup

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Git

### Building from Source

```bash
# Clone the repository
git clone https://github.com/AlexandrosLiaskos/Carch.git
cd carch

# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Run tests (once test suite is set up)
ctest

# Install (optional)
cmake --install .
```

### Running Tests

```bash
cd build
ctest --output-on-failure
```

## Coding Standards

### C++ Style Guide

We follow a consistent C++ coding style:

**Naming Conventions:**
- **Classes/Structs**: `PascalCase` (e.g., `TypeChecker`, `ASTNode`)
- **Functions/Methods**: `snake_case` (e.g., `parse_type_expr`, `check_errors`)
- **Variables**: `snake_case` (e.g., `current_token`, `symbol_table_`)
- **Constants**: `SCREAMING_SNAKE_CASE` (e.g., `MAX_DEPTH`)
- **Private members**: Trailing underscore (e.g., `lexer_`, `errors_`)
- **Namespaces**: `snake_case` (e.g., `carch::lexer`, `carch::parser`)

**Formatting:**
- Indentation: 4 spaces (no tabs)
- Brace style: Allman (braces on new line for functions/classes)
- Line length: 120 characters max
- Include order: system headers, then project headers

**Example:**

```cpp
namespace carch {
namespace parser {

class TypeChecker {
public:
    explicit TypeChecker(SchemaNode* schema);
    bool check();
    
private:
    void check_type_expr(TypeExprNode* expr);
    
    SchemaNode* schema_;
    std::vector<std::string> errors_;
};

} // namespace parser
} // namespace carch
```

### Best Practices

- Use RAII for resource management
- Prefer `std::unique_ptr` for ownership, raw pointers for non-owning references
- Use `const` wherever possible
- Avoid manual memory management (new/delete)
- Write self-documenting code with clear variable names
- Add comments for complex algorithms, not obvious code

## Pull Request Process

1. **Fork the repository** and create a new branch from `main`

   ```bash
   git checkout -b feature/my-new-feature
   ```

2. **Make your changes** following the coding standards

3. **Write or update tests** for your changes

4. **Update documentation** if you're changing behavior or adding features

5. **Ensure all tests pass**

   ```bash
   cmake --build . && ctest
   ```

6. **Commit with clear messages**

   ```
   Add support for generic types in parser
   
   - Implemented generic type syntax parsing
   - Added AST nodes for generic type parameters
   - Updated grammar specification
   - Added tests for generic type parsing
   ```

7. **Push to your fork** and create a Pull Request

8. **PR description should include:**
   - Summary of changes
   - Motivation and context
   - Related issue numbers (if any)
   - Testing done
   - Screenshots/examples (if applicable)

### Branch Naming

- Feature branches: `feature/description`
- Bug fixes: `fix/description`
- Documentation: `docs/description`
- Refactoring: `refactor/description`

## Testing Requirements

All pull requests must include appropriate tests:

- **Unit tests**: For individual components (lexer, parser, type checker, code generator)
- **Integration tests**: For end-to-end compilation pipeline
- **Regression tests**: For bug fixes, include test that would have caught the bug

Tests should:
- Cover both success and failure cases
- Test edge cases and boundary conditions
- Be deterministic and repeatable
- Run quickly (< 1 second per test ideally)

**Example test:**

```cpp
TEST(ParserTest, ParseSimpleStruct) {
    std::string source = "Position : struct { x: f32, y: f32 }";
    carch::lexer::Lexer lexer(source);
    carch::parser::Parser parser(lexer);
    
    auto schema = parser.parse();
    
    ASSERT_FALSE(parser.has_errors());
    ASSERT_EQ(schema->definitions.size(), 1);
    EXPECT_EQ(schema->definitions[0]->name, "Position");
}
```

## Documentation Requirements

When adding features or changing behavior:

1. **Update the specification** ([docs/specification.md](docs/specification.md))
   - Add or modify grammar rules
   - Update type system documentation
   - Add semantic rules if applicable

2. **Update examples** ([docs/examples.md](docs/examples.md))
   - Add examples demonstrating new features
   - Show both simple and complex usage

3. **Update design decisions** ([docs/design-decisions.md](docs/design-decisions.md))
   - Explain rationale for new features
   - Document trade-offs considered
   - Compare with alternative approaches

4. **Update README** if changing user-facing behavior

5. **Add code comments** for complex implementations

## Language Design Authority

The [language specification](docs/specification.md) is the authoritative source for Carch language design. Before implementing language changes:

1. Propose changes to the specification first
2. Discuss design implications in an issue
3. Get consensus from maintainers
4. Update specification before implementing

This ensures the language remains coherent and well-designed.

## Code of Conduct

All participants are expected to follow our [Code of Conduct](CODE_OF_CONDUCT.md).

- Be respectful and inclusive
- Focus on constructive feedback
- Accept criticism gracefully
- Prioritize project goals over personal preferences
- Help newcomers learn and contribute

## Questions?

If you have questions about contributing:
- Open an issue with the `question` label
- Join discussions in existing issues
- Contact the project maintainer: Alexandros Liaskos ([alexliaskos@geol.uoa.gr](mailto:alexliaskos@geol.uoa.gr))

**Note**: As this is an initial development release (0.0.1), response times may vary. We appreciate your patience and contributions as we work toward the 1.0.0 stable release.

Thank you for contributing to Carch! 🚀
