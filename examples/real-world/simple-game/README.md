# Simple 2D Game Example

A complete example demonstrating how to use Carch-generated components in a real game project.

## Components

Defined in `components.carch`:

- **Transform**: Position, rotation, scale
- **Sprite**: Rendering data
- **RigidBody**: Physics properties
- **Collider**: Collision shapes (circle, box, polygon)
- **Health**: Hit points and regeneration
- **Player**: Input state and score
- **Enemy**: AI state machine
- **Projectile**: Bullet/missile data
- **PowerUp**: Various power-up types

## AI State Machine

The Enemy component uses a variant-based state machine:

```carch
AIState : variant {
    idle: unit,
    patrol: struct { waypoints: array<...>, current_index: u32 },
    chase: struct { target: ref<entity> },
    attack: struct { target: ref<entity>, cooldown: f32 }
}
```

In C++, we use `std::visit` to handle each state:

```cpp
std::visit([&](auto& state) {
    using T = std::decay_t<decltype(state)>;
    if constexpr (std::is_same_v<T, game::AIState_idle>) {
        // Idle behavior
    } else if constexpr (std::is_same_v<T, game::AIState_chase>) {
        // Chase behavior
    }
    // ...
}, enemy->ai_state);
```

## Building

```bash
# Configure
cmake -B build

# Build (will compile schema automatically)
cmake --build build

# Run
./build/simple_game
```

## Code Structure

- `components.carch` - Schema definitions
- `main.cpp` - Game implementation
- `CMakeLists.txt` - Build configuration

## Key Concepts Demonstrated

1. **Component Design**: Each component represents one aspect of game entities
2. **Variant Usage**: Type-safe state machines with variants
3. **Entity References**: Using `ref<entity>` for relationships
4. **Container Types**: Arrays for collections
5. **Build Integration**: Automatic code generation in CMake

## Extending

To add new components:

1. Edit `components.carch`
2. Rebuild (schema will be recompiled automatically)
3. Use new components in `main.cpp`

## Notes

- This is a minimal ECS implementation for demonstration
- For production, use a mature ECS library (EnTT, flecs, etc.)
- The generated types work seamlessly with any C++17 codebase
