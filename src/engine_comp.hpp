#pragma once
#include <cstdint>
#include <stdexcept>

#include "engine_core.hpp"
#include "sparse_component_set.hpp"

// Define engine components/systems here
namespace game_engine
{

    ////////////////
    // Components //
    ////////////////

    /// @brief A component that represents a position in 2D space
    struct point : public component
    {
        float x, y;
    };

    /// @brief A component that represents a 2D box
    struct box : public component
    {
    public:
        float x, y, w, h;
        box() = default;
        box(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
    };
    /// @brief A component that stores a texture id
    struct texture : public component
    {
        GLuint id;
    };

    /// @brief A Box2D component that stores a body
    struct box2d_body : public component
    {
        uint32_t id;
    };

    //////////////
    // Systems  //
    //////////////

    struct box_system : public system
    {
    private:
        sparse_component_set<box> m_boxes;

    public:
        void update() override
        {
            // Do not call update() on this system
            throw std::runtime_error("box_system::update() should not be called");
        }

        void add(uint32_t entity, box &&b)
        {
            m_boxes.add(entity, std::move(b));
        }

        void remove(uint32_t entity)
        {
            m_boxes.remove(entity);
        }

        box &get(uint32_t entity)
        {
            return m_boxes.get(entity);
        }
    };

    struct render_system : public system
    {
    private:
        sparse_component_set<texture> m_sprite_textures;
        texture background_texture;
        texture foreground_texture;
        texture light_texture;

        GLFWwindow *m_window;
        uint32_t m_frame_count = 0;

    public:
        render_system(GLFWwindow *window) : m_window(window)
        {
            // Do some initialization stuff
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            // create textures
            // glGenTextures(1, &background_texture.id);
            // glGenTextures(1, &foreground_texture.id);
            // glGenTextures(1, &light_texture.id);

            // glBindTexture(GL_TEXTURE_2D, background_texture.id);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        void update() override
        {
            // Do rendering stuff
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // draw stuff

            glfwSwapBuffers(m_window);
            glfwPollEvents();
            m_frame_count++;
        }

        void add(uint32_t entity, texture &&t)
        {
            m_sprite_textures.add(entity, std::move(t));
        }

        void remove(uint32_t entity)
        {
            m_sprite_textures.remove(entity);
        }

        void set_background_texture(texture &&t)
        {
            background_texture = std::move(t);
        }
        void set_foreground_texture(texture &&t)
        {
            foreground_texture = std::move(t);
        }
        void set_light_texture(texture &&t)
        {
            light_texture = std::move(t);
        }

    };
}