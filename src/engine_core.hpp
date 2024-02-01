#pragma once

#include <GL/glew.h>
// #include <GL/glew.h>
#include <gl\GL.h>
#include <GLFW/glfw3.h>
#include <windows.h>

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
	const uint16_t window_width = 2400;
	const uint16_t window_height = 1200;
	// float projection_matrix[16]{
	//     //};
	//     8.0f / window_width, 0.0f, 0.0f, 0.0f,
	//     0.0f, 8.0f / window_height, 0.0f, 0.0f,
	//     0.0f, 0.0f, -1.0f, 0.0f,
	//     -8.0f, -8.0f, 0.0f, 1.0f
	//     // 0.001042, 0.000000, 0.000000, 0.000000,
	//     // 0.000000, -0.001852, 0.000000, 0.000000,
	//     // 0.000000, 0.000000, -0.020000, 0.000000,
	//     // -1.000000, 1.000000, -1.000000, 1.000000
	// };
	float projection_matrix[16] = {
		8.0f / window_width, 0.0f, 0.0f, 0.0f,
		0.0f, 8.0f / window_height, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		-8.0f, -8.0f, 0.0f, 1.0f};
	float view_matrix[16]{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f};

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

		entity player_entitiy; // player entity
		std::unordered_set<int> pressed_keys;
		std::unordered_set<int> pressed_mouse_buttons;

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

		void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
		{
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			{
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}

			if (action == GLFW_PRESS)
			{
				pressed_keys.insert(key);
			}
			else if (action == GLFW_RELEASE)
			{
				if (pressed_keys.count(key) > 0)
					pressed_keys.erase(key);
			}
		}

		void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
		{
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			{
				// std::cout << "Left mouse button pressed" << std::endl;
				pressed_mouse_buttons.insert(button);
			}
			else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
			{
				// std::cout << "Left mouse button released" << std::endl;
				if (pressed_mouse_buttons.count(button) > 0)
					pressed_mouse_buttons.erase(button);
			}
		}
	};
	void static static_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		game_engine_pointer->key_callback(window, key, scancode, action, mods);
	}
	void static static_mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
	{
		printf("Mouse button callback called %d \n", button);
		game_engine_pointer->mouse_button_callback(window, button, action, mods);
	}
}
