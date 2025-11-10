# Carch IDL

[![CI Build Status](https://github.com/AlexandrosLiaskos/Carch/actions/workflows/ci.yml/badge.svg)](https://github.com/AlexandrosLiaskos/Carch/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://github.com/AlexandrosLiaskos/Carch/blob/main/LICENSE)
[![Version](https://img.shields.io/badge/version-0.0.1-blue.svg)](https://github.com/AlexandrosLiaskos/Carch/releases)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)](https://github.com/AlexandrosLiaskos/Carch)

<div align="center">
<img width="400" alt="Carch Logo" src="https://github.com/user-attachments/assets/516b48c7-49a1-45b8-8817-c705e0f3c270" />
</div>



**Carch** (Component Architecture) is an Interface Definition Language designed specifically for defining Entity-Component System (ECS) architectures in C++. It provides a clean, expressive syntax for declaring component schemas with strong type safety and algebraic data types.

## Overview

Carch enables you to define ECS architectures using a declarative syntax featuring:

- **Structs** (AND logic): Combine multiple fields into a single component
- **Variants** (OR logic): Define sum types with multiple alternatives
- **Enums**: Simple discriminated unions for categorical values
- **Rich Type System**: Primitives, containers, references, and user-defined types
- **Dual Syntax**: Both compact single-line and expanded multi-line forms

The Carch compiler generates idiomatic C++ code from your schemas, producing structs, `std::variant` types, and enum classes ready for use in your ECS implementation.

> **Note**: This is version 0.0.1, an initial development release. APIs and generated code format may change before version 1.0.0. See the [roadmap](docs/roadmap.md) for planned features.

## Quick Start

### Installation

```bash
# Clone the repository
git clone https://github.com/AlexandrosLiaskos/Carch.git
cd Carch

# Build with CMake
mkdir build && cd build
cmake ..
cmake --build .

# Install (optional)
cmake --install .
```

### Basic Usage

Create a `.carch` file defining your components:

```
// person.carch
Person : struct { id: u64, name: str, age: u32 }
Gender : enum { male, female, other }
Contact : variant { email: str, phone: str }
```

Compile to C++:

```bash
carch person.carch -o generated/
```

Use the generated code:

```cpp
#include "generated/person.h"

Person player{1, "Alice", 25};
Gender gender = Gender::female;
// Variant construction using alternative struct
Contact contact = Contact_Email{std::string("alice@example.com")};
// Or using std::variant in-place construction:
// Contact contact{std::in_place_type<Contact_Email>, "alice@example.com"};
```

## Troubleshooting CI Failures

### How to view CI logs
- Navigate to GitHub Actions tab
- Click on failed workflow run
- Expand test steps to see detailed output

### Common CI failure causes
- Uncommitted/unpushed changes
- Branch mismatch (CI running on different branch)
- Cache issues (solution: re-run workflow)
- Test syntax errors (check for typed entity references)
- Compiler implementation bugs

### How to reproduce CI failures locally
- Use exact same CMake flags as CI: `-DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON -DBUILD_BENCHMARKS=OFF`
- Run tests with: `ctest --output-on-failure --verbose`
- Check which tests are registered: `ctest -N`

### Entity reference syntax reminder
- Always use `ref<entity>`, never `ref<TypeName>`
- This applies to all test code, example files, and dynamically generated test strings

## Core Concepts

### Struct (AND Logic)

Structs combine multiple fields together. All fields must be present.

```
Transform : struct {
    position: struct { x: f32, y: f32, z: f32 },
    rotation: struct { x: f32, y: f32, z: f32, w: f32 },
    scale: struct { x: f32, y: f32, z: f32 }
}
```

Compact form:

```
Position : struct { x: f32, y: f32, z: f32 }
```

### Variant (OR Logic)

Variants represent a choice between alternatives. One alternative must be selected.

```
Weapon : variant {
    sword: struct { damage: u32, durability: u32 },
    bow: struct { damage: u32, range: f32 },
    staff: struct { magic_power: u32 }
}
```

Variants with unit (no data):

```
AIState : variant { idle: unit, patrol, chase, attack: unit }
```

### Enum

Enums define simple categorical values without associated data.

```
Team : enum { player, enemy, neutral }
Direction : enum { north, south, east, west }
```

### Type System

**Primitives:**
- Strings: `str`
- Integers: `int`, `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`
- Floats: `f32`, `f64`
- Boolean: `bool`
- Unit: `unit` (empty type)

**Containers:**
- Arrays: `array<T>`
- Maps: `map<K, V>`
- Optional: `optional<T>`

**References:**
- Entity references: `ref<entity>`

## Syntax Reference

### Compact Syntax

Single-line definitions for brevity:

```
Health : struct { current: u32, max: u32 }
State : variant { idle, moving, attacking }
Color : enum { red, green, blue }
```

### Expanded Syntax

Multi-line definitions for readability:

```
Player : struct {
    id: u64,
    name: str,
    health: struct {
        current: u32,
        max: u32
    },
    inventory: array<struct { item_id: u32, quantity: u32 }>,
    team: enum { player, enemy, neutral }
}
```

### Nesting

Structs and variants can be arbitrarily nested:

```
Entity : struct {
    core: struct {
        id: u64,
        name: str
    },
    components: struct {
        transform: struct { x: f32, y: f32, z: f32 },
        renderable: variant {
            sprite: struct { texture_id: u32 },
            mesh: struct { model_id: u32 }
        }
    }
}
```

## Examples

### Entity-Component Definition

```
// Core components
Transform : struct { x: f32, y: f32, z: f32 }
Velocity : struct { dx: f32, dy: f32, dz: f32 }
Health : struct { current: u32, max: u32 }

// Complex component with variants
Weapon : variant {
    melee: struct { damage: u32, range: f32 },
    ranged: struct { damage: u32, ammo: u32 }
}

// Complete entity archetype
Player : struct {
    id: u64,
    transform: Transform,
    velocity: Velocity,
    health: Health,
    weapon: Weapon,
    inventory: array<u32>
}
```

### Comparison with Traditional ECS

**Traditional C++ ECS:**
```cpp
struct Transform { float x, y, z; };
struct Health { uint32_t current, max; };
// Components defined separately, relationships unclear
```

**With Carch:**
```
Transform : struct { x: f32, y: f32, z: f32 }
Health : struct { current: u32, max: u32 }
Player : struct { transform: Transform, health: Health }
// Clear composition and relationships
```

## Use Cases

- **Game Development**: Define entities, components, and their relationships
- **Simulation Systems**: Model complex state machines with variants
- **Data Modeling**: Express hierarchical data with strong typing
- **Code Generation**: Automatically generate C++ boilerplate from schemas

## Documentation

- [Language Specification](docs/specification.md) - Formal grammar and semantics
- [Examples](docs/examples.md) - Comprehensive usage examples
- [Design Decisions](docs/design-decisions.md) - Rationale behind language design
- [API Reference](docs/api-reference.md) - Using Carch as a library
- [Troubleshooting](docs/troubleshooting.md) - Common issues and solutions

## Contributing

Contributions are welcome! We'd love your help improving Carch.

- **Read the guidelines**: [CONTRIBUTING.md](CONTRIBUTING.md)
- **Follow our Code of Conduct**: [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md)
- **Report bugs**: [GitHub Issues](https://github.com/AlexandrosLiaskos/Carch/issues)
- **Discuss ideas**: [GitHub Discussions](https://github.com/AlexandrosLiaskos/Carch/discussions)
- **Developer**: Alexandros Liaskos ([alexliaskos@geol.uoa.gr](mailto:alexliaskos@geol.uoa.gr))

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Quick Links

- 📖 [Documentation](docs/)
- 💡 [Examples](examples/)
- 🤝 [Contributing](CONTRIBUTING.md)
- 📜 [License](LICENSE)
- 📋 [Changelog](CHANGELOG.md)
- 🗺️ [Roadmap](docs/roadmap.md)
- 🐛 [Report Issues](https://github.com/AlexandrosLiaskos/Carch/issues)
- 💬 [Discussions](https://github.com/AlexandrosLiaskos/Carch/discussions)
