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
		GLuint binding;
		GLuint format = GL_RGBA;
		uint32_t width = 0;
		uint32_t height = 0;
		
		texture() = default;
		texture(GLuint id) : id(id) {}
		texture(GLuint id, GLuint binding, GLuint format, uint32_t width, uint32_t height) : id(id), binding(binding), format(format), width(width), height(height) {}
	};

	struct sprite : public component
	{
		GLuint program = 0;
		std::vector<texture> textures;

		sprite() = default;
		sprite(GLuint program) : program(program) {}
		void add_texture(texture t)
		{
			textures.push_back(t);
		}
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

		void update_box(entity ent, box b)
		{
			m_boxes.update(ent, b);
		}
	};

	struct texture_vbo_system : public system
	{
	private:
		sparse_component_set<GLuint> m_vbos;
		sparse_component_set<GLuint> m_vaos;
		// GLuint m_vao = 0;

	public:
		texture_vbo_system() = default;
		// {
		// Create a VAO
		// glGenVertexArrays(1, &m_vao);
		// glBindVertexArray(m_vao);
		// }

		/// @brief Create a new VBO for the given entity.
		/// @param ent The entity to create a VBO for. Entity must contain a box component to pull values from
		void add(uint32_t ent)
		{
			// Get the box component
			box &b = ((box_system *)game_engine_pointer->get_system(family::type<box_system>()))->get(ent);

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
				b.x, b.y + b.h, b.z};
			// texture data
			float texture_data[] = {
				0.0f, 0.0f,
				1.0f, 0.0f,
				1.0f, 1.0f,
				0.0f, 1.0f};

			GLuint new_vao;
			glGenVertexArrays(1, &new_vao);

			// Bind the VAO
			glBindVertexArray(new_vao);

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
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)sizeof(vertices));

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			// Add the VBO to the sparse component set
			m_vbos.add(ent, vbo);
			m_vaos.add(ent, new_vao);

			// Unbind the VB0
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Unbind the VAO
			glBindVertexArray(0);
		}

		/// @brief Update the VBO for the given entity.
		/// @param ent The entity to update the VBO for. Entity must contain a box component to pull values from
		void update(entity ent)
		{
			// Get the box component
			box &b = ((box_system *)game_engine_pointer->get_system(family::type<box_system>()))->get(ent);
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
				b.x, b.y + b.h, b.z};
			// texture data
			float texture_data[] = {
				0.0f, 0.0f,
				1.0f, 0.0f,
				1.0f, 1.0f,
				0.0f, 1.0f};

			// Bind the VAO
			glBindVertexArray(m_vaos.get(ent));

			// Bind the VBO
			glBindBuffer(GL_ARRAY_BUFFER, m_vbos.get(ent));

			// Upload the vertex data to the GPU
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(texture_data), NULL, GL_DYNAMIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(texture_data), texture_data);

			// Unbind the VB0
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Unbind the VAO
			glBindVertexArray(0);
		}

		/// @brief Get the VAO for this system
		GLuint get_vao(entity ent)
		{
			return m_vaos.get(ent);
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
		sparse_component_set<sprite> m_sprite_textures;
		// sparse_component_set<GLuint> m_sprite_programs;
		// std::vector<sparse_component_set<texture> *> m_texture_groups;
		// std::unordered_map<GLuint, uint16_t> m_program_groups;

		// function pointer for key callback
		void (*key_callback)(std::unordered_set<int> &keys_pressed);
		void (*mouse_callback)(GLFWwindow *window, std::unordered_set<int> &mouse_buttons_pressed);

		GLFWwindow *m_window;
		uint32_t m_frame_count = 0;

	public:
		render_system(GLFWwindow *window) : m_window(window)
		{
			printf("Error_1.5: x%d\n", glGetError());
			// Do some initialization stuff
			// glClearColor(0.8392f, 0.0078f, 0.4392f, 1.0f);
			// glClearColor(0.2902, 0.0118, 0.2118, 1.0f);
			glClearColor(0.7, 0.8, 1.0, 1.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			// glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			// void (engine::*ptr)(GLFWwindow *window, int key, int scancode, int action, int mods) = &(engine::key_callback);
			// glfwSetKeyCallback(window, (game_engine_pointer->*ptr));
			glfwSetKeyCallback(window, game_engine::static_key_callback);
			glfwSetMouseButtonCallback(window, game_engine::static_mouse_button_callback);
		}

		void set_key_callback(void (*key_callback)(std::unordered_set<int> &keys_pressed))
		{
			this->key_callback = key_callback;
		}
		void set_mouse_callback(void (*mouse_callback)(GLFWwindow *window, std::unordered_set<int> &mouse_buttons_pressed))
		{
			this->mouse_callback = mouse_callback;
		}

		void update() override
		{
			// Do rendering stuff
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			update_keys();

			if (mouse_callback && game_engine_pointer->pressed_mouse_buttons.size() > 0)
			{
				mouse_callback(m_window, game_engine_pointer->pressed_mouse_buttons);
				return;
			}

			texture_vbo_system *texture_vbo_system_pointer = ((texture_vbo_system *)game_engine_pointer->get_system(family::type<texture_vbo_system>()));
			box_system *bo_system_pointer = ((box_system *)game_engine_pointer->get_system(family::type<box_system>()));
			box &b = bo_system_pointer->get(game_engine_pointer->player_entitiy);

			// view_matrix[12] = -b.x - 0.5 * glsl_helper::character_width + window_width * (1.0 / (2 * PIXEL_SCALE));
			// view_matrix[13] = -b.y - 0.5 * glsl_helper::character_height + window_height * (1.0 / (2 * PIXEL_SCALE));

			// projection_matrix[0] += 0.001 / window_width;
			// projection_matrix[5] += 0.001 / window_height;

			// draw stuff
			// glBindVertexArray(texture_vbo_system_pointer -> get_vao());
			// for (auto shader_program : m_program_groups)
			// {
			// 	std::vector<uint32_t> *entities = m_texture_groups[shader_program.second]->get_entities();

			std::vector<uint32_t> *entities = m_sprite_textures.get_entities();
			// glUseProgram(m_);


			// glUniform1i(glGetUniformLocation(shader_program.first, "tex"), 0);

			glActiveTexture(GL_TEXTURE0);

			// GLuint projection_location = glGetUniformLocation(shader_program.first, "projection");
			// glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection_matrix);
			// GLuint view_location = glGetUniformLocation(shader_program.first, "view");
			// glUniformMatrix4fv(view_location, 1, GL_FALSE, view_matrix);

			// printf("Rendfering %d entities\n", entities->size());
			for (std::vector<uint32_t>::iterator it = entities->begin(); it != entities->end(); it++)
			{
				// GLuint program = m_sprite_programs.get(*it);
				sprite &t = m_sprite_textures.get(*it);
				GLuint program = t.program;
				glUseProgram(program);
				
				// GLuint texture_loc = glGetUniformLocation(program, "tex");
				// glUniform1i(texture_loc, 0);
				
				GLuint projection_location = glGetUniformLocation(program, "projection");
				glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection_matrix);
				GLuint view_location = glGetUniformLocation(program, "view");
				glUniformMatrix4fv(view_location, 1, GL_FALSE, view_matrix);
				GLuint texture_size = glGetUniformLocation(program, "texture_size");
				glUniform2i(texture_size, t.textures[0].width, t.textures[0].height);


				// texture &t = m_texture_groups[shader_program.second]->get(*it);
				GLuint vbo = texture_vbo_system_pointer->get_vbo(*it);
				glBindVertexArray(texture_vbo_system_pointer->get_vao(*it));

				// Bind the texture
				// glBindTexture(GL_TEXTURE_2D, t.id);
				// glBindTextureUnit(0, t.id);
				for(int i = 0; i < t.textures.size(); i++)
				{
					glActiveTexture(GL_TEXTURE0 + i);
					glBindTexture(GL_TEXTURE_2D, t.textures[i].id);
					glBindImageTexture(t.textures[i].binding, t.textures[i].id, 0, GL_FALSE, 0, GL_READ_ONLY, t.textures[i].format);
				}
				// glBindTexture(GL_TEXTURE_2D, t.id);
				// glBindImageTexture(0, t.id, 0, GL_FALSE, 0, GL_READ_ONLY, t.format);

				// Bind the VBO
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			}
			glUseProgram(0);
			// }
			// glfwSwapBuffers(m_window);
			glfwPollEvents();
			m_frame_count++;
		}

		// void add(uint32_t ent, texture &&t) = delete;

		void add(entity ent, sprite &&t)
		{
			m_sprite_textures.add(ent, t);
		}
		
		void add(entity ent, sprite t)
		{
			m_sprite_textures.add(ent, t);
		}
		

		void remove(uint32_t ent)
		{
			// m_sprite_textures.remove(ent);
			// for (auto &group : m_texture_groups)
			// {
			// 	if (group->contains(ent))
			// 	{
			// 		group->remove(ent);
			// 		return;
			// 	}
			// }
			m_sprite_textures.remove(ent);
			// m_sprite_programs.remove(ent);
		}

		void update_texture(entity ent, uint8_t *data, int width, int height, GLuint program, int texture_index = 0)
		{
			// texture &t = m_texture_groups[m_program_groups[program]] -> get(ent);
			sprite &t = m_sprite_textures.get(ent);
			glBindTexture(GL_TEXTURE_2D, t.textures[texture_index].id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
		}
		
		void update_texture_section(entity ent, uint8_t * data, int x, int y, int width, int height, GLuint program, int texture_index = 0)
		{
			// texture &t = m_texture_groups[m_program_groups[program]] -> get(ent);
			sprite &t = m_sprite_textures.get(ent);
			glBindTexture(GL_TEXTURE_2D, t.textures[texture_index].id);
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RED, GL_UNSIGNED_BYTE, data);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void update_keys()
		{
			if (!game_engine_pointer->game_engine::engine::pressed_keys.size())
				return;

			// if key_callback is defined, use it instead
			if (key_callback)
			{
				key_callback(game_engine_pointer->pressed_keys);
				return;
			}

			box_system *b_system = (game_engine::box_system *)(game_engine_pointer->get_system(family::type<game_engine::box_system>()));
			texture_vbo_system *vbo_system = (game_engine::texture_vbo_system *)(game_engine_pointer->get_system(family::type<game_engine::texture_vbo_system>()));
			box &b = b_system->get(game_engine_pointer->player_entitiy);

			for (auto &key : game_engine_pointer->pressed_keys)
			{
				if (key == GLFW_KEY_W)
				{
					b.y -= 1.0f;
				}
				if (key == GLFW_KEY_A)
				{
					b.x -= 1.0f;
				}
				if (key == GLFW_KEY_S)
				{
					b.y += 1.0f;
				}
				if (key == GLFW_KEY_D)
				{
					b.x += 1.0f;
				}
			}
			vbo_system->update(game_engine_pointer->player_entitiy);
		}
		void draw_line(float x1, float y1, float z1, float x2, float y2, float z2)
		{
			// float vertices[] = {
			// 	x1, y1, z1,
			// 	x2, y2, z2};
			// GLuint vbo;
			// glGenBuffers(1, &vbo);
			// glBindBuffer(GL_ARRAY_BUFFER, vbo);
			// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			// glEnableVertexAttribArray(0);
			// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			// glDrawArrays(GL_LINES, 0, 2);
			// glDisableVertexAttribArray(0);
			// glDeleteBuffers(1, &vbo);

			// do in instant mode
			glBegin(GL_LINES);
			glVertex3f(x1, y1, z1);
			glVertex3f(x2, y2, z2);
			glEnd();
		}
	};

	// static void draw_line(float x1, float y1, float z1, float x2, float y2, float z2)
	// {
	// 	float vertices[] = {
	// 		x1, y1, z1,
	// 		x2, y2, z2};
	// 	GLuint vbo;
	// 	glGenBuffers(1, &vbo);
	// 	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// 	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 	glEnableVertexAttribArray(0);
	// 	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// 	glDrawArrays(GL_LINES, 0, 2);
	// 	glDisableVertexAttribArray(0);
	// 	glDeleteBuffers(1, &vbo);
	// }
	struct vec2 {
		float x;
		float y;
	};
	
}