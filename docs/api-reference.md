# API Reference for Carch 0.0.1

> ⚠️ **Pre-1.0 API - Subject to change**
>
> This API is for an early development version. It is not yet stable and may have breaking changes in future releases. API stability is a primary goal for the 1.0.0 release.
>
> **Repository**: https://github.com/AlexandrosLiaskos/Carch
> **Examples**: See the [examples directory](../examples/) for usage.

Using Carch as a library in your own tools.

## Overview

Carch can be used as a library to build custom tools, IDE plugins, or alternative code generators.

## Core Classes

### Lexer

```cpp
#include "lexer/lexer.h"

carch::lexer::Lexer lexer(source_code);

// Get next token
auto token = lexer.next_token();

// Check token type
if (token.type == carch::lexer::TokenType::Identifier) {
    std::string name = token.value;
}

// Peek ahead without consuming
auto next = lexer.peek_token();
```

**Thread Safety**: Not thread-safe. Create separate instances per thread.

### Parser

```cpp
#include "parser/parser.h"

carch::lexer::Lexer lexer(source);
carch::parser::Parser parser(lexer);

// Parse schema
auto schema = parser.parse();

// Check for errors
if (parser.has_errors()) {
    // Handle errors
}

// Access type definitions
for (const auto& type_def : schema->type_definitions) {
    std::cout << type_def->name << "\n";
}
```

### Type Checker

```cpp
#include "semantic/type_checker.h"

carch::semantic::TypeChecker checker(schema.get());

// Run semantic analysis
bool valid = checker.check();

if (!valid) {
    // Semantic errors detected
}
```

### Code Generator

```cpp
#include "codegen/cpp_generator.h"

carch::codegen::GenerationOptions opts;
opts.namespace_name = "mygame";
opts.output_basename = "components";

carch::codegen::CppGenerator generator(schema.get(), opts);

// Generate C++ header
std::string header = generator.generate_header();

// Write to file
std::ofstream out("components.h");
out << header;
```

## AST Node Types

### Schema

```cpp
struct Schema {
    std::vector<std::unique_ptr<TypeDefinition>> type_definitions;
};
```

### TypeDefinition

```cpp
struct TypeDefinition {
    std::string name;
    std::unique_ptr<Type> type;
    size_t line;
    size_t column;
};
```

### Type Hierarchy

- `Type` (base class)
  - `PrimitiveType`
  - `StructType`
  - `VariantType`
  - `EnumType`
  - `ArrayType`
  - `MapType`
  - `OptionalType`
  - `RefType`

## Visitor Pattern

To traverse the AST:

```cpp
class MyVisitor : public carch::parser::TypeVisitor {
public:
    void visit(carch::parser::StructType* type) override {
        // Handle struct
        for (const auto& field : type->fields) {
            // Process field
        }
    }
    
    void visit(carch::parser::VariantType* type) override {
        // Handle variant
    }
    
    // Implement other visit methods...
};

// Use visitor
MyVisitor visitor;
type->accept(&visitor);
```

## Example: Custom Code Generator

```cpp
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semantic/type_checker.h"

class PythonGenerator {
public:
    std::string generate(carch::parser::Schema* schema) {
        std::ostringstream out;
        
        out << "from dataclasses import dataclass\n";
        out << "from typing import Optional, List, Dict\n\n";
        
        for (const auto& type_def : schema->type_definitions) {
            if (auto* struct_type = dynamic_cast<carch::parser::StructType*>(type_def->type.get())) {
                generate_struct(out, type_def->name, struct_type);
            }
        }
        
        return out.str();
    }
    
private:
    void generate_struct(std::ostream& out, const std::string& name, 
                        carch::parser::StructType* type) {
        out << "@dataclass\n";
        out << "class " << name << ":\n";
        
        for (const auto& field : type->fields) {
            out << "    " << field.name << ": ";
            // Generate Python type for field...
            out << "\n";
        }
        
        out << "\n";
    }
};
```

## Error Handling

Errors are reported through the parser:

```cpp
parser.parse();

if (parser.has_errors()) {
    // Access error messages
    // (Implementation detail - check parser.h for exact API)
}
```

## Memory Management

- Use `std::unique_ptr` for ownership
- AST nodes are owned by parent nodes
- Schema owns TypeDefinitions
- TypeDefinitions own Types

## Building Custom Tools

### IDE Plugin

```cpp
// Provide autocomplete
std::vector<std::string> get_completions(const std::string& source, size_t cursor) {
    carch::lexer::Lexer lexer(source);
    carch::parser::Parser parser(lexer);
    auto schema = parser.parse();
    
    std::vector<std::string> completions;
    
    // Add type names
    for (const auto& type_def : schema->type_definitions) {
        completions.push_back(type_def->name);
    }
    
    // Add keywords
    completions.push_back("struct");
    completions.push_back("variant");
    completions.push_back("enum");
    
    return completions;
}
```

### Schema Validator

```cpp
bool validate_file(const std::string& filename) {
    std::ifstream file(filename);
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    carch::lexer::Lexer lexer(source);
    carch::parser::Parser parser(lexer);
    auto schema = parser.parse();
    
    if (parser.has_errors()) return false;
    
    carch::semantic::TypeChecker checker(schema.get());
    return checker.check();
}
```

## See Also

- [Source Code](../src/) - Implementation details
- [Examples](../examples/) - Usage examples
- [Contributing](../CONTRIBUTING.md) - Development guidelines
