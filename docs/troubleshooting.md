# Troubleshooting Guide

Common issues and solutions when using Carch version 0.0.1.

> **Note**: This is an initial development release. If you encounter an issue not listed here, please report it. Response times may vary, but all reports will be reviewed.

## Compilation Errors

### Syntax Errors

**Symptom**: `Parse error: expected ':', got 'struct'`

**Solution**: Check syntax - type name, colon, type definition:
```carch
// Correct
Point : struct { x: f32, y: f32 }

// Incorrect (missing colon)
Point struct { x: f32, y: f32 }
```

### Duplicate Type Definitions

**Symptom**: `Semantic error: Type 'Position' is already defined`

**Solution**: Each type name must be unique. Rename one of them.

### Undefined Type Reference

**Symptom**: `Semantic error: Undefined type 'Transform'`

**Solution**: Define types before use, or use `ref<entity>` for forward references.

### Circular Dependencies

**Symptom**: `Semantic error: Circular dependency detected`

**Solution**: Use `ref<entity>` to break the cycle:
```carch
// Incorrect
Node : struct { next: Node }

// Correct
Node : struct { next: optional<ref<entity>> }
```

### Nested Optionals

**Symptom**: `Semantic error: Nested optionals are not allowed`

**Solution**: Remove nested optionals:
```carch
// Incorrect
value: optional<optional<str>>

// Correct
value: optional<str>
```

## Generated Code Issues

### Compilation Failures

**Symptom**: C++ compiler errors in generated code

**Solutions**:
1. Check for C++ reserved keywords as identifiers
2. Ensure C++17 or later
3. Include necessary headers (`<variant>`, `<optional>`, etc.)

### Type Mismatches

**Symptom**: `error: no matching function for call to 'std::variant<...>'`

**Solution**: Use correct variant alternative:
```cpp
// If Weapon has alternatives: sword, bow, unarmed
game::Weapon weapon = game::Weapon_sword{10}; // Correct
// Not: game::Weapon weapon{10}; // Incorrect
```

### Missing Headers

**Symptom**: `error: 'std::variant' has not been declared`

**Solution**: Ensure C++17 and include generated header:
```cpp
#include "components.h"  // Contains necessary includes
```

## Performance Problems

### Slow Compilation

**Symptom**: `carch` takes a long time

**Solutions**:
1. Split large schemas into multiple files
2. Reduce deep nesting
3. Check for accidental infinite recursion

### Large Generated Files

**Symptom**: Generated headers are megabytes in size

**Solutions**:
1. Avoid generating thousands of types
2. Use named types instead of repeating inline types
3. Split into multiple schema files

## Integration Issues

### Build System Problems

**Symptom**: Generated headers not found

**Solution**: Add to include path:
```cmake
target_include_directories(myproject PRIVATE ${CMAKE_BINARY_DIR}/generated)
```

**Symptom**: Headers not regenerated when .carch files change

**Solution**: Add proper dependencies in CMake:
```cmake
add_dependencies(myproject schema_generation)
```

### Include Path Issues

**Symptom**: `fatal error: components.h: No such file or directory`

**Solution**: Verify output directory and include paths match.

## Tool Issues

### Linter False Positives

**Symptom**: `carch-lint` reports issues that aren't problems

**Solution**: Warnings can be ignored if intentional. Use `--strict` only for CI.

### Formatter Edge Cases

**Symptom**: `carch-fmt` produces unexpected formatting

**Solution**: Currently basic - manual formatting may be needed for complex schemas.

## Debugging Tips

### Verbose Output

```bash
carch --verbose components.carch
```

### View Tokens

```bash
# Not built-in, but you can add to lexer_tests
./lexer_tests < components.carch
```

### Check Semantic Analysis

```bash
carch-validate components.carch
```

### View Generated Output

```bash
cat generated/components.h
```

## Reporting Bugs

When reporting issues for this development release, please include:

1. Carch version: `carch --version` (should be 0.0.1)
2. Input `.carch` file
3. Error message (full output)
4. Platform (OS, compiler, versions)
5. Expected vs actual behavior

Submit at: https://github.com/AlexandrosLiaskos/Carch/issues

## FAQ

**Q: Can I use C++ keywords as identifiers?**  
A: Avoid them. Carch may allow it but C++ won't compile the output.

**Q: How do I handle schema versioning?**  
A: Use optional fields for additions, separate schemas for breaking changes.

**Q: Can I customize code generation?**  
A: Not currently. Extensions planned for future versions.

**Q: Does Carch support generics?**  
A: Not yet. Planned for v2.0.

**Q: Can I generate code for languages other than C++?**  
A: Not currently. Rust/C# generators planned.

## Getting Help

For further assistance:

- **Developer Contact**: Alexandros Liaskos ([alexliaskos@geol.uoa.gr](mailto:alexliaskos@geol.uoa.gr))
- [Specification](specification.md) - Language reference
- [API Reference](api-reference.md) - Using Carch as a library
- [GitHub Discussions](https://github.com/AlexandrosLiaskos/Carch/discussions) - Community help