#pragma once

#include <array>
#include <shared_mutex>

#include "engine_comp.hpp"
#include "game_core.hpp"
#include "box2d_system.hpp"
#include "chunk.hpp"
#include "flying_creature.hpp"


namespace game
{

	static siv::PerlinNoise perlin_noise_chunk_frame(10.0);

	struct chunk_frame_system : public game_engine::system
	{
	private:
		game_engine::sparse_component_set<flying_creature> flying_creatures;
		game::box2d_system * b2d_system;
		game_engine::render_system * render_sys;
		game_engine::box_system *box_sys;
		game_engine::texture_vbo_system *texture_vbo_sys;
		game::world_tile_system *world_tile_sys;
		bool running = 0;
		const uint64_t time_step_ms = 666;

		uint8_t current_frame = 0;
		bool forwards = 1;
		

	public:
		void update();
		void update(uint64_t tick_count);
		void update_rendering(uint64_t tick_count);
		chunk_frame_system() = delete;
		chunk_frame_system(game::box2d_system * b2d_sys, game_engine::render_system * render_system, game::world_tile_system * world_system) : b2d_system(b2d_sys), render_sys(render_system), world_tile_sys(world_system)
		{
		}

		void start_thread();

	};



























}