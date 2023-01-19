#include <cstdint>


namespace game_engine {
    struct component {
        virtual ~component() = default;
    };
    
    struct entity {
        uint32_t m_id;
        entity() = default;
        entity(uint32_t id) : m_id(id) {}
        // std::vector<component*> components;
    };

    struct system {
    private:

    public:
        system() = default;
        system(const system&) = delete;
        system(system&&) = delete;
        system& operator=(const system&) = delete;
        system& operator=(system&&) = delete;
        virtual ~system() = default;
        
        
        virtual void update() = 0;
    };

}