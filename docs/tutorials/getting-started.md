# Getting Started with Carch

Welcome to Carch! This tutorial will help you get started with the Carch IDL compiler for defining game component schemas.

> **Note**: This tutorial is for Carch version 0.0.1, an initial development release. APIs and generated code format may change in future versions.

## Installation

### Pre-built Binaries

Download the latest release for your platform from the [Releases page](https://github.com/AlexandrosLiaskos/Carch/releases):

- **Linux**: `carch-linux-x64.tar.gz`
- **Windows**: `carch-windows-x64.zip`
- **macOS**: `carch-macos-x64.tar.gz` or `carch-macos-arm64.tar.gz`

Extract the archive and add the `carch` executable to your PATH.

### Building from Source

```bash
git clone https://github.com/AlexandrosLiaskos/Carch.git
cd Carch
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
```

## Your First Schema

Create a file named `components.carch`:

```carch
// Position component with 3D coordinates
Position : struct {
    x: f32,
    y: f32,
    z: f32
}

// Velocity component
Velocity : struct {
    dx: f32,
    dy: f32,
    dz: f32
}

// Health component
Health : struct {
    current: u32,
    max: u32
}
```

## Compiling Your Schema

Run the Carch compiler:

```bash
carch components.carch
```

This generates `generated/components.h` with C++ code:

```cpp
#pragma once

#include <cstdint>

namespace game {

struct Position {
    float x;
    float y;
    float z;
};

struct Velocity {
    float dx;
    float dy;
    float dz;
};

struct Health {
    uint32_t current;
    uint32_t max;
};

} // namespace game
```

## Using Generated Code

Include the generated header in your C++ project:

```cpp
#include "components.h"

int main() {
    // Create components
    game::Position pos{0.0f, 0.0f, 0.0f};
    game::Velocity vel{1.0f, 0.0f, 0.0f};
    game::Health health{100, 100};
    
    // Use in your ECS
    // entity.add_component(pos);
    // entity.add_component(vel);
    // entity.add_component(health);
    
    return 0;
}
```

## Type System Basics

### Primitive Types

Carch supports 14 primitive types:

- **Integers**: `u8`, `u16`, `u32`, `u64`, `i8`, `i16`, `i32`, `i64`
- **Floats**: `f32`, `f64`
- **Other**: `bool`, `str`, `bytes`, `unit`

### Structs

Define product types (AND logic):

```carch
Player : struct {
    id: u64,
    name: str,
    position: Position,
    health: Health
}
```

### Variants

Define sum types (OR logic):

```carch
Weapon : variant {
    sword: struct { damage: u32 },
    bow: struct { damage: u32, arrows: u32 },
    unarmed: unit
}
```

Generates `std::variant` in C++.

### Enums

Simple enumerations:

```carch
Team : enum {
    red,
    blue,
    green
}
```

Generates `enum class` in C++.

### Containers

- **Arrays**: `array<T>`
- **Maps**: `map<K, V>`
- **Optional**: `optional<T>`

Example:

```carch
Inventory : struct {
    items: array<u32>,
    metadata: map<str, str>,
    equipped_weapon: optional<Weapon>
}
```

### Entity References

Break circular dependencies:

```carch
Node : struct {
    value: u32,
    parent: optional<ref<entity>>,
    children: array<ref<entity>>
}
```

## Command-Line Options

```bash
# Specify output directory
carch -o output/ components.carch

# Set namespace
carch --namespace myproject components.carch

# Multiple input files
carch components.carch entities.carch items.carch
```

## Next Steps

- [ECS Patterns](ecs-patterns.md) - Learn common ECS design patterns
- [Advanced Types](advanced-types.md) - Master the type system
- [Integration Guide](integration-guide.md) - Integrate Carch into your build system

## Common Patterns

### Component Composition

```carch
Transform : struct {
    position: Position,
    rotation: struct { x: f32, y: f32, z: f32, w: f32 },
    scale: struct { x: f32, y: f32, z: f32 }
}

Renderable : struct {
    mesh_id: u32,
    material_id: u32,
    visible: bool
}

PhysicsBody : struct {
    velocity: Velocity,
    mass: f32,
    friction: f32
}
```

### State Machines with Variants

```carch
PlayerState : variant {
    idle: unit,
    running: struct { speed: f32 },
    jumping: struct { velocity: f32 },
    falling: struct { velocity: f32 },
    dead: unit
}
```

### Optional Components

```carch
Character : struct {
    core: struct {
        id: u64,
        name: str
    },
    equipment: optional<struct {
        weapon: Weapon,
        armor: u32
    }>,
    mount: optional<ref<entity>>
}
```

## Troubleshooting

### Compilation Errors

If you see "undefined type" errors, ensure:
1. All referenced types are defined
2. Forward references use `ref<entity>`
3. Types are defined before use (except with refs)

### Generated Code Won't Compile

Check for:
1. Circular dependencies (use `ref<entity>` to break cycles)
2. Reserved C++ keywords as identifiers
3. Nested `optional<optional<T>>` (not allowed)

## Getting Help

Carch is developed by Alexandros Liaskos. For questions or support, please use the following resources:

- [Documentation](../docs/) - Complete reference
- [Examples](../examples/) - Sample schemas
- [GitHub Issues](https://github.com/AlexandrosLiaskos/Carch/issues) - Report bugs
- [Discussions](https://github.com/AlexandrosLiaskos/Carch/discussions) - Ask questions
- **Contact**: [alexliaskos@geol.uoa.gr](mailto:alexliaskos@geol.uoa.gr)