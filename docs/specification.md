# Carch Language Specification

**Specification Version**: 0.0.1
**Status**: Draft
**Author**: Alexandros Liaskos
**Repository**: https://github.com/AlexandrosLiaskos/Carch

> **Note**: This specification is for a pre-1.0 version of Carch and may change before the 1.0.0 release.

## Table of Contents

1. [Overview](#overview)
2. [Design Goals](#design-goals)
3. [Lexical Structure](#lexical-structure)
4. [Type System](#type-system)
5. [Grammar](#grammar)
6. [Semantic Rules](#semantic-rules)
7. [Syntax Forms](#syntax-forms)
8. [Reserved Keywords](#reserved-keywords)

## Overview

Carch (Component Architecture) is a domain-specific language for defining Entity-Component System architectures. It provides algebraic data types (product types via structs, sum types via variants, and enumeration types) with a focus on clarity, composability, and safe code generation to C++.

## Design Goals

1. **Type Safety**: Prevent invalid component compositions at compile time
2. **Expressiveness**: Support complex ECS patterns with minimal syntax
3. **Clarity**: Make component relationships explicit and obvious
4. **C++ Integration**: Generate idiomatic, zero-overhead C++ code
5. **Composability**: Enable deep nesting and composition of types

## Lexical Structure

### Keywords

Reserved keywords that cannot be used as identifiers:

```
struct      variant     enum        unit
array       map         optional    ref
entity      str         int         bool
u8  u16  u32  u64
i8  i16  i32  i64
f32 f64
```

### Identifiers

Identifiers name types and fields. They must:
- Start with a letter (a-z, A-Z)
- Contain only letters, digits (0-9), and underscores (_)
- Not be a reserved keyword

**Valid identifiers:**
```
Position
player_health
Weapon2D
_internal
```

**Invalid identifiers:**
```
2Position    // starts with digit
player-health // contains hyphen
struct       // reserved keyword
```

### Literals

**Integer Literals:**
```
0  42  1000  0xFF  0b1010  0o755
```

**Float Literals:**
```
0.0  3.14  1.5e10  2.0E-5
```

**String Literals:**
```
"hello"  "player name"  "path\\to\\file"  "line\nnew"
```

**Boolean Literals:**
```
true  false
```

### Comments

**Single-line comments:**
```
// This is a comment
Position : struct { x: f32, y: f32 } // end-of-line comment
```

**Multi-line comments:**
```
/*
 * Multi-line comment
 * describing complex logic
 */
```

### Whitespace

Whitespace (spaces, tabs, newlines) is generally insignificant except:
- To separate tokens
- In expanded syntax, newlines may improve readability
- Indentation is optional but recommended for nested structures

## Type System

### Primitive Types

| Type | Description | C++ Mapping | Range/Notes |
|------|-------------|-------------|-------------|
| `str` | Unicode string | `std::string` | UTF-8 encoded |
| `int` | Default integer | `int32_t` | 32-bit signed |
| `u8` | Unsigned 8-bit | `uint8_t` | 0 to 255 |
| `u16` | Unsigned 16-bit | `uint16_t` | 0 to 65,535 |
| `u32` | Unsigned 32-bit | `uint32_t` | 0 to 4,294,967,295 |
| `u64` | Unsigned 64-bit | `uint64_t` | 0 to 2^64-1 |
| `i8` | Signed 8-bit | `int8_t` | -128 to 127 |
| `i16` | Signed 16-bit | `int16_t` | -32,768 to 32,767 |
| `i32` | Signed 32-bit | `int32_t` | -2^31 to 2^31-1 |
| `i64` | Signed 64-bit | `int64_t` | -2^63 to 2^63-1 |
| `f32` | 32-bit float | `float` | IEEE 754 single |
| `f64` | 64-bit float | `double` | IEEE 754 double |
| `bool` | Boolean | `bool` | true or false |
| `unit` | Empty type | `std::monostate` | No data |

### Composite Types

#### Struct (Product Type)

Combines multiple fields with AND semantics. All fields must be present.

```
TypeName : struct {
    field1: Type1,
    field2: Type2,
    ...
    fieldN: TypeN
}
```

**Properties:**
- Field names must be unique within the struct
- Fields are ordered
- Can be nested arbitrarily

#### Variant (Sum Type)

Represents a choice between alternatives with OR semantics. Exactly one alternative is active.

```
TypeName : variant {
    alternative1: Type1,
    alternative2: Type2,
    ...
    alternativeN: TypeN
}
```

**Properties:**
- Alternative names must be unique within the variant
- Each alternative may have a type or be `unit` (implicit if omitted)
- Variants are discriminated unions

**Unit alternatives:**
```
State : variant {
    idle: unit,      // explicit unit
    running,         // implicit unit
    jumping: unit    // explicit unit
}
```

#### Enum (Enumeration Type)

Simple discriminated union without associated data.

```
TypeName : enum { value1, value2, ..., valueN }
```

**Properties:**
- Values must be unique within the enum
- No associated data (all values are unit-like)
- Maps to C++ `enum class`

### Container Types

#### Array

Homogeneous sequence of elements.

```
array<ElementType>
```

**Examples:**
```
Inventory : array<u32>
Positions : array<struct { x: f32, y: f32 }>
```

**C++ Mapping:** `std::vector<ElementType>`

#### Map

Key-value associative container.

```
map<KeyType, ValueType>
```

**Examples:**
```
ItemCounts : map<u32, u32>
NamedEntities : map<str, ref<entity>>
```

**C++ Mapping:** `std::unordered_map<KeyType, ValueType>`

#### Optional

Value that may or may not be present.

```
optional<Type>
```

**Examples:**
```
OptionalName : optional<str>
MaybeParent : optional<ref<entity>>
```

**C++ Mapping:** `std::optional<Type>`

### Reference Types

#### Entity Reference

Reference to another entity in the ECS.

```
ref<entity>
```

**Examples:**
```
Parent : ref<entity>
Target : optional<ref<entity>>
Children : array<ref<entity>>
```

**C++ Mapping:** `entity_id` (typedef for `uint64_t`) or pointer depending on code generation options.

### User-Defined Types

Types defined in the schema can be referenced by name:

```
Position : struct { x: f32, y: f32 }
Entity : struct { id: u64, pos: Position }  // references Position
```

## Grammar

See [grammar.ebnf](grammar.ebnf) for the complete formal grammar.

### High-Level Structure

```ebnf
schema = { type_definition } ;

type_definition = identifier ":" type_expr ;

type_expr = struct_type
          | variant_type
          | enum_type
          | primitive_type
          | container_type
          | ref_type
          | identifier ;
```

### Struct Type

```ebnf
struct_type = "struct" "{" [ field_list ] "}" ;
field_list = field { "," field } [ "," ] ;
field = identifier ":" type_expr ;
```

### Variant Type

```ebnf
variant_type = "variant" "{" [ alternative_list ] "}" ;
alternative_list = alternative { "," alternative } [ "," ] ;
alternative = identifier [ ":" type_expr ] ;
```

### Enum Type

```ebnf
enum_type = "enum" "{" [ enum_value_list ] "}" ;
enum_value_list = identifier { "," identifier } [ "," ] ;
```

### Container Types

```ebnf
container_type = array_type | map_type | optional_type ;
array_type = "array" "<" type_expr ">" ;
map_type = "map" "<" type_expr "," type_expr ">" ;
optional_type = "optional" "<" type_expr ">" ;
```

### Reference Types

```ebnf
ref_type = "ref" "<" "entity" ">" ;
```

## Semantic Rules

### Type Checking

1. **Type Name Uniqueness**: All type definitions must have unique names within a schema
2. **Type Reference Validity**: All type references must refer to defined types or primitives
3. **No Forward References**: Types must be defined before use (topological ordering)

### Struct Rules

1. **Field Name Uniqueness**: Field names must be unique within a struct
2. **Non-Empty Structs**: Structs must have at least one field
3. **Type Validity**: All field types must be valid type expressions

### Variant Rules

1. **Alternative Name Uniqueness**: Alternative names must be unique within a variant
2. **Non-Empty Variants**: Variants must have at least one alternative
3. **Type Validity**: All alternative types must be valid type expressions
4. **Unit Inference**: Alternatives without explicit types are inferred as `unit`

### Enum Rules

1. **Value Uniqueness**: Enum values must be unique within an enum
2. **Non-Empty Enums**: Enums must have at least one value
3. **No Data**: Enums cannot have associated data (use variants instead)

### Nesting Constraints

1. **Leaf Nodes**: The nesting tree must terminate at primitive types, `unit`, or references
2. **No Infinite Recursion**: Direct or indirect recursive type definitions are forbidden (except through `ref<entity>`)

### Container Type Rules

1. **Array Element Validity**: Array element type must be a valid type expression
2. **Map Key/Value Validity**: Both key and value types must be valid type expressions
3. **Optional Type Validity**: Optional element type must be a valid type expression
4. **No Optional Optional**: `optional<optional<T>>` is forbidden (semantic error)

### Reference Type Rules

1. **Entity Reference Only**: `ref` can only contain `entity` as the parameter
2. **Breaking Cycles**: `ref<entity>` breaks recursive type cycles

## Syntax Forms

Carch supports two syntax forms for flexibility:

### Compact Syntax

Single-line definitions for brevity:

```
Position : struct { x: f32, y: f32, z: f32 }
State : variant { idle, running, jumping }
Team : enum { red, blue, green }
```

### Expanded Syntax

Multi-line definitions with optional indentation:

```
Position : struct {
    x: f32,
    y: f32,
    z: f32
}

State : variant {
    idle,
    running,
    jumping
}

Player : struct {
    id: u64,
    position: struct {
        x: f32,
        y: f32
    },
    health: struct {
        current: u32,
        max: u32
    }
}
```

Both forms are semantically equivalent. Mixing forms in a single schema is allowed.

## Reserved Keywords

The following keywords are reserved and cannot be used as identifiers:

**Type constructors:**
```
struct  variant  enum  unit
```

**Container types:**
```
array  map  optional  ref  entity
```

**Primitive types:**
```
str  int  bool
u8  u16  u32  u64
i8  i16  i32  i64
f32  f64
```

**Future reserved (for extensions):**
```
interface  impl  trait  meta  annotation
import  module  namespace
```

## Entity Hierarchy and Levels

In ECS architectures, entities are often organized hierarchically. Carch supports this through:

1. **Entity References**: `ref<entity>` creates relationships between entities
2. **Parent-Child Relationships**: Expressed through fields containing entity references
3. **Component Composition**: Structs nest components to define entity archetypes

**Example hierarchy:**

```
Transform : struct {
    local: struct { x: f32, y: f32, z: f32 },
    parent: optional<ref<entity>>
}

SceneNode : struct {
    entity_id: u64,
    transform: Transform,
    children: array<ref<entity>>
}
```

This enables scene graph representations where entities reference their parents and children.

## Formal Semantics

### Type Expressions

The type system forms a lattice with:
- **Top**: `variant` of all possible types (most general)
- **Bottom**: `unit` (most specific, no information)

**Subtyping**: Not supported (nominal typing only)

**Type Equivalence**: Structural for inline types, nominal for named types

### Component Composition Semantics

**Struct Semantics (AND):**
Given struct `S { a: A, b: B }`, an instance of `S` contains both an `A` and a `B`.

**Variant Semantics (OR):**
Given variant `V { a: A, b: B }`, an instance of `V` contains either an `A` or a `B`, but not both.

**Enum Semantics (CHOICE):**
Given enum `E { a, b, c }`, an instance of `E` is exactly one of `a`, `b`, or `c`.

This maps directly to ECS concepts:
- Components are structs (data AND data)
- Component states are variants (state OR state)
- Component flags are enums (value choice)

---

*This specification is authoritative for Carch language semantics and code generation.*
