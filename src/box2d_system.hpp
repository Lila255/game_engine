#pragma once
#include <mutex>

#include "engine_comp.hpp"

namespace game
{
	extern std::mutex b2d_mutex;
	extern float box2d_scale;

	enum b2fixture_types
	{
		EMPTY = 0, // set to this to delete fixture
		PLAYER = 1,
		TERRAIN = 2,
		PROJECTILE = 4,
		DEBRIS = 8,
		FLYING_CREATURE = 16,
		LEGGED_CREATURE = 32,
		LEGS = 64,
		FEET = 128,
		
	};

	struct b2_user_data
	{
		entity ent;
		entity ent_2;
		entity ent_3;
		entity ent_4;
		entity ent_5;
		uint64_t index = 0;
		b2fixture_types type;
		std::chrono::time_point<std::chrono::steady_clock>  spawn_time;
		uint16_t lifetime = 500;
		b2_user_data() = delete;
		b2_user_data(entity e, b2fixture_types t) : ent(e), type(t) {
			spawn_time = std::chrono::steady_clock::now();
		}
		void set_lifetime(uint16_t millis_lifetime) {
			lifetime = millis_lifetime;
		}
	};

	struct box2d_system : public game_engine::system
	{
	private:
		b2Vec2 gravity; //(0.0f, -9.8f);
		game_engine::sparse_component_set<b2Body *> static_bodies;
		game_engine::sparse_component_set<b2Body *> dynamic_bodies;
		uint8_t running = 0;
		const uint64_t time_step_ms = 10;
	public:
		b2World *world;

		box2d_system();
		~box2d_system();

		/// @brief Create and add a static body to the b2d world
		/// @param ent The entity to associate the body with
		/// @param mesh The triangulated mesh to add to the body
		void create_static_body(entity ent, std::vector<std::pair<float, float>> mesh);

		/// @brief Create a static body with multiple meshes
		/// @param ent The entity to associate the body with
		/// @param meshes The meshes to add to the body
		void create_static_bodies(entity ent, std::vector<std::vector<std::pair<float, float>>> *meshes);

		void update_static_outlines(entity ent, std::vector<std::vector<std::pair<float, float>>> *meshes);

		void remove_static_body(entity ent);

		void create_dynamic_body(entity ent, std::vector<std::pair<float, float>> mesh);

		void remove_dynamic_body(entity ent);

		void start_thread();
		
		void stop_thread();

		void update() {}
		void add_dynamic_body(entity ent, b2Body * body);
		void add_mesh(entity ent,  std::vector<std::pair<float, float>> mesh, b2fixture_types mesh_type);


		void update(uint64_t time_to_step);

		b2Body *get_static_body(entity ent);

		b2Body *get_dynamic_body(entity ent);

		bool contains_static_body(entity ent);
		bool contains_dynamic_body(entity ent);
	};
}