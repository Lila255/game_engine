#pragma once
#include <cstdint>
#include <stdexcept>
#include <box2d/box2d.h>

#include "engine_core.hpp"
#include "sparse_component_set.hpp"

// Define engine components/systems here
namespace game_engine
{

	// std::mutex rendering_mutex;

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

	struct box_lerp : public component
	{
		float x, y, z, w, h;
		float x2, y2, z2, w2, h2;
		uint64_t t0, t1;

		box_lerp() = default;
		box_lerp(float x, float y, float z, float w, float h) : x(x), y(y), w(w), h(h), z(z), x2(x), y2(y), w2(w), h2(h), z2(z), t0(0), t1(0) {}

		box get_box_lerped(uint64_t t)
		{
			uint64_t last_tick_time = t1 - t0;
			if (t < t1)
				return box(x, y, z, w, h);
			if (t > t1 + last_tick_time)
				return box(x2, y2, z2, w2, h2);

			uint64_t time_elapsed = (t - last_tick_time - t0);
			uint64_t total_time = (t1 - t0);
			double t_d = (double)(time_elapsed) / (double)(total_time);

			return box(x + (x2 - x) * t_d, y + (y2 - y) * t_d, z + (z2 - z) * t_d, w + (w2 - w) * t_d, h + (h2 - h) * t_d);
			// return box(x + (x2 - x) * t, y + (y2 - y) * t, z + (z2 - z) * t, w + (w2 - w) * t, h + (h2 - h) * t);
		}

		box get_box()
		{
			return box(x, y, z, w, h);
		}

		box get_box_last()
		{
			return box(x2, y2, z2, w2, h2);
		}
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

	enum sprite_type
	{
		STATIC_IMAGE,
		STATIC_LINE,
	};

	struct sprite : public component
	{
		GLuint program = 0;
		std::vector<texture> textures;
		sprite_type type = STATIC_IMAGE;
		uint8_t is_ui = 0;

		sprite() {}
		sprite(GLuint program, uint8_t is_ui) : program(program), is_ui(is_ui) {}

		void add_texture(texture t)
		{
			textures.push_back(t);
		}

		sprite(const sprite &other) : program(other.program), textures(other.textures), type(other.type), is_ui(other.is_ui) {}
		sprite(sprite &&other) noexcept : program(other.program), textures(std::move(other.textures)), type(other.type), is_ui(other.is_ui) {}
		sprite &operator=(const sprite &other)
		{
			if (this != &other)
			{
				program = other.program;
				textures = other.textures;
				type = other.type;
				is_ui = other.is_ui;
			}
			return *this;
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
		sparse_component_set<box_lerp> m_boxes;

	public:
		void update() override
		{
			// Do not call update() on this system
			throw std::runtime_error("box_system::update() should not be called");
		}

		void add(uint32_t ent, box_lerp &&b)
		{
			b.t0 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();
			b.t1 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();
			m_boxes.add(ent, b);
		}

		void remove(uint32_t ent)
		{
			m_boxes.remove(ent);
		}

		box_lerp &get(uint32_t ent)
		{
			return m_boxes.get(ent);
		}

		void update_box(entity ent, box_lerp b)
		{
			m_boxes.update(ent, b);
		}

		bool contains_ent(entity ent)
		{
			return m_boxes.contains(ent);
		}
	};

	struct texture_vbo_system : public system
	{
	private:
		sparse_component_set<GLuint> m_vbos;
		sparse_component_set<GLuint> m_vaos;
		// GLuint m_vao = 0;

		box_system *box_system_pointer;

	public:
		texture_vbo_system() = delete;
		texture_vbo_system(box_system *box_sys_ptr) : box_system_pointer(box_sys_ptr) {}

