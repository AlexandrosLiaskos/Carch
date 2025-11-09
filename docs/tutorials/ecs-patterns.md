# ECS Patterns with Carch

Common Entity-Component-System patterns using Carch schemas.

## Basic Component Design

### Single Responsibility

Each component should represent one aspect of an entity:

```carch
Transform : struct { position: struct { x: f32, y: f32, z: f32 } }
Health : struct { current: u32, max: u32 }
Velocity : struct { dx: f32, dy: f32, dz: f32 }
```

### Data-Oriented Design

Keep components simple and data-focused:

```carch
// Good: Pure data
Sprite : struct {
    texture_id: u32,
    layer: i32,
    visible: bool
}

// Avoid: Mixing logic concerns
// (Logic belongs in systems, not components)
```

## Entity Archetypes

### Player Entity

```carch
Player : struct {
    core: struct { id: u64, name: str, level: u32 },
    transform: Transform,
    health: Health,
    inventory: Inventory,
    state: PlayerState
}

PlayerState : variant {
    idle, 
    moving: struct { speed: f32 },
    attacking: struct { target: ref<entity> }
}
```

### Enemy Entity

```carch
Enemy : struct {
    id: u64,
    type: str,
    transform: Transform,
    health: Health,
    ai: AIState,
    loot: array<u32>
}

AIState : variant {
    idle,
    patrol: struct { waypoints: array<Position> },
    chase: struct { target: ref<entity> },
    attack: struct { target: ref<entity> }
}
```

## Component Relationships

### Parent-Child with Entity Refs

```carch
SceneNode : struct {
    transform: Transform,
    parent: optional<ref<entity>>,
    children: array<ref<entity>>
}
```

### Component Arrays

```carch
Inventory : struct {
    items: array<struct {
        item_id: u32,
        quantity: u32,
        slot: u32
    }>,
    capacity: u32
}
```

## State Machines

### Animation State

```carch
AnimationState : variant {
    idle: struct { frame: u32 },
    walk: struct { frame: u32, speed: f32 },
    run: struct { frame: u32, speed: f32 },
    jump: struct { frame: u32, apex_reached: bool },
    attack: struct { frame: u32, attack_type: enum { light, heavy } }
}
```

### Game Flow

```carch
GameState : variant {
    menu: struct { selected: u32 },
    playing: struct { paused: bool },
    game_over: struct { score: u64, time: f32 }
}
```

## Event Systems

```carch
InputEvent : variant {
    key_press: struct { key_code: u32 },
    mouse_move: struct { x: i32, y: i32 },
    mouse_click: struct { button: u32, x: i32, y: i32 }
}

GameEvent : variant {
    player_damaged: struct { amount: u32, source: ref<entity> },
    enemy_killed: struct { enemy: ref<entity>, exp: u64 },
    item_collected: struct { item_id: u32 }
}
```

## Performance Considerations

### Cache-Friendly Layouts

```carch
// Good: Flat structure
Position : struct { x: f32, y: f32, z: f32 }

// Less ideal: Deep nesting (if accessed frequently)
Transform : struct {
    position: struct {
        coords: struct { x: f32, y: f32, z: f32 }
    }
}
```

### Avoid Deep Nesting in Hot Components

Components accessed every frame should be flat.

## See Also

- [Advanced Types](advanced-types.md) - Complex type patterns
- [Integration Guide](integration-guide.md) - Using with ECS libraries
