#pragma once

#include <GL/glew.h>
// #include <GL/glew.h>
#include <gl\GL.h>
#include <GLFW/glfw3.h>
#include <windows.h>

#include <cstdint>
#include <unordered_set>
#include <unordered_map>
#include <mutex>

#include "util.hpp"
#include "opengl_util.hpp"
#include "glsl_helper.hpp"

#define M_PI 3.14159265358979323846		/* pi */
#define radians(x) ((x) * M_PI / 180.0) // degrees to radians


namespace game_engine
{
	struct engine;
	struct task_scheduler;
	extern engine *game_engine_pointer;
	extern task_scheduler *task_scheduler_pointer;
	extern std::vector<GLuint> shader_programs;
	extern const uint16_t window_width;
	extern const uint16_t window_height;

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
	extern float projection_matrix[16];
	
	extern float view_matrix[16];

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
			if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_PRESS)
			{
				// std::cout << "Left mouse button pressed" << std::endl;
				pressed_mouse_buttons.insert(button);
			}
			else if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_RELEASE)
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
		game_engine_pointer->mouse_button_callback(window, button, action, mods);
	}

	template<typename T>
	struct thread_safe_queue
	{
	private:
		std::queue<T> queue;
		std::mutex mutex;
		std::condition_variable cond;

	public:
		void push(T t)
		{
			std::lock_guard<std::mutex> lock(mutex);
			queue.push(t);
			cond.notify_one();
		}
		T pop()
		{
			std::unique_lock<std::mutex> lock(mutex);
			cond.wait(lock, [this] {return !queue.empty();});
			T value = queue.front();
			queue.pop();
			return value;
		}
		bool empty()
		{
			std::lock_guard<std::mutex> lock(mutex);
			return queue.empty();
		}
	};

	struct task
	{
		void (* function_pointer)(void *);
		void * task_parameters;
		void run()
		{
			function_pointer(task_parameters);
		}
	};

	
	struct task_scheduler
	{
	private:
		thread_safe_queue<task> task_queue;
		bool shutdown_flag = false;
		static uint64_t task_counter;
		static std::unordered_map<uint64_t, uint64_t> task_count_by_type;
		static std::unordered_map<uint64_t, std::string> task_names;
	public:
		void start()
		{
			task_counter = 0;
			task_count_by_type.clear();
			while(!shutdown_flag)
			{
				task t = task_queue.pop();
				t.function_pointer(t.task_parameters);
				task_counter++;
				if(task_count_by_type.count((uint64_t)t.function_pointer) > 0)
				{
					task_count_by_type[(uint64_t)t.function_pointer]++;
				}
				else
				{
					task_count_by_type[(uint64_t)t.function_pointer] = 1;
				}
				delete t.task_parameters;
			}
		}

		void add_task(task t)
		{
			task_queue.push(t);
		}

		bool pop_task(task &t)
		{
			if(task_queue.empty())
			{
				return false;
			}
			t = task_queue.pop();
			return true;
		}

		void shutdown(task t)
		{
			shutdown_flag = true;
			add_task(t);
		}

		void add_task_name(uint64_t id, std::string name)
		{
			task_names[id] = name;
		}

		static void print_task_counter(void * null_param)
		{
			printf("Task counter: %d\n", task_counter);
			printf("Task type breakdown:\n");

			for(auto &task: task_count_by_type)
			{
				printf("  Function pointer: %d - %s: %d\n", task.first, task_names.count(task.first) ? task_names[task.first].c_str() : "task with no name", task.second);
			}
			
			// if(task_count_by_type.count((uint64_t)print_task_counter) > 0)
			// {
			// 	printf("print_task_counter: %d\n", task_count_by_type[(uint64_t)print_task_counter]);
			// }
			// if(task_count_by_type.count((uint64_t)ingle_debris_task) > 0)
			// {
			// 	printf("create_single_debris_task: %d\n", task_count_by_type[(uint64_t)create_single_debris_task]);
			// }
			// else
			// {
			// 	// printf("print_task_counter: 0\n");
			// }
		}
	};
}
