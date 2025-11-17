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
		uint64_t _counter = 0;
		uint64_t total_time_millis = 0;
	public:
		system() = default;
		system(const system &) = delete;
		system(system &&) = delete;
		system &operator=(const system &) = delete;
		system &operator=(system &&) = delete;
		virtual ~system() = default;

		virtual void update() = 0;
		uint64_t get_counter() const { return _counter; }
		uint64_t get_total_time_millis() const { return total_time_millis; }
		void reset_counter() { _counter = 0; total_time_millis = 0; }
		void increment_counter() { _counter++; }
		void add_time(uint64_t time) { total_time_millis += time; }
	};

	struct engine
	{
	private:
		// std::unordered_set<system *> m_systems;
		std::unordered_map<uint32_t, system *> m_systems;
		std::unordered_set<entity> m_entities;
		std::mutex entity_mutex;

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
			std::lock_guard<std::mutex> lock(entity_mutex);
			m_entities.insert(ent);
			return ent;
		}

		void add_entity(entity ent)
		{
			std::lock_guard<std::mutex> lock(entity_mutex);
			m_entities.insert(ent);
		}

		void remove_entity(entity ent)
		{
			std::lock_guard<std::mutex> lock(entity_mutex);
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
		size_t size()
		{
			std::lock_guard<std::mutex> lock(mutex);
			return queue.size();
		}
	};

	struct task
	{
		bool (* function_pointer)(void *);
		void * task_parameters;
		uint64_t run_time = 0;
		uint16_t retry_count = 10;

		void run()
		{
			function_pointer(task_parameters);
		}
		// bool run(task_scheduler* scheduler)
		// {
		// 	bool result = function_pointer(task_parameters);
			// if(!result && retry_count > 0)
			// {
			// 	retry_count--;
			// 	scheduler->add_backlog_task(*this, 1000);
			// }

		// }
	};

	
	struct task_scheduler
	{
	private:
		thread_safe_queue<task> task_queue;
		std::function<bool(const task&, const task&)> cmp = [](const task& a, const task& b) {
			return a.run_time > b.run_time;
		};
		std::mutex backlog_mutex;
		std::priority_queue<task, std::vector<task>, decltype(cmp)> backlog_queue{cmp};
		
		// max threads
		uint16_t thread_count = 1;
		std::unordered_set<std::thread::id> worker_threads;
		std::mutex worker_threads_mutex;

		bool shutdown_flag = false;
		static uint64_t task_counter;
		static std::unordered_map<uint64_t, uint64_t> task_count_by_type;
		static std::unordered_map<uint64_t, std::string> task_names;

		
		void start_backlog_runner()
		{
			uint64_t check_interval_ms = 100;
			while(!shutdown_flag)
			{
				auto start = std::chrono::steady_clock::now();
				uint64_t current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
				
				backlog_mutex.lock();
				while(!backlog_queue.empty() && backlog_queue.top().run_time <= current_time)
				{
					task t = backlog_queue.top();
					backlog_queue.pop();
					add_task(t);
				}
				backlog_mutex.unlock();
				
				auto end = std::chrono::steady_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
				if(elapsed < check_interval_ms)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(check_interval_ms - elapsed));
				}
			}
		}

		void run_async_task(task t)
		{
			if(!t.function_pointer || !t.task_parameters || shutdown_flag)
			{
				std::lock_guard<std::mutex> lock(worker_threads_mutex);
				worker_threads.erase(std::this_thread::get_id());
				return;
			}

			bool success = t.function_pointer(t.task_parameters);
			if(success)
			{
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
			else
			{
				if(t.retry_count > 0)
				{
					t.retry_count--;
					add_backlog_task(t, 1000);
				}
			}
			std::lock_guard<std::mutex> lock(worker_threads_mutex);
			worker_threads.erase(std::this_thread::get_id());
		}

	public:
		void start()
		{
			task_counter = 0;
			task_count_by_type.clear();
			std::thread backlog_runner(&task_scheduler::start_backlog_runner, this);

			while(!shutdown_flag)
			{
				if(thread_count == 1)
				{
					task t = task_queue.pop();
					bool success = t.function_pointer(t.task_parameters);
					if(success)
					{
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
					else
					{
						if(t.retry_count > 0)
						{
							t.retry_count--;
							add_backlog_task(t, 1000);
						}
					}
				}
				else
				{
					if(worker_threads.size() >= thread_count)
					{
						// sleep for a bit
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
						continue;
					}
					task t = task_queue.pop();
					worker_threads_mutex.lock();
					std::thread worker(&task_scheduler::run_async_task, this, t);
					worker_threads.insert(worker.get_id());
					worker.detach();
					worker_threads_mutex.unlock();
				}
			}
			backlog_runner.join();
		}

		void run_all_tasks()
		{
			while(!task_queue.empty())
			{
				task t = task_queue.pop();
				bool success = t.function_pointer(t.task_parameters);
				if (!success)
				{
					printf("Task failed to run in run_all_tasks()\n");
				}
				else
				{
					task_counter++;
					if (task_count_by_type.count((uint64_t)t.function_pointer) > 0)
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
		}

		void add_task(task t)
		{
			task_queue.push(t);
		}

		uint64_t get_active_task_count()
		{
			return task_queue.size();
		}

		void add_backlog_task(task t, uint64_t delay_ms = 0)
		{
			if (delay_ms > 0)
			{
				t.run_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() + delay_ms;
				std::lock_guard<std::mutex> lock(backlog_mutex);
				backlog_queue.push(t);
			}
			else if(t.run_time != 0)
			{
				std::lock_guard<std::mutex> lock(backlog_mutex);
				backlog_queue.push(t);
			}
			else
			{
				task_queue.push(t);
			}
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
				printf("  Function pointer:  %llu - %s: %d\n", (uint64_t)task.first, task_names.count(task.first) ? task_names[task.first].c_str() : "task with no name", task.second);
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
	
		void set_max_threads(uint16_t count)
		{
			thread_count = count;
		}
	};
}
