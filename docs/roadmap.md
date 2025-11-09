# Roadmap

This document outlines the future development plans for the Carch IDL Compiler.

## Version 0.0.1 (Current)

**Status**: Initial Development Release

This is the initial public release for community feedback and testing.

- ✓ Complete lexer with all token types
- ✓ Recursive descent parser
- ✓ Full semantic analysis (type checking, circular dependency detection)
- ✓ C++ code generator (structs, variants, enums)
- ✓ Dual syntax support (compact and expanded)
- ✓ Entity reference system
- ✓ Container types (array, map, optional)
- ✓ CLI tool with options
- ✓ Comprehensive test suite
- ✓ Documentation and examples
- ✓ GitHub Actions CI/CD

## Version 1.0 (Planned)

**Target**: Q2 2026

**Focus**: API Stability and Production Readiness

- [ ] Stabilize APIs and generated code format
- [ ] Address community feedback from 0.0.1
- [ ] Performance optimizations based on real-world usage
- [ ] Enhanced error messages and diagnostics
- [ ] Complete all features from the original 1.0 list (now part of 0.0.1)

## Version 1.1 (Q3 2026)

**Focus**: Tooling and IDE Support

- [ ] **Language Server Protocol (LSP)**
  - Autocomplete for type names and keywords
  - Go-to-definition support
  - Inline error diagnostics
  - Hover documentation

- [ ] **IDE Plugins**
  - VS Code extension
  - IntelliJ/CLion plugin
  - Vim/Neovim syntax highlighting

- [ ] **Enhanced Tooling**
  - Improved linter with more rules
  - Formatter with customizable styles
  - Schema diff tool for version comparison

- [ ] **Documentation Generator**
  - Generate HTML/Markdown docs from schemas
  - Diagram generation (UML-style)

## Version 1.2 (Q4 2026)

**Focus**: Multi-Language Support

- [ ] **Additional Code Generators**
  - Rust code generator
  - C# code generator
  - TypeScript/JavaScript generator

- [ ] **Serialization Support**
  - Built-in JSON serialization
  - Binary serialization (MessagePack, Protocol Buffers)
  - Custom serialization templates

- [ ] **Schema Registry**
  - Schema versioning system
  - Compatibility checking
  - Migration tool generation

## Version 2.0 (2027)

**Focus**: Language Enhancements

- [ ] **Generics/Templates**
  - Generic struct definitions
  - Type parameters
  - Constraints on generic types

- [ ] **Traits/Interfaces**
  - Abstract type definitions
  - Implementation checking

- [ ] **Advanced Type System**
  - Union types (vs current tagged unions)
  - Intersection types
  - Type aliases
  - Newtype pattern support

- [ ] **Code Organization**
  - Module system for large schemas
  - Import/include mechanism
  - Namespace management

## Long-Term Vision

### Ecosystem Growth

- **Package Registry**: Central repository for shared schemas
- **Standard Library**: Common game component schemas
- **Best Practices**: Curated patterns and examples
- **Community Contributions**: Plugin ecosystem

### Game Engine Integration

- **Unity Integration**: Direct import of Carch schemas
- **Unreal Engine**: Plugin for UE component system
- **Godot**: Native Carch support
- **Custom Engines**: Integration guides and examples

## Community Requests

Vote on features at: https://github.com/AlexandrosLiaskos/Carch/discussions/categories/ideas

Current top requests:
1. Rust code generator
2. VS Code extension
3. JSON schema export
4. Generic types
5. Better error messages

## Contributing

Want to help? See [CONTRIBUTING.md](../CONTRIBUTING.md) for:
- Feature implementation guidelines
- Code review process
- Testing requirements
- Documentation standards

## Versioning Policy

- **Major versions** (2.0, 3.0): Breaking changes to language or generated code
- **Minor versions** (1.1, 1.2): New features, backward compatible
- **Patch versions** (1.0.1, 1.0.2): Bug fixes only

## Deprecation Policy

- Features marked deprecated for at least one minor version before removal
- Migration guides provided for breaking changes
- Compatibility tools for transitioning

## Release Schedule

- **Minor releases**: Quarterly
- **Patch releases**: As needed for critical bugs
- **Major releases**: Yearly or when significant features ready

## Feedback

- Feature requests: GitHub Discussions
- Bug reports: GitHub Issues
- General discussion: Discord/Community forum

Stay updated: https://github.com/AlexandrosLiaskos/Carch/releases