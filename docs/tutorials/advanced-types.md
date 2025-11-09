# Advanced Type System Features

Master Carch's advanced type system capabilities.

## Nested Structures

### Inline Anonymous Types

```carch
Player : struct {
    core: struct {
        id: u64,
        name: str,
        stats: struct {
            strength: u32,
            agility: u32,
            intelligence: u32
        }
    }
}
```

### Named vs Inline Types

Use named types for reusability:

```carch
// Named (reusable)
Stats : struct { strength: u32, agility: u32 }

Player : struct { stats: Stats }
Enemy : struct { stats: Stats }

// Inline (one-time use)
Config : struct {
    graphics: struct { width: u32, height: u32 }
}
```

## Variant Design Patterns

### Tagged Unions

```carch
Damage : variant {
    physical: struct { amount: u32, armor_penetration: f32 },
    magical: struct { amount: u32, element: enum { fire, ice, lightning } },
    true_damage: struct { amount: u32 }
}
```

### Nested Variants

```carch
SpellEffect : variant {
    heal: struct {
        amount: u32,
        target: variant {
            single: ref<entity>,
            area: struct { radius: f32 }
        }
    },
    damage: Damage,
    buff: struct { stat: enum { str, agi, int }, value: f32 }
}
```

## Container Types

### Arrays

```carch
// Simple array
items: array<u32>

// Struct array
waypoints: array<struct { x: f32, y: f32 }>

// Nested arrays
grid: array<array<u32>>
```

### Maps

```carch
// String keys
metadata: map<str, str>

// Integer keys
lookup: map<u64, struct { name: str, value: u32 }>

// Nested maps
cache: map<str, map<str, u32>>
```

### Optionals

```carch
// Optional primitives
nickname: optional<str>

// Optional structs
parent: optional<ref<entity>>

// Optional in containers
items: array<optional<u32>>
tags: map<str, optional<str>>

// NOT ALLOWED: nested optionals
// bad: optional<optional<str>>
```

## Entity References

Breaking circular dependencies:

```carch
// Self-referential structure
Node : struct {
    value: u32,
    next: optional<ref<entity>>
}

// Mutual references
Parent : struct {
    children: array<ref<entity>>
}

Child : struct {
    parent: ref<entity>
}
```

## Type Composition

### Building Complex Types

```carch
// Base types
Position : struct { x: f32, y: f32, z: f32 }
Rotation : struct { x: f32, y: f32, z: f32, w: f32 }
Scale : struct { x: f32, y: f32, z: f32 }

// Composed type
Transform : struct {
    position: Position,
    rotation: Rotation,
    scale: Scale
}

// Higher-level composition
Entity : struct {
    id: u64,
    transform: Transform,
    components: map<str, Variant<...>>
}
```

## Syntax Forms

### Compact vs Expanded

```carch
// Compact (one line)
Point : struct { x: f32, y: f32 }

// Expanded (multi-line)
Point : struct {
    x: f32,
    y: f32
}

// Mixed (as appropriate)
Entity : struct {
    id: u64,
    position: struct { x: f32, y: f32, z: f32 },
    data: struct {
        health: u32,
        mana: u32,
        stamina: u32
    }
}
```

## Best Practices

1. **Keep components flat** for performance
2. **Use named types** for shared structures
3. **Use variants** for polymorphism, not enums with unit fields
4. **Use optionals** for truly optional data
5. **Use entity refs** to break cycles
6. **Limit nesting depth** to 3-4 levels max

## See Also

- [Specification](../specification.md) - Complete type reference
- [ECS Patterns](ecs-patterns.md) - Practical patterns
