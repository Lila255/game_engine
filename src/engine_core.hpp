#pragma once
#include <cstdint>
#include <unordered_set>

// #include "util.hpp"

namespace game_engine
{

    struct component
    {
        virtual ~component() = default;
    };

    struct entity
    {
        // hash function
        friend std::hash<entity>;
        uint32_t m_id;
        entity() = default;
        entity(uint32_t id) : m_id(id) {}
        entity(const entity & other) = default;
        entity(entity &&) = default;

        // equality operator
        friend bool operator==(const entity &lhs, const entity &rhs)
        {
            return lhs.m_id == rhs.m_id;
        }

        struct hash
        {
            std::size_t operator()(const entity &e) const noexcept
            {
                return std::hash<uint32_t>{}(e.m_id);
            }
        };
    };

    

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
        std::unordered_set<system *> m_systems;
        std::unordered_set<entity, entity::hash> m_entities;

    public:
        engine() = default;
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

        void add_system(system *sys)
        {
            m_systems.insert(sys);
        }

        void remove_system(system *sys)
        {
            // Remove the system from the vector if it contains it
            if (m_systems.count(sys) > 0)
            {
                m_systems.erase(sys);
            }
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
            }
        }

    };
}

