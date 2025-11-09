# Integration Guide

Integrate Carch into your C++ game project.

## CMake Integration

### Basic Setup

Add to your `CMakeLists.txt`:

```cmake
# Find or build Carch compiler
find_program(CARCH_COMPILER carch)

if(NOT CARCH_COMPILER)
    message(FATAL_ERROR "Carch compiler not found")
endif()

# Function to compile .carch files
function(add_carch_schema TARGET_NAME CARCH_FILE)
    get_filename_component(BASENAME ${CARCH_FILE} NAME_WE)
    set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/${BASENAME}.h")
    
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND ${CARCH_COMPILER} ${CMAKE_CURRENT_SOURCE_DIR}/${CARCH_FILE}
                -o ${CMAKE_CURRENT_BINARY_DIR}/generated
        DEPENDS ${CARCH_FILE}
        COMMENT "Compiling Carch schema: ${CARCH_FILE}"
    )
    
    add_custom_target(${TARGET_NAME} DEPENDS ${OUTPUT_FILE})
endfunction()

# Compile your schema
add_carch_schema(components_schema schemas/components.carch)

# Add to your executable
add_executable(game main.cpp)
add_dependencies(game components_schema)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)
```

### Multiple Schemas

```cmake
set(CARCH_SCHEMAS
    schemas/components.carch
    schemas/entities.carch
    schemas/items.carch
)

foreach(SCHEMA ${CARCH_SCHEMAS})
    get_filename_component(NAME ${SCHEMA} NAME_WE)
    add_carch_schema(schema_${NAME} ${SCHEMA})
    add_dependencies(game schema_${NAME})
endforeach()
```

## File Organization

```
project/
├── CMakeLists.txt
├── schemas/
│   ├── components.carch
│   ├── entities.carch
│   └── items.carch
├── src/
│   └── main.cpp
└── build/
    └── generated/
        ├── components.h
        ├── entities.h
        └── items.h
```

## Usage with ECS Libraries

### EnTT

```cpp
#include <entt/entt.hpp>
#include "components.h"

entt::registry registry;

// Create entity
auto entity = registry.create();

// Add components
registry.emplace<game::Position>(entity, 0.0f, 0.0f, 0.0f);
registry.emplace<game::Health>(entity, 100, 100);

// Query components
auto view = registry.view<game::Position, game::Velocity>();
for (auto [entity, pos, vel] : view.each()) {
    pos.x += vel.dx;
    pos.y += vel.dy;
    pos.z += vel.dz;
}
```

### flecs

```cpp
#include <flecs.h>
#include "components.h"

flecs::world world;

// Register components
world.component<game::Position>();
world.component<game::Velocity>();

// Create entity
auto entity = world.entity()
    .set(game::Position{0.0f, 0.0f, 0.0f})
    .set(game::Velocity{1.0f, 0.0f, 0.0f});

// System
world.system<game::Position, game::Velocity>()
    .each([](game::Position& pos, game::Velocity& vel) {
        pos.x += vel.dx;
        pos.y += vel.dy;
        pos.z += vel.dz;
    });
```

## Serialization

### JSON with nlohmann/json

```cpp
#include <nlohmann/json.hpp>
#include "components.h"

// Serialize
void to_json(nlohmann::json& j, const game::Position& p) {
    j = nlohmann::json{{"x", p.x}, {"y", p.y}, {"z", p.z}};
}

// Deserialize
void from_json(const nlohmann::json& j, game::Position& p) {
    j.at("x").get_to(p.x);
    j.at("y").get_to(p.y);
    j.at("z").get_to(p.z);
}
```

### Binary Serialization

```cpp
#include <fstream>
#include "components.h"

// Write
void save(const game::Player& player, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    file.write(reinterpret_cast<const char*>(&player), sizeof(player));
}

// Read
game::Player load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    game::Player player;
    file.read(reinterpret_cast<char*>(&player), sizeof(player));
    return player;
}
```

Note: For types with std::string, std::vector, etc., use proper serialization libraries.

## Versioning and Migration

### Schema Evolution

When updating schemas:

1. Add new optional fields rather than modifying existing ones
2. Use separate schema versions for breaking changes
3. Implement migration code for data conversion

```carch
// v1
Player : struct {
    id: u64,
    name: str
}

// v2 (backward compatible)
Player : struct {
    id: u64,
    name: str,
    level: optional<u32>  // New optional field
}
```

### Migration Example

```cpp
struct PlayerV1 { uint64_t id; std::string name; };
struct PlayerV2 { uint64_t id; std::string name; std::optional<uint32_t> level; };

PlayerV2 migrate(const PlayerV1& old) {
    return {old.id, old.name, std::nullopt};
}
```

## Build System Tips

1. **Clean builds**: Regenerate schemas when .carch files change
2. **Dependencies**: Make executables depend on schema generation
3. **Include paths**: Add generated/ to include directories
4. **CI/CD**: Run `carch` in build pipeline

## Debugging

### View Generated Code

```bash
carch components.carch -o output/
cat output/components.h
```

### Compile Test

```bash
g++ -std=c++17 -fsyntax-only -I output/ test.cpp
```

## See Also

- [Getting Started](getting-started.md) - Basic usage
- [Examples](../../examples/) - Complete examples
