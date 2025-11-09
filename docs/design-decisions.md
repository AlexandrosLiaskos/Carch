# Carch Design Decisions

This document explains the rationale behind key design decisions in the Carch Interface Definition Language.

## Table of Contents

1. [Why Struct/Variant/Enum Distinction](#why-structvariantenum-distinction)
2. [Implicit vs Explicit Unit in Variants](#implicit-vs-explicit-unit-in-variants)
3. [Compact vs Expanded Syntax](#compact-vs-expanded-syntax)
4. [Type System Choices](#type-system-choices)
5. [Mapping to ECS Concepts](#mapping-to-ecs-concepts)
6. [Comparison with Other IDLs](#comparison-with-other-idls)
7. [Future Extensibility](#future-extensibility)

## Why Struct/Variant/Enum Distinction

### Algebraic Data Types

Carch is built on the foundation of algebraic data types (ADTs), which provide mathematical precision and type safety:

- **Structs (Product Types)**: Represent AND relationships. `struct { a: A, b: B }` means "A AND B together"
- **Variants (Sum Types)**: Represent OR relationships. `variant { a: A, b: B }` means "A OR B, not both"
- **Enums (Simple Discriminated Unions)**: Special case of sum types with no associated data

### Why Three Constructs?

**Why not just structs?**
- Structs cannot express alternatives or choice
- Cannot represent state machines or polymorphic data
- Would require error-prone tag fields and unions

**Why not just variants?**
- Variants are more complex than needed for simple flag-like data
- Enums provide clearer intent for simple categorization
- Enums map more naturally to C++ enum class

**Why enums when variants can do the same?**
- **Clarity**: `enum { red, green, blue }` vs `variant { red: unit, green: unit, blue: unit }`
- **Intent**: Enums signal "simple choice", variants signal "data alternatives"
- **Code Generation**: Enums generate simpler C++ code (enum class vs variant with monostate)
- **Ergonomics**: Less typing for common case of simple categorization

### Type Safety Benefits

The distinction enables compile-time guarantees:

```
// Compile error: cannot mix incompatible types
Position : struct { x: f32, y: f32 }
State : enum { idle, running }
p : Position = State::idle  // TYPE ERROR
```

```
// Exhaustiveness checking in variants
Weapon : variant { sword: Sword, bow: Bow, staff: Staff }
// Code gen can ensure all alternatives are handled
```

## Implicit vs Explicit Unit in Variants

### The Decision

Variant alternatives without an explicit type are treated as having type `unit`:

```
State : variant {
    idle,           // implicitly idle: unit
    running,        // implicitly running: unit
    jumping: unit   // explicitly jumping: unit
}
```

### Rationale

**Pros of Implicit Unit:**
- **Conciseness**: Less typing for common case of stateless alternatives
- **Readability**: `idle` is clearer than `idle: unit`
- **Familiarity**: Matches Rust enum syntax, feels natural to programmers

**Cons Considered:**
- **Ambiguity**: Could be confused with forward declarations
- **Inconsistency**: Mixing implicit and explicit feels inconsistent

**Why We Chose Implicit:**
1. State machines are common in game development
2. Most state machine states are stateless (unit)
3. Forcing `: unit` everywhere creates line noise
4. Context makes intent clear (inside variant body)
5. Aligns with Rust, which has proven ergonomics

### Alternative Considered

Require all alternatives to have explicit types:

```
State : variant { idle: unit, running: unit, jumping: unit }
```

Rejected because it's verbose for little benefit. The context (variant body) makes the meaning unambiguous.

## Compact vs Expanded Syntax

### Design Philosophy

Carch supports both single-line (compact) and multi-line (expanded) syntax:

```
// Compact
Position : struct { x: f32, y: f32, z: f32 }

// Expanded
Position : struct {
    x: f32,
    y: f32,
    z: f32
}
```

### Rationale

**Why support both?**
1. **Simple cases**: Compact syntax reduces file length and improves scanability
2. **Complex cases**: Expanded syntax improves readability for deeply nested structures
3. **Developer choice**: Different contexts have different optimal styles
4. **Refactoring**: Easy to start compact, expand later without semantic changes

**Implementation Impact:**
- Lexer must handle both newline-significant and newline-insignificant modes
- Parser must accept both forms equivalently
- Slightly more complex grammar, but worth the flexibility

**Precedent:**
- JSON allows both compact and expanded
- Rust allows single-line and multi-line struct definitions
- C++ allows both styles

### Guidelines (Not Enforced)

Recommended style:
- Use compact for ≤3 fields with primitive types
- Use expanded for >3 fields or nested structures
- Use expanded for variants and enums with >2 alternatives
- Be consistent within a file

## Type System Choices

### Primitive Types

**Why these specific primitives?**

| Type | Rationale |
|------|-----------|
| `str` | UTF-8 strings are essential for names, text, identifiers |
| `int` | Default signed integer for common cases (maps to i32) |
| `u8`-`u64` | Explicit unsigned sizes for memory control (crucial in games) |
| `i8`-`i64` | Explicit signed sizes for memory control |
| `f32`, `f64` | IEEE 754 floats, f32 dominant in games (GPU compatibility) |
| `bool` | Boolean logic essential for flags and conditions |
| `unit` | Zero-size type for variants and empty markers |

**Why no `char` or `byte`?**
- `char` is ambiguous (1 byte? UTF-32 codepoint?)
- Use `u8` for byte, `str` for text
- Avoids encoding confusion

**Why no `usize` or `isize`?**
- Platform-dependent sizes hurt cross-platform ECS
- Explicit sizes (`u64` for entity IDs) are clearer
- Can add later if needed

### Container Types

**Why these containers?**

- **`array<T>`**: Dynamic sequences are ubiquitous (inventories, waypoints, children)
- **`map<K, V>`**: Associative data common (entity lookups, item databases)
- **`optional<T>`**: Nullable data is common (parent references, optional components)

**Why not `set<T>`?**
- Less common in ECS patterns
- Can be represented as `array<T>` with semantic convention
- Reduces complexity

**Why not fixed-size arrays `[T; N]`?**
- Adds complexity (type-level integers)
- Less flexible (need to know size at schema time)
- Could add in future version

### Reference Types

**`ref<entity>` Design:**

Decision: Only allow `ref<entity>`, not `ref<T>` for arbitrary types.

**Rationale:**
- ECS entities are special: they're the top-level organizational unit
- Arbitrary references create complex ownership semantics
- Keep Carch focused on ECS, not general-purpose data modeling
- `ref<entity>` is sufficient for scene graphs, targeting, relationships

**Why `ref<entity>` and not just `u64`?**
- **Intent**: Makes entity references explicit in schema
- **Type Safety**: Can't accidentally use regular integer as entity ID
- **Code Generation**: Can generate special types (strong typedef, wrapper class)

## Mapping to ECS Concepts

### ECS Architecture Primer

**Entity**: Unique ID (integer), container for components
**Component**: Pure data, no behavior
**System**: Logic that operates on entities with specific component combinations

### How Carch Maps to ECS

| Carch Construct | ECS Concept | Purpose |
|-----------------|-------------|---------|
| Top-level type def | Component or Entity archetype | Define reusable data schemas |
| Struct | Component (usually) | Hold related data together |
| Variant | Component state | Represent mutually exclusive states |
| Enum | Component flag/category | Represent simple choices |
| `ref<entity>` | Entity reference | Relationships between entities |

### Example Mapping

```
// Component definitions
Position : struct { x: f32, y: f32, z: f32 }
Velocity : struct { dx: f32, dy: f32, dz: f32 }

// Entity archetype
Player : struct {
    id: u64,
    position: Position,
    velocity: Velocity,
    health: struct { current: u32, max: u32 }
}

// System (in C++ code, not Carch)
// void PhysicsSystem(Position& pos, const Velocity& vel, float dt) {
//     pos.x += vel.dx * dt;
//     pos.y += vel.dy * dt;
//     pos.z += vel.dz * dt;
// }
```

### Philosophy

Carch is **data-definition focused**:
- Define component schemas
- Define entity archetypes (compositions of components)
- Define relationships via entity references

Carch is **NOT**:
- A complete ECS framework (no system definition)
- A game engine (just data schemas)
- A query language (no filtering syntax)

**Rationale**: Keep Carch focused on what it does well (data schemas) and integrate with existing ECS frameworks for system logic.

## Comparison with Other IDLs

### Protocol Buffers

**Similarities:**
- Schema-first design
- Code generation to multiple languages
- Strong typing

**Differences:**
- **Protobuf**: Designed for serialization, RPC, backwards compatibility
- **Carch**: Designed for ECS in-memory data structures, algebraic types
- **Protobuf**: No sum types (use oneof, less ergonomic)
- **Carch**: No wire format, versioning, or RPC features

**When to use each:**
- Protobuf: Network protocols, persistence, cross-language communication
- Carch: Game ECS architectures, C++ data modeling

### FlatBuffers

**Similarities:**
- Schema definition language
- Focus on performance (games)
- C++ code generation

**Differences:**
- **FlatBuffers**: Zero-copy serialization, memory layout control
- **Carch**: In-memory data structures, algebraic types
- **FlatBuffers**: Access via accessors (indirection)
- **Carch**: Direct struct access (POD when possible)

**When to use each:**
- FlatBuffers: Serialization, asset loading, binary formats
- Carch: Runtime ECS components, type-safe composition

### Cap'n Proto

**Similarities:**
- Schema language
- Performance focus
- Capability-based (in Cap'n Proto)

**Differences:**
- **Cap'n Proto**: RPC, serialization, zero-copy
- **Carch**: ECS data modeling only
- **Cap'n Proto**: Complex versioning, generics
- **Carch**: Simpler, focused feature set

### Rust Enums/Structs

**Biggest Inspiration:**

Carch borrows heavily from Rust's type system:
- Struct syntax similar to Rust
- Variant ~ Rust enum with data
- Implicit unit in variants ~ Rust enum variants

**Differences:**
- Rust is a full language; Carch is an IDL
- Rust has traits, generics, lifetimes; Carch has none
- Carch targets C++ code generation

## Future Extensibility

### Planned Extensions

**1. Metadata/Annotations**

```
@serialize
@editor_visible
Player : struct {
    @range(0, 100)
    health: u32
}
```

Enables: editor integration, serialization control, validation rules

**2. Code Generation Targets**

Currently: C++ only
Future: Rust, C#, TypeScript for multi-language game engines

**3. Generics/Templates**

```
Pair<T, U> : struct { first: T, second: U }
Position : Pair<f32, f32>
```

Enables: reusable container definitions, less duplication

**4. Imports/Modules**

```
import components.common.*
import math.Vector3
```

Enables: large-scale projects, code organization, reusability

**5. Schema Evolution**

Versioning and migration support:
```
@version(2)
Player : struct {
    id: u64,
    @deprecated(use: "transform.position")
    x: f32
}
```

Enables: backwards compatibility, gradual migration

### Non-Goals

**Will NOT add:**
- **Inheritance**: Use composition, not inheritance (ECS philosophy)
- **Behavior/Methods**: Components are pure data, systems provide behavior
- **Computation/Expressions**: Schemas are declarative, not computational
- **Query Language**: Integrate with existing ECS query systems
- **Full Serialization**: Use existing libraries (FlatBuffers, protobuf, JSON)

### Design Principles for Extensions

1. **Stay Focused**: Every feature must serve ECS data modeling
2. **Simplicity**: Prefer simple solutions over comprehensive ones
3. **C++ Compatibility**: Extensions must map cleanly to C++
4. **Backwards Compatibility**: New features shouldn't break existing schemas
5. **Optional Complexity**: Advanced features should be opt-in

---

These design decisions prioritize developer ergonomics, type safety, and ECS-specific needs while maintaining simplicity and clarity. The language is intentionally constrained to do one thing well: define ECS component architectures.
