#pragma once

#include <GL/glew.h>
#include <gl\GL.h>
#include <GLFW/glfw3.h>

#include <cstdint>
#include <unordered_set>
#include <unordered_map>

#include "util.hpp"
#include "opengl_util.hpp"
#include "glsl_shaders.hpp"

namespace game_engine
{
    struct engine;
    engine *game_engine_pointer;
    std::vector<GLuint> shader_programs;
    uint16_t window_width = 1920;
    uint16_t window_height = 1080;
    float projection_matrix[16]{};
    
    struct component
    {
        virtual ~component() = default;
    };
// struct entity
// {
//     // hash function
//     friend std::hash<entity>;
//     uint32_t m_id;
//     entity() = default;
//     entity(uint32_t id) : m_id(id) {}
//     entity(const entity & other) = default;
//     entity(entity &&) = default;

//     // equality operator
//     friend bool operator==(const entity &lhs, const entity &rhs)
//     {
//         return lhs.m_id == rhs.m_id;
//     }

//     struct hash
//     {
//         std::size_t operator()(const entity &e) const noexcept
//         {
//             return std::hash<uint32_t>{}(e.m_id);
//         }
//     };
// };
// What dumbass wrote this code ^^^
#define entity uint32_t

    struct system
    {
    private:
    public:
        system() = default;
        system(const system &) = delete;
        system(system &&) = delete;
        system &operator=(const system &) = delete;
        system &operator=(system &&) = delete;
        virtual ~system() = default;

        virtual void update() = 0;
    };

    struct engine
    {
    private:
        // std::unordered_set<system *> m_systems;
        std::unordered_map<uint32_t, system *> m_systems;
        std::unordered_set<entity> m_entities;

    public:
        engine()
        {
            game_engine_pointer = this;
        };
        engine(const engine &) = delete;
        engine(engine &&) = delete;
        engine &operator=(const engine &) = delete;
        engine &operator=(engine &&) = delete;
        ~engine() = default;

        void update_physics()
        {
            // Update all physics systems
        }

        void update_graphics()
        {
            // Update all graphics systems
        }

        void add_system(uint32_t type, system *sys)
        {
            m_systems.insert(std::make_pair(type, sys));
            // m_systems.insert(sys);
        }

        system *get_system(uint32_t type)
        {
            // Get the system from the vector if it contains it
            if (m_systems.count(type) > 0)
            {
                return m_systems[type];
            }

            return nullptr;
        }

        void remove_system(uint32_t type)
        {
            // Remove the system from the vector if it contains it
            if (m_systems.count(type) > 0)
            {
                m_systems.erase(type);
            }
        }

        entity create_entity()
        {
            entity ent = entity(id_generator::generate());
            m_entities.insert(ent);
            return ent;
        }

        void add_entity(entity ent)
        {
            m_entities.insert(ent);
        }

        void remove_entity(entity ent)
        {
            // Remove the entity from the vector if it contains it
            if (m_entities.count(ent) > 0)
            {
                m_entities.erase(ent);
                game_engine::id_generator::free_id(ent);
            }
        }
    };
}
