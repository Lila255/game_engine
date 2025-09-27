#pragma once

#include <array>
#include <shared_mutex>

#include "engine_comp.hpp"
#include "box2d_system.hpp"
#include "chunk.hpp"
#include "legged_creature.hpp"
#include "world_tile_system.hpp"
#include "tile_pathfinding_system.hpp"
namespace game
{

	static siv::PerlinNoise perlin_noise_spider(20.0);
	const float legged_creature_scale = 8.0f;

	struct legged_creature_system : public game_engine::system
	{
	private:
		game::box2d_system * b2d_system;
		game_engine::sparse_component_set<legged_creature> legged_creatures;
		game_engine::render_system * render_sys;
		game_engine::box_system *box_sys;
		game_engine::texture_vbo_system *texture_vbo_sys;
		game::world_tile_system *world_tile_sys;
		game::tile_pathfinding_system *pathfinding_system;

		bool running = 0;
		const uint64_t time_step_ms = 50;


		
		game_engine::task_scheduler* task_scheduler_pointer;

	public:
		// legged_creature_system() : task_scheduler_pointer(new game_engine::task_scheduler()) {}
		~legged_creature_system() { delete task_scheduler_pointer; }
		void update(){};
		void update(uint64_t tick_count);
		void update_rendering(uint64_t tick_count);
		
		legged_creature_system() = delete;
		legged_creature_system(game::box2d_system * b2d_sys
			, game_engine::render_system * render_system
			, game_engine::box_system *box_sys
			, game_engine::texture_vbo_system *texture_vbo_sys
			, game::world_tile_system * world_system
			, game::tile_pathfinding_system *pathfinding_system) : 
			b2d_system(b2d_sys)
			, render_sys(render_system)
			, box_sys(box_sys)
			, texture_vbo_sys(texture_vbo_sys)
			, world_tile_sys(world_system)
			, pathfinding_system(pathfinding_system)
		{
			task_scheduler_pointer = new game_engine::task_scheduler();
		}

		
		void add(entity ent, legged_creature &creature);
		legged_creature& get(entity ent);
		void create_legged_creature(entity ent, float x, float y, legged_creature_type type, entity head_ent, entity tail_ent);

		void start_thread();

		void add_task(game_engine::task t);

		std::vector<entity> get_entities()
		{
			return legged_creatures.get_entities();
		}

		void set_running(bool run) { running = run; }
	};



























}