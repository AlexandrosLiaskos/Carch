// Simple 2D Game Example using Carch-generated components
// Demonstrates how to use generated types in a real game

#include "components.h"
#include <iostream>
#include <vector>
#include <memory>
#include <variant>

// Simple Entity-Component System
using EntityID = uint64_t;

struct Entity {
    EntityID id;
    std::unique_ptr<game::Transform> transform;
    std::unique_ptr<game::Sprite> sprite;
    std::unique_ptr<game::RigidBody> physics;
    std::unique_ptr<game::Health> health;
    std::unique_ptr<game::Player> player;
    std::unique_ptr<game::Enemy> enemy;
};

class GameWorld {
public:
    EntityID create_entity() {
        EntityID id = next_id++;
        entities.push_back(Entity{id});
        return id;
    }
    
    Entity* get_entity(EntityID id) {
        for (auto& e : entities) {
            if (e.id == id) return &e;
        }
        return nullptr;
    }
    
    void update(float dt) {
        // Movement system
        for (auto& entity : entities) {
            if (entity.transform && entity.physics) {
                entity.transform->position.x += entity.physics->velocity.x * dt;
                entity.transform->position.y += entity.physics->velocity.y * dt;
            }
        }
        
        // Player input system
        for (auto& entity : entities) {
            if (entity.player && entity.physics) {
                entity.physics->velocity.x = entity.player->input_state.move_x * 5.0f;
                entity.physics->velocity.y = entity.player->input_state.move_y * 5.0f;
            }
        }
        
        // Enemy AI system
        for (auto& entity : entities) {
            if (entity.enemy && entity.transform) {
                update_enemy_ai(entity, dt);
            }
        }
    }
    
private:
    std::vector<Entity> entities;
    EntityID next_id = 1;
    
    void update_enemy_ai(Entity& entity, float dt) {
        // Handle AI state using std::visit
        std::visit([&](auto& state) {
            using T = std::decay_t<decltype(state)>;
            
            if constexpr (std::is_same_v<T, game::AIState_idle>) {
                // Idle logic
            } else if constexpr (std::is_same_v<T, game::AIState_patrol>) {
                // Patrol logic
                if (!state.waypoints.empty()) {
                    // Move toward current waypoint
                }
            } else if constexpr (std::is_same_v<T, game::AIState_chase>) {
                // Chase player
            } else if constexpr (std::is_same_v<T, game::AIState_attack>) {
                // Attack logic
                state.cooldown = std::max(0.0f, state.cooldown - dt);
            }
        }, entity.enemy->ai_state);
    }
};

// Example: Create game entities
int main() {
    GameWorld world;
    
    // Create player
    EntityID player_id = world.create_entity();
    auto* player_entity = world.get_entity(player_id);
    
    player_entity->transform = std::make_unique<game::Transform>();
    player_entity->transform->position = {100.0f, 100.0f};
    player_entity->transform->rotation = 0.0f;
    player_entity->transform->scale = {1.0f, 1.0f};
    
    player_entity->sprite = std::make_unique<game::Sprite>();
    player_entity->sprite->texture_id = 1;
    player_entity->sprite->layer = 10;
    player_entity->sprite->color = {1.0f, 1.0f, 1.0f, 1.0f};
    
    player_entity->health = std::make_unique<game::Health>();
    player_entity->health->current = 100;
    player_entity->health->max = 100;
    player_entity->health->regeneration = 1.0f;
    
    player_entity->player = std::make_unique<game::Player>();
    player_entity->player->score = 0;
    
    player_entity->physics = std::make_unique<game::RigidBody>();
    player_entity->physics->mass = 1.0f;
    player_entity->physics->friction = 0.1f;
    
    // Create enemy
    EntityID enemy_id = world.create_entity();
    auto* enemy_entity = world.get_entity(enemy_id);
    
    enemy_entity->transform = std::make_unique<game::Transform>();
    enemy_entity->transform->position = {200.0f, 200.0f};
    
    enemy_entity->enemy = std::make_unique<game::Enemy>();
    enemy_entity->enemy->ai_state = game::AIState_idle{};
    enemy_entity->enemy->detection_radius = 50.0f;
    
    enemy_entity->health = std::make_unique<game::Health>();
    enemy_entity->health->current = 50;
    enemy_entity->health->max = 50;
    
    // Game loop (simplified)
    std::cout << "Game initialized!" << std::endl;
    std::cout << "Player at: (" << player_entity->transform->position.x 
              << ", " << player_entity->transform->position.y << ")" << std::endl;
    std::cout << "Enemy at: (" << enemy_entity->transform->position.x 
              << ", " << enemy_entity->transform->position.y << ")" << std::endl;
    
    // Simulate a few frames
    for (int frame = 0; frame < 60; ++frame) {
        float dt = 1.0f / 60.0f;
        
        // Simulate player input
        player_entity->player->input_state.move_x = 1.0f;
        player_entity->player->input_state.move_y = 0.0f;
        
        world.update(dt);
    }
    
    std::cout << "After 1 second:" << std::endl;
    std::cout << "Player at: (" << player_entity->transform->position.x 
              << ", " << player_entity->transform->position.y << ")" << std::endl;
    
    return 0;
}
