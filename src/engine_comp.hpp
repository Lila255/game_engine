#include <cstdint>
#include <stdexcept>

#include "engine_core.hpp"
#include "sparse_component_set.hpp"

// Define engine components/systems here
namespace game_engine {

    ////////////////
    // Components //
    ////////////////

    /// @brief A component that represents a position in 2D space
    struct point : public component {
        float x, y;
    };

    /// @brief A component that represents a 2D box
    struct box : public component {
    public:
        float x, y, w, h;
        box() = default;
        box(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}

    };
    /// @brief A component that stores a texture id
    struct texture : public component {
        uint32_t id;
    };

    /// @brief A Box2D component that stores a body
    struct box2d_body : public component {
        uint32_t id;
    }; 
 



    //////////////
    // Systems  //
    //////////////

    struct box_system : public system {
    private:
        sparse_component_set<box> m_boxes;
    public:

        void update() override {
            // Do not call update() on this system
            throw std::runtime_error("box_system::update() should not be called");
        }

        void add(uint32_t entity, box&& b) {
            m_boxes.add(entity, std::move(b));
        }

        void remove(uint32_t entity) {
            m_boxes.remove(entity);
        }

        box& get(uint32_t entity) {
            return m_boxes.get(entity);
        }
    };
}