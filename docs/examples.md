# Carch IDL Examples

This document provides comprehensive examples demonstrating all features of the Carch Interface Definition Language.

## Table of Contents

1. [Basic Components](#basic-components)
2. [Struct Usage](#struct-usage)
3. [Variant Usage](#variant-usage)
4. [Enum Usage](#enum-usage)
5. [Nested Structures](#nested-structures)
6. [Deep Nesting](#deep-nesting)
7. [Collections](#collections)
8. [Optional Fields](#optional-fields)
9. [Entity References](#entity-references)
10. [Complex Game Entities](#complex-game-entities)
11. [Real-World ECS Patterns](#real-world-ecs-patterns)

## Basic Components

### Simple Data Components

**Compact syntax:**
```
Position : struct { x: f32, y: f32, z: f32 }
Velocity : struct { dx: f32, dy: f32, dz: f32 }
Health : struct { current: u32, max: u32 }
Name : struct { value: str }
Age : struct { years: u32 }
```

**Expanded syntax:**
```
Position : struct {
    x: f32,
    y: f32,
    z: f32
}

Health : struct {
    current: u32,
    max: u32
}

Name : struct {
    value: str
}
```

### Person Component Example

**Compact:**
```
Person : struct { id: u64, name: str, age: u32 }
```

**Expanded:**
```
Person : struct {
    id: u64,
    name: str,
    age: u32
}
```

## Struct Usage

### Multiple Fields

```
Transform : struct {
    position: struct { x: f32, y: f32, z: f32 },
    rotation: struct { x: f32, y: f32, z: f32, w: f32 },
    scale: struct { x: f32, y: f32, z: f32 }
}

RigidBody : struct {
    mass: f32,
    velocity: struct { x: f32, y: f32, z: f32 },
    angular_velocity: struct { x: f32, y: f32, z: f32 },
    friction: f32,
    restitution: f32
}
```

### Inline Struct Definitions

```
Character : struct {
    id: u64,
    stats: struct {
        strength: u32,
        dexterity: u32,
        intelligence: u32,
        vitality: u32
    },
    equipment: struct {
        weapon_id: u32,
        armor_id: u32,
        accessory_id: u32
    }
}
```

## Variant Usage

### Sum Types for Weapons

```
Weapon : variant {
    sword: struct { damage: u32, durability: u32, sharpness: f32 },
    bow: struct { damage: u32, range: f32, arrow_count: u32 },
    staff: struct { magic_power: u32, mana_cost: u32 },
    unarmed: unit
}
```

### AI State Machine

```
AIState : variant {
    idle: unit,
    patrol: struct { waypoints: array<struct { x: f32, y: f32 }>, current_index: u32 },
    chase: struct { target: ref<entity>, speed_multiplier: f32 },
    attack: struct { target: ref<entity>, cooldown: f32 },
    flee: struct { danger_source: ref<entity> }
}
```

### Contact Information

```
Contact : variant {
    email: str,
    phone: str,
    address: struct { street: str, city: str, postal_code: str }
}
```

### Implicit Unit in Variants

```
// These are equivalent:
State1 : variant { idle: unit, running: unit, jumping: unit }
State2 : variant { idle, running, jumping }

// Mixed explicit and implicit:
State3 : variant {
    idle,
    running: struct { speed: f32 },
    jumping: unit
}
```

## Enum Usage

### Simple Categorical Values

```
Team : enum { player, enemy, neutral, ally }
Direction : enum { north, south, east, west }
Rarity : enum { common, uncommon, rare, epic, legendary }
```

### Gender Example

```
Gender : enum { male, female, other }
```

### Game State

```
GameState : enum { menu, playing, paused, game_over }
```

### Day of Week

```
DayOfWeek : enum { monday, tuesday, wednesday, thursday, friday, saturday, sunday }
```

## Nested Structures

### Two-Level Nesting

```
Player : struct {
    id: u64,
    name: str,
    stats: struct {
        health: u32,
        mana: u32,
        stamina: u32
    }
}
```

### Three-Level Nesting

```
Character : struct {
    core: struct {
        id: u64,
        name: str,
        level: u32
    },
    combat: struct {
        stats: struct {
            strength: u32,
            agility: u32,
            intelligence: u32
        },
        equipment: struct {
            weapon: u32,
            armor: u32
        }
    }
}
```

## Deep Nesting

### Four-Level Entity Hierarchy

```
GameEntity : struct {
    identity: struct {
        id: u64,
        type: enum { player, npc, enemy, item, prop }
    },
    spatial: struct {
        transform: struct {
            position: struct { x: f32, y: f32, z: f32 },
            rotation: struct { x: f32, y: f32, z: f32, w: f32 },
            scale: struct { x: f32, y: f32, z: f32 }
        },
        physics: struct {
            velocity: struct { x: f32, y: f32, z: f32 },
            mass: f32
        }
    },
    gameplay: struct {
        attributes: struct {
            stats: struct {
                health: u32,
                mana: u32,
                stamina: u32
            },
            modifiers: struct {
                strength_bonus: i32,
                speed_multiplier: f32
            }
        }
    }
}
```

### Complex Variant Nesting

```
QuestObjective : variant {
    kill: struct {
        target: struct {
            entity_type: str,
            count: u32,
            location: optional<struct { x: f32, y: f32 }>
        }
    },
    collect: struct {
        item: struct {
            item_id: u32,
            quantity: u32,
            quality: enum { poor, normal, rare }
        }
    },
    reach: struct {
        destination: struct { x: f32, y: f32, z: f32 },
        radius: f32
    }
}
```

## Collections

### Arrays

```
// Array of primitives
Inventory : struct {
    item_ids: array<u32>,
    max_capacity: u32
}

// Array of structs
Waypoints : struct {
    points: array<struct { x: f32, y: f32, z: f32 }>,
    loop: bool
}

// Array of variants
Actions : struct {
    queue: array<variant {
        move: struct { x: f32, y: f32 },
        attack: struct { target: ref<entity> },
        wait: struct { duration: f32 }
    }>
}
```

### Maps

```
// Map with primitive values
ScoreBoard : struct {
    player_scores: map<str, u32>
}

// Map with complex values
EntityDatabase : struct {
    entities: map<u64, struct {
        name: str,
        position: struct { x: f32, y: f32 },
        health: u32
    }>
}

// Map with variant values
ItemDefinitions : struct {
    items: map<u32, variant {
        weapon: struct { damage: u32 },
        armor: struct { defense: u32 },
        consumable: struct { effect_id: u32 }
    }>
}
```

### Nested Collections

```
ComplexInventory : struct {
    categories: map<str, array<struct {
        item_id: u32,
        quantity: u32,
        metadata: map<str, str>
    }>>
}
```

## Optional Fields

### Basic Optional Usage

```
Character : struct {
    id: u64,
    name: str,
    nickname: optional<str>,
    guild: optional<u64>
}
```

### Optional Nested Structures

```
Entity : struct {
    id: u64,
    parent: optional<ref<entity>>,
    metadata: optional<struct {
        description: str,
        tags: array<str>
    }>
}
```

### Optional in Variants

```
Spell : variant {
    fireball: struct {
        damage: u32,
        radius: f32,
        burn_duration: optional<f32>
    },
    heal: struct {
        amount: u32,
        over_time: optional<struct { duration: f32, tick_rate: f32 }>
    }
}
```

## Entity References

### Parent-Child Relationships

```
SceneNode : struct {
    entity_id: u64,
    parent: optional<ref<entity>>,
    children: array<ref<entity>>,
    local_transform: struct { x: f32, y: f32, z: f32 }
}
```

### Targeting System

```
Combat : struct {
    current_target: optional<ref<entity>>,
    threat_table: array<struct {
        entity: ref<entity>,
        threat_level: f32
    }>
}
```

### Relationship Network

```
SocialEntity : struct {
    entity_id: u64,
    friends: array<ref<entity>>,
    enemies: array<ref<entity>>,
    faction_leader: optional<ref<entity>>,
    followers: array<ref<entity>>
}
```

## Complex Game Entities

### Complete Player Entity

```
Player : struct {
    // Identity
    id: u64,
    name: str,
    account_id: u64,
    
    // Spatial
    transform: struct {
        position: struct { x: f32, y: f32, z: f32 },
        rotation: struct { yaw: f32, pitch: f32, roll: f32 }
    },
    
    // Combat
    health: struct { current: u32, max: u32 },
    mana: struct { current: u32, max: u32 },
    stats: struct {
        strength: u32,
        agility: u32,
        intelligence: u32,
        vitality: u32
    },
    
    // Equipment
    weapon: optional<variant {
        sword: struct { damage: u32, durability: u32 },
        bow: struct { damage: u32, arrows: u32 },
        staff: struct { magic_power: u32 }
    }>,
    
    // Inventory
    inventory: struct {
        items: array<struct { item_id: u32, quantity: u32, slot: u32 }>,
        capacity: u32,
        gold: u64
    },
    
    // Social
    guild: optional<u64>,
    party: optional<array<ref<entity>>>,
    friends: array<ref<entity>>,
    
    // State
    state: variant {
        idle,
        moving: struct { speed: f32 },
        combat: struct { target: ref<entity> },
        dead: struct { respawn_timer: f32 }
    }
}
```

### Enemy Entity

```
Enemy : struct {
    id: u64,
    type: str,
    transform: struct { x: f32, y: f32, z: f32 },
    health: struct { current: u32, max: u32 },
    
    ai: struct {
        state: variant {
            idle,
            patrol: struct { waypoints: array<struct { x: f32, y: f32 }> },
            chase: struct { target: ref<entity>, aggro_range: f32 },
            attack: struct { target: ref<entity>, attack_cooldown: f32 }
        },
        aggro_radius: f32,
        patrol_speed: f32,
        chase_speed: f32
    },
    
    loot_table: array<struct {
        item_id: u32,
        drop_chance: f32,
        quantity_min: u32,
        quantity_max: u32
    }>,
    
    team: enum { enemy, neutral, boss }
}
```

### Projectile Entity

```
Projectile : struct {
    id: u64,
    owner: ref<entity>,
    
    transform: struct {
        position: struct { x: f32, y: f32, z: f32 },
        direction: struct { x: f32, y: f32, z: f32 }
    },
    
    physics: struct {
        velocity: f32,
        lifetime: f32,
        gravity_scale: f32
    },
    
    damage: struct {
        amount: u32,
        type: enum { physical, magical, fire, ice, lightning },
        pierce: bool
    },
    
    effects: array<variant {
        explosion: struct { radius: f32, damage: u32 },
        poison: struct { duration: f32, damage_per_second: u32 },
        knockback: struct { force: f32 }
    }>
}
```

## Real-World ECS Patterns

### Separation of Data and Behavior

Components store data, systems operate on components:

```
// Pure data components
Position : struct { x: f32, y: f32, z: f32 }
Velocity : struct { dx: f32, dy: f32, dz: f32 }
Acceleration : struct { ax: f32, ay: f32, az: f32 }

// System will query entities with Position + Velocity
// and update Position based on Velocity
```

### Component Composition

Build complex entities from simple components:

```
// Basic components
Transform : struct { x: f32, y: f32, z: f32 }
Renderable : struct { sprite_id: u32, layer: u32 }
Collidable : struct { radius: f32, solid: bool }
Health : struct { current: u32, max: u32 }
Input : struct { up: bool, down: bool, left: bool, right: bool }

// Player = Transform + Renderable + Collidable + Health + Input
// Enemy = Transform + Renderable + Collidable + Health + AI
// Projectile = Transform + Renderable + Collidable + Lifetime
// Decoration = Transform + Renderable
```

### Tag Components

Use unit structs or enums for tagging:

```
Player : struct { marker: unit }
Enemy : struct { marker: unit }
Dead : struct { marker: unit }

// Or use enums:
EntityType : enum { player, enemy, npc, item, decoration }
```

### State Machines with Variants

```
CharacterState : variant {
    grounded: struct {
        movement: variant {
            idle,
            walking: struct { speed: f32 },
            running: struct { speed: f32 },
            crouching
        }
    },
    airborne: struct {
        vertical_velocity: f32,
        double_jump_available: bool
    },
    climbing: struct {
        surface: ref<entity>,
        height: f32
    }
}
```

### Event Components

Components that represent events (cleared each frame):

```
DamageEvent : struct {
    target: ref<entity>,
    amount: u32,
    source: ref<entity>,
    type: enum { physical, magical, true }
}

CollisionEvent : struct {
    entity_a: ref<entity>,
    entity_b: ref<entity>,
    point: struct { x: f32, y: f32, z: f32 },
    normal: struct { x: f32, y: f32, z: f32 }
}
```

### Hierarchical Transforms

```
LocalTransform : struct {
    position: struct { x: f32, y: f32, z: f32 },
    rotation: struct { x: f32, y: f32, z: f32, w: f32 },
    scale: struct { x: f32, y: f32, z: f32 }
}

GlobalTransform : struct {
    matrix: array<f32>  // 16 floats for 4x4 matrix
}

Parent : struct {
    entity: ref<entity>
}

Children : struct {
    entities: array<ref<entity>>
}

// System computes GlobalTransform from LocalTransform and Parent
```

---

These examples demonstrate the full expressiveness of Carch IDL for defining ECS architectures. Mix and match patterns to suit your specific game or simulation needs.
