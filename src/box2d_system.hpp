#pragma once
#include <mutex>

#include "engine_comp.hpp"

namespace game
{
	extern std::mutex b2d_mutex;

	enum b2fixture_types
	{
		EMPTY, // set to this to delete fixture
		PLAYER,
		TERRAIN,
		PROJECTILE,
		DEBRIS
	};

	struct b2_user_data
	{
		entity ent;
		b2fixture_types type;
		std::chrono::time_point<std::chrono::steady_clock>  spawn_time;
		uint16_t lifetime = 500;
		b2_user_data() = delete;
		b2_user_data(entity e, b2fixture_types t) : ent(e), type(t) {
			spawn_time = std::chrono::steady_clock::now();
		}
	};

	struct box2d_system : public game_engine::system
	{
	private:
		b2Vec2 gravity; //(0.0f, -9.8f);
		game_engine::sparse_component_set<b2Body *> static_bodies;
		game_engine::sparse_component_set<b2Body *> dynamic_bodies;

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

		void update() {}

		void update(uint64_t time_to_step);

		b2Body *get_static_body(entity ent);

		b2Body *get_dynamic_body(entity ent);
	};
}