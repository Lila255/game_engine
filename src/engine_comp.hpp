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
        float x, y, z, w, h;
        box() = default;
        box(float x, float y, float w, float h) : x(x), y(y), w(w), h(h), z(0) {}
        box(float x, float y, float z, float w, float h) : x(x), y(y), w(w), h(h), z(z) {}
    };

    /// @brief A component that stores a texture id
    struct texture : public component
    {
        GLuint id;
        texture() = default;
        texture(GLuint id) : id(id) {}
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

        void add(uint32_t ent, box &&b)
        {
            m_boxes.add(ent, b);
        }

        void remove(uint32_t ent)
        {
            m_boxes.remove(ent);
        }

        box &get(uint32_t ent)
        {
            return m_boxes.get(ent);
        }
    };

    struct texture_vbo_system : public system
    {
    private:
        sparse_component_set<GLuint> m_vbos;
        GLuint m_vao = 0;

    public:
        texture_vbo_system()
        {
            // Create a VAO
            glGenVertexArrays(1, &m_vao);
            glBindVertexArray(m_vao);
        }

        /// @brief Create a new VBO for the given entity.
        /// @param ent The entity to create a VBO for. Entity must contain a box component to pull values from
        void add(uint32_t ent)
        {
            // Get the box component
            box &b = ((box_system*)game_engine_pointer->get_system(family::type<box_system>())) -> get(ent);

            // Create the vertex data
            // float vertex_data[] = {
            //     b.x, b.y, b.z, 0.0f, 0.0f,
            //     b.x + b.w, b.y, b.z, 1.0f, 0.0f,
            //     b.x + b.w, b.y + b.h, b.z, 1.0f, 1.0f,
            //     b.x, b.y + b.h, b.z, 0.0f, 1.0f
            // };
            // verticies data   
            float vertices[] = {
                b.x, b.y, b.z,
                b.x + b.w, b.y, b.z,
                b.x + b.w, b.y + b.h, b.z,
                b.x, b.y + b.h, b.z
            };
            // texture data
            float texture_data[] = {
                0.0f, 0.0f,
                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 1.0f
            };
            
            // Bind the VAO
            glBindVertexArray(m_vao);
            
            // Create a new VBO
            GLuint vbo;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            // Upload the vertex data to the GPU
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(texture_data), NULL, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(texture_data), texture_data);

            // Enable the vertex attributes
            // glEnableVertexAttribArray(0);
            // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            // glEnableVertexAttribArray(1);
            // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(vertices));

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            // Add the VBO to the sparse component set
            m_vbos.add(ent, vbo);

            // Unbind the VB0
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Unbind the VAO
            glBindVertexArray(0);
        }

        /// @brief Get the VAO for this system
        GLuint get_vao()
        {
            return m_vao;
        }

        /// @brief Get the VBO for the given entity
        GLuint get_vbo(uint32_t ent)
        {
            return m_vbos.get(ent);
        }

        void update() override
        {
            // Do not call update() on this system
            throw std::runtime_error("texture_vbo_system::update() should not be called");
        }
    };

    struct render_system : public system
    {
    private:
        sparse_component_set<texture> m_sprite_textures;
        // texture background_texture;
        // texture foreground_texture;
        // texture light_texture;

        GLFWwindow *m_window;
        uint32_t m_frame_count = 0;

    public:
        render_system(GLFWwindow *window) : m_window(window)
        {
            printf("Error_1.5: x%d\n", glGetError());
            // Do some initialization stuff
            glClearColor(0.25f, 0.0f, 0.2f, 1.0f);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            printf("Error_1.5: x%d\n", glGetError());

            // GLint currnet_matrix_mode;
            // glGetIntegerv(GL_MATRIX_MODE, &currnet_matrix_mode);
            // if(currnet_matrix_mode == GL_PROJECTION) {
            //     printf("Error: already matrix\n");
            // }

            // glMatrixMode(GL_PROJECTION);
            // printf("Error_1.5: x%d\n", glGetError());
            // glLoadIdentity();
            // printf("Error_1.5: x%d\n", glGetError());
            // glOrtho(0.0f, 1.0f * window_width, 1.0f * window_height, 0.0f, 0.0f, 1.0f);
            // printf("Error_1.5: x%d\n", glGetError());
            // glMatrixMode(GL_MODELVIEW);
            // printf("Error_1.5: x%d\n", glGetError());

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

            texture_vbo_system *texture_vbo_system_pointer = ((texture_vbo_system*)game_engine_pointer->get_system(family::type<texture_vbo_system>()));
            // draw stuff
            glBindVertexArray(texture_vbo_system_pointer -> get_vao());
            std::vector<uint32_t> * entities = m_sprite_textures.get_entities();
            
            glUseProgram(shader_programs[0] );
            
            glActiveTexture(GL_TEXTURE0);

            GLuint projection_location = glGetUniformLocation(shader_programs[0], "projection");
            glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection_matrix);

            printf("Rendfering %d entities\n", entities->size());
            for (std::vector<uint32_t>::iterator it = entities->begin(); it != entities->end(); it++)
            {   
                glUseProgram(shader_programs[0] );
                texture &t = m_sprite_textures.get(*it);
                GLuint vbo = texture_vbo_system_pointer -> get_vbo(*it);

                printf("Rendering entity %d\n", *it);
                printf("    texture id: %d\n", t.id);
                printf("    vbo id: %d\n", vbo);

                // Bind the texture
                glBindTexture(GL_TEXTURE_2D, t.id);
                // Bind the VBO
                glBindBuffer(GL_ARRAY_BUFFER, vbo);

                // GLint active_texture;
                // glGetIntegerv(GL_ACTIVE_TEXTURE, &active_texture);
                // printf("Error_1.5: x%d\n", glGetError());
                // printf("active_texture: %d\n", active_texture);
                // printf("real texture: %d\n", t.id);
                // printf("GL_TEXTURE0: %d\n", GL_TEXTURE0);


                // GLint vao2;
                // glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao2);
                // printf("vao2: %d\n", vao2);
                // printf("vao: %d\n", texture_vbo_system_pointer -> get_vao());

                // // GL_ARRAY_BUFFER_BINDING 
                // GLint vbo2;
                // glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo2);
                // printf("Error_1.5: x%d\n", glGetError());
                // printf("vbo2: %d\n", vbo2);
                // printf("vbo: %d\n", vbo);
                // set the texture uniform
                glUniform1i(glGetUniformLocation(shader_programs[0], "tex"), 0);
                // Draw the VBO
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            }
            glUseProgram(0);
            // {
            //     texture &t = pair.second;
            //     GLuint vbo = ((texture_vbo_system*)game_engine_pointer->get_system(family::type<texture_vbo_system>())) -> get_vbo(pair.first);

            //     // Bind the texture
            //     glBindTexture(GL_TEXTURE_2D, t.id);

            //     // Bind the VBO
            //     glBindBuffer(GL_ARRAY_BUFFER, vbo);

            //     // Draw the VBO
            //     glDrawArrays(GL_QUADS, 0, 4);
            // }

            glfwSwapBuffers(m_window);
            glfwPollEvents();
            m_frame_count++;
        }

        void add(uint32_t ent, texture &&t)
        {
            m_sprite_textures.add(ent, t);
        }

        void remove(uint32_t ent)
        {
            m_sprite_textures.remove(ent);
        }
    };
}