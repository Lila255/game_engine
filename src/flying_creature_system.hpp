#pragma once

#include <array>
#include <shared_mutex>

#include "engine_comp.hpp"
#include "box2d_system.hpp"
#include "chunk.hpp"
#include "flying_creature.hpp"
#include "tile_pathfinding_system.hpp"

namespace game
{

	static siv::PerlinNoise perlin_noise__bees(10.0);


	struct flying_creature_system : public game_engine::system
	{
	private:
		game_engine::sparse_component_set<flying_creature> flying_creatures;
		game::box2d_system * b2d_system;
		game_engine::render_system * render_sys;
		game_engine::box_system *box_sys;
		game_engine::texture_vbo_system *texture_vbo_sys;
		tile_pathfinding_system* pathfinding_system;
		world_tile_system* world_tiles;
		bool running = 0;
		const uint64_t time_step_ms = 50;
		
		game_engine::task_scheduler* task_scheduler_pointer;

	public:
		// flying_creature_system() : task_scheduler_pointer(new game_engine::task_scheduler()) {}
		~flying_creature_system() { delete task_scheduler_pointer; }
		void update(){};
		void update(uint64_t tick_count);
		void update_rendering(uint64_t tick_count);
		flying_creature_system() = delete;
		flying_creature_system(game::box2d_system * b2d_sys, game_engine::render_system * render_system, game_engine::box_system *box_sys, game_engine::texture_vbo_system *texture_vbo_sys, tile_pathfinding_system* pathfinding_system, world_tile_system* world_tiles) : b2d_system(b2d_sys), render_sys(render_system), box_sys(box_sys), texture_vbo_sys(texture_vbo_sys), pathfinding_system(pathfinding_system), world_tiles(world_tiles)
		{
			task_scheduler_pointer = new game_engine::task_scheduler();
		}

		
		void add(entity ent, flying_creature &creature);
		flying_creature& get(entity ent);
		std::vector<entity> get_entities();
		void create_flying_creature(entity ent, float x, float y, flying_creature_type type);

		void start_thread();

		void add_task(game_engine::task t);
		
		void set_running(bool run) { running = run; }
	};



























}