		/// @brief Create a new VBO for the given entity.
		/// @param ent The entity to create a VBO for. Entity must contain a box component to pull values from
		void add(uint32_t ent)
		{
			// Get the box component
			// printf("\tBox system pointer: %p\n", box_system_pointer);
			if (box_system_pointer == nullptr || !box_system_pointer->contains_ent(ent))
			{
				throw std::runtime_error("texture_vbo_system::add() - Entity does not contain a box component");
			}
			// 	return;
			// printf("\tGetting box for entity %d \n", ent);

			// printf("\tBox system contains entity: %d\n", box_system_pointer->contains_ent(ent));
			// return;

			box b = box_system_pointer->get(ent).get_box();

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
			box b = ((box_system *)game_engine_pointer->get_system(family::type<box_system>()))->get(ent).get_box();
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

		void update(entity ent, box b, std::vector<float> vertices = {}, std::vector<float> texture_data = {})
		{
			// Create the vertex data
			// float vertex_data[] = {
			//     b.x, b.y, b.z, 0.0f, 0.0f,
			//     b.x + b.w, b.y, b.z, 1.0f, 0.0f,
			//     b.x + b.w, b.y + b.h, b.z, 1.0f, 1.0f,
			//     b.x, b.y + b.h, b.z, 0.0f, 1.0f
			// };
			// verticies data
			std::vector<float> vertices_ = vertices.size() > 0 ? vertices : std::vector<float>{b.x, b.y, b.z, b.x + b.w, b.y, b.z, b.x + b.w, b.y + b.h, b.z, b.x, b.y + b.h, b.z};

			// texture data
			std::vector<float> texture_data_ = texture_data.size() > 0 ? texture_data : std::vector<float>{0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

			// Bind the VAO
			glBindVertexArray(m_vaos.get(ent));

			// Bind the VBO
			glBindBuffer(GL_ARRAY_BUFFER, m_vbos.get(ent));

			// Upload the vertex data to the GPU
			glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(float) + texture_data_.size() * sizeof(float), NULL, GL_DYNAMIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_.size() * sizeof(float), vertices_.data());
			glBufferSubData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(float), texture_data_.size() * sizeof(float), texture_data_.data());

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

		game_engine::task_scheduler *task_scheduler_pointer;

	public:
		render_system(GLFWwindow *window) : m_window(window)
		{
			printf("Error_1.5: x%d\n", glGetError());
			// Do some initialization stuff
			// glClearColor(0.8392f, 0.0078f, 0.4392f, 1.0f);
			glClearColor(0.0, 0.00, 0.0, 1.0f);
			// glClearColor(0.7, 0.8, 1.0, 1.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			// glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			// void (engine::*ptr)(GLFWwindow *window, int key, int scancode, int action, int mods) = &(engine::key_callback);
			// glfwSetKeyCallback(window, (game_engine_pointer->*ptr));
			glfwSetKeyCallback(window, game_engine::static_key_callback);
			glfwSetMouseButtonCallback(window, game_engine::static_mouse_button_callback);

			task_scheduler_pointer = new game_engine::task_scheduler();
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

			task_scheduler_pointer->run_all_tasks();

			if (mouse_callback && game_engine_pointer->pressed_mouse_buttons.size() > 0)
			{
				mouse_callback(m_window, game_engine_pointer->pressed_mouse_buttons);
				// return;
			}
			uint64_t time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
			texture_vbo_system *texture_vbo_system_pointer = ((texture_vbo_system *)game_engine_pointer->get_system(family::type<texture_vbo_system>()));
			box_system *bo_system_pointer = ((box_system *)game_engine_pointer->get_system(family::type<box_system>()));
			// box &b = bo_system_pointer->get(game_engine_pointer->player_entitiy);
			box lerped_player_box = bo_system_pointer->get(game_engine_pointer->player_entitiy).get_box_lerped(time);
			texture_vbo_system_pointer->update(game_engine_pointer->player_entitiy, lerped_player_box);
			// view_matrix[12] = -b.x - 0.5 * glsl_helper::character_width + window_width * (1.0 / (2 * PIXEL_SCALE));
			// view_matrix[13] = -b.y - 0.5 * glsl_helper::character_height + window_height * (1.0 / (2 * PIXEL_SCALE));

			// projection_matrix[0] += 0.001 / window_width;
			// projection_matrix[5] += 0.001 / window_height;

			// draw stuff
			// glBindVertexArray(texture_vbo_system_pointer -> get_vao());
			// for (auto shader_program : m_program_groups)
			// {
			// 	std::vector<uint32_t> *entities = m_texture_groups[shader_program.second]->get_entities();

			std::vector<uint32_t> entities = m_sprite_textures.get_entities();
			// sort entities by box z value
			std::sort(entities.begin(), entities.end(), [&](uint32_t a, uint32_t b)
					  { return bo_system_pointer->get(a).z < bo_system_pointer->get(b).z; });

			// glUniform1i(glGetUniformLocation(shader_program.first, "tex"), 0);

			glActiveTexture(GL_TEXTURE0);

			// GLuint projection_location = glGetUniformLocation(shader_program.first, "projection");
			// glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection_matrix);
			// GLuint view_location = glGetUniformLocation(shader_program.first, "view");
			// glUniformMatrix4fv(view_location, 1, GL_FALSE, view_matrix);

			// printf("Rendfering %d entities\n", entities->size());
			for (std::vector<uint32_t>::iterator it = entities.begin(); it != entities.end(); it++)
			{
				// GLuint program = m_sprite_programs.get(*it);
				sprite &t = m_sprite_textures.get(*it);
				GLuint program = t.program;
				glUseProgram(program);

				// GLuint texture_loc = glGetUniformLocation(program, "tex");
				// glUniform1i(texture_loc, 0);

				if (!t.is_ui)
				{
					GLuint projection_location = glGetUniformLocation(program, "projection");
					glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection_matrix);
					GLuint view_location = glGetUniformLocation(program, "view");
					glUniformMatrix4fv(view_location, 1, GL_FALSE, view_matrix);
				}
				else
				{
					GLuint projection_location = glGetUniformLocation(program, "projection");
					glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection_matrix_ortho);
					GLuint view_location = glGetUniformLocation(program, "view");
					glUniformMatrix4fv(view_location, 1, GL_FALSE, view_matrix_ortho);
				}

				switch (t.type)
				{
				case STATIC_IMAGE:
				{
					GLuint texture_size = glGetUniformLocation(program, "texture_size");
					glUniform2i(texture_size, t.textures[0].width, t.textures[0].height);

					// texture &t = m_texture_groups[shader_program.second]->get(*it);
					GLuint vbo = texture_vbo_system_pointer->get_vbo(*it);
					glBindVertexArray(texture_vbo_system_pointer->get_vao(*it));

					// Bind the texture
					// glBindTexture(GL_TEXTURE_2D, t.id);
					// glBindTextureUnit(0, t.id);
					for (int i = 0; i < t.textures.size(); i++)
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
					break;
				}
				case STATIC_LINE:
				{
					// get box component
					box b = bo_system_pointer->get(*it).get_box();
					// draw line
					draw_line(b.x, b.y, b.z, b.x + b.w, b.y + b.h, b.z);

					break;
				}
				}
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

		void add_task_to_scheduler(task t)
		{
			task_scheduler_pointer->add_task(t);
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
			// rendering_mutex.lock();
			sprite &t = m_sprite_textures.get(ent);
			glBindTexture(GL_TEXTURE_2D, t.textures[texture_index].id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
			// glBindTexture(GL_TEXTURE_2D, 0);
			// rendering_mutex.unlock();
		}

		void update_texture_section(entity ent, uint8_t *data, int x, int y, int width, int height, int texture_index = 0)
		{
			// texture &t = m_texture_groups[m_program_groups[program]] -> get(ent);
			// rendering_mutex.lock();
			sprite &t = m_sprite_textures.get(ent);
			glBindTexture(GL_TEXTURE_2D, t.textures[texture_index].id);
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RED, GL_UNSIGNED_BYTE, data);
			glBindTexture(GL_TEXTURE_2D, 0);
			// rendering_mutex.unlock();
		}
		void update_texture_section_16(entity ent, int16_t *data, int x, int y, int width, int height, int texture_index = 0)
		{
			sprite &t = m_sprite_textures.get(ent);
			glBindTexture(GL_TEXTURE_2D, t.textures[texture_index].id);
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RED, GL_SHORT, (uint8_t *)data);
			glBindTexture(GL_TEXTURE_2D, 0);
			// rendering_mutex.unlock();
		}
		void update_texture_section_float(entity ent, float *data, int x, int y, int width, int height, int texture_index = 0)
		{
			sprite &t = m_sprite_textures.get(ent);
			glBindTexture(GL_TEXTURE_2D, t.textures[texture_index].id);
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RED, GL_FLOAT, data);
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
			box_lerp &b = b_system->get(game_engine_pointer->player_entitiy);

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
			vbo_system->update(game_engine_pointer->player_entitiy, b.get_box());
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
	struct vec2
	{
		float x;
		float y;
	};

}