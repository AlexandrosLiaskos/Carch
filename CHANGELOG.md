# Changelog

All notable changes to Carch IDL Compiler will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.0.1] - 2025-11-09

### Added

**Core Features**
- Complete lexer with support for all tokens (keywords, identifiers, literals, operators)
- Recursive descent parser for Carch grammar
- Full semantic analysis including type checking, circular dependency detection, and forward reference handling
- C++ code generator producing idiomatic C++17 code
- Dual syntax support (compact and expanded forms)
- 14 primitive types: u8, u16, u32, u64, i8, i16, i32, i64, f32, f64, bool, str, bytes, unit
- Container types: array<T>, map<K,V>, optional<T>
- Entity reference system (ref<entity>) for breaking circular dependencies
- Struct, variant, and enum type definitions

**Type System**
- Product types (structs) with named fields
- Sum types (variants) with tagged alternatives
- Enumerations (enums) with named values
- Nested inline types (anonymous structs, variants, enums)
- Named type references and composition

**Code Generation**
- C++ struct generation for Carch structs
- std::variant generation for Carch variants
- enum class generation for Carch enums
- Proper handling of nested types
- Header guards and include directives
- Configurable namespace

**CLI Tool**
- Command-line interface with options
- Input file processing
- Output directory configuration
- Namespace customization
- Multiple file compilation

**Testing**
- Comprehensive unit tests for lexer
- Parser tests covering all type definitions
- Semantic analysis tests (circular deps, duplicates, forward refs)
- Code generation tests
- Integration tests for full pipeline
- Example compilation tests
- Stress tests for compiler robustness
- Edge case tests
- Error message tests
- Golden file tests for output verification
- Performance benchmarks

**Tooling**
- Linter (carch-lint) for style checking and best practices
- Formatter (carch-fmt) for code formatting
- Validator (carch-validate) for deep schema validation

**Documentation**
- Complete language specification
- Formal grammar (EBNF)
- Getting started tutorial
- ECS patterns guide
- Advanced types tutorial
- Integration guide for build systems
- API reference for library usage
- Troubleshooting guide
- Example schemas (basic, game entities, advanced)
- Real-world example project

**Infrastructure**
- CMake build system
- GitHub Actions CI/CD for Linux, Windows, macOS
- Automated release builds
- Code coverage reporting
- Static analysis with clang-tidy
- Fuzzing harnesses for security testing

**Distribution**
- Installation scripts for Unix and Windows
- Package generation scripts
- Binary releases for multiple platforms

### Known Limitations

- No generic/template types
- No traits/interfaces
- Single file input (no module system)
- C++ is the only target language
- Basic error recovery in parser

## [Unreleased]

See [ROADMAP.md](docs/roadmap.md) for planned features.

---

## Release Notes

### Version 0.0.1 - Initial Development Release

Carch IDL Compiler 0.0.1 is the initial development release, providing a complete toolchain for defining game component schemas and generating C++17 code. The compiler has been thoroughly tested and is ready for evaluation and testing in game development projects.

> **Note**: This is a pre-1.0 release. APIs and generated code format may change in future versions. See [ROADMAP.md](docs/roadmap.md) for plans leading to the 1.0.0 stable release.

**Highlights:**
- Type-safe component definitions
- Algebraic data types (sum and product types)
- Seamless C++ integration
- Comprehensive documentation and examples
- Cross-platform support (Linux, Windows, macOS)

**Getting Started:**
```bash
# Install
./scripts/install.sh

# Create a schema
echo "Position : struct { x: f32, y: f32, z: f32 }" > components.carch

# Compile
carch components.carch

# Use in C++
#include "generated/components.h"
```

For detailed information, see the [documentation](docs/).
