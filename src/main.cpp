#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <thread>
#include <mutex>

#include "engine_comp.hpp"
#include "game_core.hpp"

static std::vector<std::vector<std::vector<std::pair<float, float>>>> chunk_outlines;

game::tile_type selected_tile_type = game::tile_type::AIR;
entity selected_hotbar_entity;

void custom_key_callback(std::unordered_set<int> &keys)
{
	if (keys.find(GLFW_KEY_ESCAPE) != keys.end())
	{

		glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
	}
	// w
	if (keys.count(GLFW_KEY_SPACE) > 0)
	{
		game::box2d_system *b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::box2d_system>()));
		entity player = game_engine::game_engine_pointer->player_entitiy;
		game::b2d_mutex.lock();
		b2Body *body = b2d_sys->get_dynamic_body(player);
		// if body is touching something
		if (1 == 1 || body->GetContactList() != NULL)
		{
			// for (b2ContactEdge *ce = body->GetContactList(); ce != NULL; ce = ce->next)
			// {
			// 	if (!ce->contact->IsTouching())
			// 		continue;
			// 	printf("normal: %f, %f\n", ce->contact->GetManifold()->localNormal.x, ce->contact->GetManifold()->localNormal.y);
			// 	// if the normal is pointing up
			// 	if (ce->contact->GetManifold()->localNormal.y <= 0)
			// 	{
			// 		b2Vec2 impulse = b2Vec2(0.0f, -1000.1f);
			// body->ApplyLinearImpulseToCenter(impulse, true);
			// apply force
			// keys.erase(GLFW_KEY_W);
			// body->ApplyForceToCenter(impulse, true);

			// set vertical velocity to negative value.
			body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, -4.0f));
			// 		break;
			// 	}
			// }
			// b2Vec2 impulse = b2Vec2(0.0f, -1000.1f);
			// body->ApplyLinearImpulseToCenter(impulse, true);
		}
		game::b2d_mutex.unlock();
		// b2Vec2 impulse = b2Vec2(0.0f, -100.1f);
		// body->ApplyLinearImpulseToCenter(impulse, true);
	}
	// a
	if (keys.count(GLFW_KEY_A) > 0)
	{
		game::box2d_system *b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::box2d_system>()));
		entity player = game_engine::game_engine_pointer->player_entitiy;
		b2Body *body = b2d_sys->get_dynamic_body(player);
		b2Vec2 impulse = b2Vec2(body->GetContactList() != NULL ? -.121f : -.03f, 0.0f);
		body->ApplyLinearImpulseToCenter(impulse, true);
	}


	// s
	if (keys.count(GLFW_KEY_S) > 0)
	{
		// if in water or something, idk

		// add angular velocity
		game::box2d_system *b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::box2d_system>()));
		entity player = game_engine::game_engine_pointer->player_entitiy;
		b2Body *body = b2d_sys->get_dynamic_body(player);
		b2Vec2 impulse = b2Vec2(0.0f, 0.81f);
		body->ApplyLinearImpulseToCenter(impulse, true);
	}

	// d
	if (keys.count(GLFW_KEY_D) > 0)
	{
		game::box2d_system *b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::box2d_system>()));
		entity player = game_engine::game_engine_pointer->player_entitiy;
		b2Body *body = b2d_sys->get_dynamic_body(player);
		b2Vec2 impulse = b2Vec2(body->GetContactList() != NULL ? .121f : .03f, 0.0f);
		body->ApplyLinearImpulseToCenter(impulse, true);
	}

	// shift
	if (keys.count(GLFW_KEY_LEFT_SHIFT) > 0)
	{
		game::box2d_system *b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::box2d_system>()));
		entity player = game_engine::game_engine_pointer->player_entitiy;
		b2Body *body = b2d_sys->get_dynamic_body(player);
		int direction = 0;
		if (keys.count(GLFW_KEY_A) > 0 && keys.count(GLFW_KEY_D) > 0)
		{
			direction = 0;
		}
		else if (keys.count(GLFW_KEY_A))
		{
			direction = -1;
		}
		else if (keys.count(GLFW_KEY_D))
		{
			direction = 1;
		}
		else
		{
			direction = body->GetLinearVelocity().x > 0 ? 1 : -1;
		}
		// b2Vec2 impulse = b2Vec2(direction * 3.f, 0.f);
		// body->ApplyLinearImpulseToCenter(impulse, true);
		// set horizontal velocity to direction * 10
		body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x + direction * 8.0f, body->GetLinearVelocity().y));
		keys.erase(GLFW_KEY_LEFT_SHIFT);
	}

	game_engine::box_system *box_sys = (game_engine::box_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::box_system>()));
	game_engine::texture_vbo_system *texture_vbo_sys = (game_engine::texture_vbo_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::texture_vbo_system>()));
	
	if (keys.count(GLFW_KEY_1) > 0)
	{
		selected_tile_type = game::tile_type::IRON;
		keys.erase(GLFW_KEY_1);

		game_engine::box_lerp &selected_hotbar_box = box_sys->get(selected_hotbar_entity);
		selected_hotbar_box.x = (game_engine::window_width / PIXEL_SCALE - (game_engine::window_width / PIXEL_SCALE) / 3.5f) / 2.f + 0 * ((game_engine::window_width / PIXEL_SCALE) / 3.5f) / 9.f;

		texture_vbo_sys->update(selected_hotbar_entity);
	}

	if (keys.count(GLFW_KEY_2) > 0)
	{
		selected_tile_type = game::tile_type::WATER;
		keys.erase(GLFW_KEY_2);

		game_engine::box_lerp &selected_hotbar_box = box_sys->get(selected_hotbar_entity);
		selected_hotbar_box.x = (game_engine::window_width / PIXEL_SCALE - (game_engine::window_width / PIXEL_SCALE) / 3.5f) / 2.f + 1 * ((game_engine::window_width / PIXEL_SCALE) / 3.5f) / 9.f;
		texture_vbo_sys->update(selected_hotbar_entity);
	}

	if (keys.count(GLFW_KEY_3) > 0)
	{
		selected_tile_type = game::tile_type::WOOD;
		keys.erase(GLFW_KEY_3);
		game_engine::box_lerp &selected_hotbar_box = box_sys->get(selected_hotbar_entity);
		selected_hotbar_box.x = (game_engine::window_width / PIXEL_SCALE - (game_engine::window_width / PIXEL_SCALE) / 3.5f) / 2.f + 2 * ((game_engine::window_width / PIXEL_SCALE) / 3.5f) / 9.f;
		texture_vbo_sys->update(selected_hotbar_entity);
	}

	if (keys.count(GLFW_KEY_4) > 0)
	{
		selected_tile_type = game::tile_type::EMBER;
		keys.erase(GLFW_KEY_4);
		game_engine::box_lerp &selected_hotbar_box = box_sys->get(selected_hotbar_entity);
		selected_hotbar_box.x = (game_engine::window_width / PIXEL_SCALE - (game_engine::window_width / PIXEL_SCALE) / 3.5f) / 2.f + 3 * ((game_engine::window_width / PIXEL_SCALE) / 3.5f) / 9.f;
		texture_vbo_sys->update(selected_hotbar_entity);
	}

	if (keys.count(GLFW_KEY_5) > 0)
	{
		selected_tile_type = game::tile_type::BEE_BLACK;
		keys.erase(GLFW_KEY_5);
		game_engine::box_lerp &selected_hotbar_box = box_sys->get(selected_hotbar_entity);
		selected_hotbar_box.x = (game_engine::window_width / PIXEL_SCALE - (game_engine::window_width / PIXEL_SCALE) / 3.5f) / 2.f + 4 * ((game_engine::window_width / PIXEL_SCALE) / 3.5f) / 9.f;
		texture_vbo_sys->update(selected_hotbar_entity);
	}
	
	if (keys.count(GLFW_KEY_6) > 0)
	{
		selected_tile_type = game::tile_type::INSULATION_FOAM;
		keys.erase(GLFW_KEY_6);
		game_engine::box_lerp &selected_hotbar_box = box_sys->get(selected_hotbar_entity);
		selected_hotbar_box.x = (game_engine::window_width / PIXEL_SCALE - (game_engine::window_width / PIXEL_SCALE) / 3.5f) / 2.f + 5 * ((game_engine::window_width / PIXEL_SCALE) / 3.5f) / 9.f;
		texture_vbo_sys->update(selected_hotbar_entity);
	}

	if (keys.count(GLFW_KEY_7) > 0)
	{
		selected_tile_type = game::tile_type::AIR;
		keys.erase(GLFW_KEY_7);
		game_engine::box_lerp &selected_hotbar_box = box_sys->get(selected_hotbar_entity);
		selected_hotbar_box.x = (game_engine::window_width / PIXEL_SCALE - (game_engine::window_width / PIXEL_SCALE) / 3.5f) / 2.f + 6 * ((game_engine::window_width / PIXEL_SCALE) / 3.5f) / 9.f;
		texture_vbo_sys->update(selected_hotbar_entity);
	}

	if (keys.count(GLFW_KEY_8) > 0)
	{
		selected_tile_type = game::tile_type::EMBER;
		keys.erase(GLFW_KEY_8);
		game_engine::box_lerp &selected_hotbar_box = box_sys->get(selected_hotbar_entity);
		selected_hotbar_box.x = (game_engine::window_width / PIXEL_SCALE - (game_engine::window_width / PIXEL_SCALE) / 3.5f) / 2.f + 7 * ((game_engine::window_width / PIXEL_SCALE) / 3.5f) / 9.f;
		texture_vbo_sys->update(selected_hotbar_entity);
	}

	if (keys.count(GLFW_KEY_9) > 0)
	{
		selected_tile_type = game::tile_type::LAVA;
		keys.erase(GLFW_KEY_9);
		game_engine::box_lerp &selected_hotbar_box = box_sys->get(selected_hotbar_entity);
		selected_hotbar_box.x = (game_engine::window_width / PIXEL_SCALE - (game_engine::window_width / PIXEL_SCALE) / 3.5f) / 2.f + 8 * ((game_engine::window_width / PIXEL_SCALE) / 3.5f) / 9.f;
		texture_vbo_sys->update(selected_hotbar_entity);
	}

}

static uint64_t mouse_click_counter = 0;
static std::pair<double, double> last_mouse_click = {-1, -1};

void custom_mouse_callback(GLFWwindow *window, std::unordered_set<int> &buttons)
{
	// get mouse position
	double xpos, ypos = -1;
	glfwGetCursorPos(window, &xpos, &ypos);

	// if mouse click
	if (buttons.count(GLFW_MOUSE_BUTTON_LEFT) > 0)
	{
		// // get character position
		game::box2d_system *b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::box2d_system>()));
		game_engine::box_system *box_sys = (game_engine::box_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::box_system>()));

		entity player = game_engine::game_engine_pointer->player_entitiy;

		b2Body *body = b2d_sys->get_dynamic_body(player);
		b2Vec2 player_pos = body->GetPosition();
		game_engine::box_lerp &player_box = box_sys->get(player);

		// // get mouse position
		int world_x = (int)((1.0 * xpos / PIXEL_SCALE) - 0.5 * (1.0 * game_engine::window_width / PIXEL_SCALE) + player_pos.x * game::box2d_scale + player_box.w / 2.f);
		int world_y = (int)((1.0 * ypos / PIXEL_SCALE) - 0.5 * (1.0 * game_engine::window_height / PIXEL_SCALE) + player_pos.y * game::box2d_scale + player_box.h / 2.f);
		// printf("Delete circle at x: %d, y: %d\n", world_x, world_y);
		// // get world tile system
		game::world_tile_system *world_sys = (game::world_tile_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::world_tile_system>()));
		// world_sys->delete_circle(world_x, world_y, 18, {});
		world_sys->task_scheduler_pointer->add_task({&game::delete_circle_task,
			new game::delete_circle_params{
				world_x,
				world_y,
				18,
			}});
		// world_sys->explode_circle(world_x, world_y, 8, 2000, {});

		buttons.erase(GLFW_MOUSE_BUTTON_LEFT);
	}
	else if (buttons.count(GLFW_MOUSE_BUTTON_RIGHT) > 0)
	{

		// get angle from center of screen to mouse
		double angle = atan2(game_engine::window_height / 2.0 - ypos, xpos - game_engine::window_width / 2.0);
		// printf("Angle: %f\n", angle);

		// lock b2d mutex
		// game::b2d_mutex.lock();
		// get character position
		game::box2d_system *b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::box2d_system>()));
		entity player = game_engine::game_engine_pointer->player_entitiy;
		b2Body *player_body = b2d_sys->get_dynamic_body(player);
		b2Vec2 player_pos = player_body->GetPosition();

		if (selected_tile_type == game::tile_type::BEE_BLACK)
		{
			// create projectile
			game::projectile_system *projectile_sys = (game::projectile_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::projectile_system>()));
			// projectile_sys->create_projectile(player_pos.x, player_pos.y, angle);

			// create entity for projectile
			int x = (int)((1.0 * xpos / PIXEL_SCALE) - 0.5 * (1.0 * game_engine::window_width / PIXEL_SCALE) + player_pos.x * game::box2d_scale);
			int y = (int)((1.0 * ypos / PIXEL_SCALE) - 0.5 * (1.0 * game_engine::window_height / PIXEL_SCALE) + player_pos.y * game::box2d_scale);

			entity projectile_entity = game_engine::game_engine_pointer->create_entity();
			projectile_sys->create_projectile(projectile_entity, (float)(player_pos.x * game::box2d_scale + cos(angle) * 5.0f), (float)(player_pos.y * game::box2d_scale + sin(angle) * 5.0f), float(angle), 15.f, glsl_helper::projectile_width / 2.0f, 500, game::b2fixture_types::PROJECTILE);

			game_engine::render_system *render_sys = (game_engine::render_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::render_system>()));
			game_engine::texture_vbo_system *texture_vbo_sys = (game_engine::texture_vbo_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::texture_vbo_system>()));
			game_engine::box_system *box_sys = (game_engine::box_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::box_system>()));
			game_engine::sprite sprt(game_engine::shader_programs[0], 0);
			GLuint projectile_texture;
			glsl_helper::create_projectile_texture(projectile_texture);

			sprt.add_texture({projectile_texture, 0, GL_R8, glsl_helper::projectile_width, glsl_helper::projectile_height});
			render_sys->add(projectile_entity, sprt);
			box_sys->add(projectile_entity, {0.f, 0.f, -4.6f, glsl_helper::projectile_width, glsl_helper::projectile_height});
			texture_vbo_sys->add(projectile_entity);

			// projectile_sys -> add_entity(projectile)
			// buttons.erase(GLFW_MOUSE_BUTTON_RIGHT);

		}

		else if (selected_tile_type == game::tile_type::INSULATION_FOAM)
		{
			int debris_x = (float)(player_pos.x * game::box2d_scale + cos(angle) * 5.0f);
			int debris_y = (float)(player_pos.y * game::box2d_scale + sin(angle) * 5.0f);

			for(int i = 0; i < 10; i++)
			{
				game_engine::task_scheduler_pointer->add_task({&game::create_single_debris_task,
					new game::create_debris_params{
						(float)(debris_x + cos(angle) * 2.0f),
						(float)(debris_y + sin(angle) * 2.0f),
						(float)(cos(angle + (rand() % 5) / 10.0) * 12.0f),
						(float)(sin(angle + (rand() % 5) / 10.0) * 12.0f),
						0.5f,
						game::tile_type::INSULATION_FOAM,
						0,
						game::tile_type::INSULATION_FOAM,
						5,
						0.0f,
						(uint16_t)(rand() % 30 + 20)
					}}
					// , i * 50
				);
				angle += 0.025f;
			}
		}

		// if (mouse_click_counter % 50 == 0)
		// {
		// 	// spawn one 82 task
		// 	game_engine::task_scheduler_pointer->add_task({&game::create_single_debris_task,
		// 		new game::create_debris_params{
		// 			(float)(debris_x + cos(angle) * 2.0f),
		// 			(float)(debris_y + sin(angle) * 2.0f),
		// 			(float)(cos(angle) * 12.0f),
		// 			(float)(sin(angle) * 12.0f),
		// 			0.5f,
		// 			82,
		// 			0,
		// 			82,
		// 			5,
		// 			0.0f,
		// 			(uint16_t)(rand() % 30 + 20)
		// 		}}
		// 	);
		// }

		// conveyor belt placement mode
		// if(mouse_click_counter % 2 == 0)
		// {
		// 	last_mouse_click = {xpos, ypos};
		// } else
		// {
		// 	// create conveyor belt between last_mouse_click and current mouse position
		// 	// game::world_tile_system *world_tile_sys = (game::world_tile_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::world_tile_system>()));

		// 	game::tile_conveyor_system *tile_conveyor_sys = (game::tile_conveyor_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::tile_conveyor_system>()));
		// 	entity conveyor_entity = game_engine::game_engine_pointer->create_entity();
		// 	game::tile_conveyor conveyor;

		// 	game::box2d_system *b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::box2d_system>()));
		// 	entity player = game_engine::game_engine_pointer->player_entitiy;
		// 	b2Body *body = b2d_sys->get_dynamic_body(player);
		// 	b2Vec2 player_pos = body->GetPosition();
		// 	int world_x = (int)((1.0 * xpos / PIXEL_SCALE) - 0.5 * (1.0 * game_engine::window_width / PIXEL_SCALE) + player_pos.x * game::box2d_scale);
		// 	int world_y = (int)((1.0 * ypos / PIXEL_SCALE) - 0.5 * (1.0 * game_engine::window_height / PIXEL_SCALE) + player_pos.y * game::box2d_scale);
		// 	conveyor.start_x = world_x;
		// 	conveyor.start_y = world_y;
		// 	world_x = (int)((1.0 * last_mouse_click.first / PIXEL_SCALE) - 0.5 * (1.0 * game_engine::window_width / PIXEL_SCALE) + player_pos.x * game::box2d_scale);
		// 	world_y = (int)((1.0 * last_mouse_click.second / PIXEL_SCALE) - 0.5 * (1.0 * game_engine::window_height / PIXEL_SCALE) + player_pos.y * game::box2d_scale);
		// 	conveyor.end_x = world_x;
		// 	conveyor.end_y = world_y;
		// 	conveyor.speed = 1;
		// 	conveyor.moving = 1;
		// 	conveyor.teeth_spacing = 6;

		// 	tile_conveyor_sys->add_component(conveyor_entity, conveyor);
		// }

		else
		{
			game::world_tile_system *world_sys = (game::world_tile_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::world_tile_system>()));
			game_engine::box_system *box_sys = (game_engine::box_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::box_system>()));
			entity player = game_engine::game_engine_pointer->player_entitiy;
			game_engine::box_lerp &player_box = box_sys->get(player);
			int world_x = (int)((1.0 * xpos / PIXEL_SCALE) - 0.5 * (1.0 * game_engine::window_width / PIXEL_SCALE) + player_pos.x * game::box2d_scale + player_box.w / 2.f);
			int world_y = (int)((1.0 * ypos / PIXEL_SCALE) - 0.5 * (1.0 * game_engine::window_height / PIXEL_SCALE) + player_pos.y * game::box2d_scale + player_box.h / 2.f);
			if (mouse_click_counter % 2 == 1)
			{
				int start_x = (int)last_mouse_click.first;
				int start_y = (int)last_mouse_click.second;
				int end_x = world_x;
				int end_y = world_y;

				float ang = atan2(end_y - start_y, end_x - start_x);
				float dist = sqrt((end_x - start_x) * (end_x - start_x) + (end_y - start_y) * (end_y - start_y));
				uint16_t misc_data = 0;
				int number_of_tiles = 1;
				if (selected_tile_type == game::tile_type::WOOD || selected_tile_type == game::tile_type::EMBER)
				{
					misc_data = 200;
				}
				if (selected_tile_type == game::tile_type::WOOD || selected_tile_type == game::tile_type::EMBER || selected_tile_type == game::tile_type::WATER)
				{
					number_of_tiles = 3;
				}


				for (float d = 0; d < dist; d += 1.0f)
				{
					int tx = (int)(start_x + cos(ang) * d);
					int ty = (int)(start_y + sin(ang) * d);
					for (int i = 0; i < number_of_tiles; i++)
					{
						world_sys->try_place_tile_with_displacement_no_lock(tx, ty, selected_tile_type, 20, misc_data, 0, 128);
					}
					// world_sys->set_tile_at_with_lock(tx, ty, (uint8_t)selected_tile_type);
					// if (selected_tile_type == game::tile_type::WOOD || selected_tile_type == game::tile_type::EMBER)
					// {
					// 	world_sys->add_tile_misc_data_at(tx, ty, 1000);
					// }
				}
			}
			else
			{
				last_mouse_click = {world_x, world_y};
			}
			mouse_click_counter++;
		}

		buttons.erase(GLFW_MOUSE_BUTTON_RIGHT);
	}
}

uint8_t physics_loop_running = 1;

// function to spawn threads on
void do_outlining(game::chunk *c, std::vector<std::vector<std::pair<float, float>>> *chunk_outline)
{
	c->create_outlines(chunk_outline);
}

// start_time = 0;
std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

void start_physics_thread()
{

	// for(int i = 0; i < 256; i++)
	// {
	// 	printf("\t\t%0.4ff, %0.4ff, %0.4ff, %0.4ff,\t//%d\n", i / 255.0, 1.0, 1.0, 1.0, i);
	// }

	game_engine::engine *engine_ptr = game_engine::game_engine_pointer;
	game::box2d_system *b2d_sys = (game::box2d_system *)(engine_ptr->get_system(game_engine::family::type<game::box2d_system>()));
	game::world_tile_system *world_sys = (game::world_tile_system *)(engine_ptr->get_system(game_engine::family::type<game::world_tile_system>()));
	game_engine::render_system *render_sys = (game_engine::render_system *)(engine_ptr->get_system(game_engine::family::type<game_engine::render_system>()));
	game::tile_arcing_system *tile_arcing_sys = (game::tile_arcing_system *)(engine_ptr->get_system(game_engine::family::type<game::tile_arcing_system>()));

	start_time = std::chrono::high_resolution_clock::now();
	const int tick_rate = 20;
	uint64_t tick_count = 0;

	// record average time of physics loop every 100 ticks
	uint64_t total_time = 0;

	while (physics_loop_running)
	{

		// custom_key_callback(engine_ptr->pressed_keys);
		// custom_mouse_callback(glfwGetCurrentContext(), engine_ptr->pressed_mouse_buttons);
		// start time
		auto start = std::chrono::high_resolution_clock::now();

		// if (tick_count % 100 == 0 && tick_count != 0)
		// {
		// 	printf("Average physics loop time: %lld ms\n", total_time / 100);
		// 	total_time = 0;
		// }

		world_sys->update(tick_count++);
		// printf("Physics Task count: %d\n", world_sys->task_scheduler_pointer->get_active_task_count());
		// tile_arcing_sys->update(tick_count);
		// printf("Tile movements took %lld ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count());
		// std::array<entity, game::NUM_CHUNKS> chunk_entities = world_sys->get_chunk_entities();
		// std::array<game::chunk *, game::NUM_CHUNKS> *chunks = world_sys->get_chunks_base();

		// std::array<std::vector<std::vector<std::pair<float, float>>> *, game::NUM_CHUNKS> chunks_outlines;
		// std::array<std::thread, game::NUM_CHUNKS> threads;

		// std::array<uint8_t, game::NUM_CHUNKS> modified_chunks = *(world_sys->get_modified_chunks());

		// for (int i = 0; i < game::NUM_CHUNKS; i++)
		// {
		// 	if (modified_chunks.at(i) == 0)
		// 		continue;
		// 	game::chunk *c = chunks->at(i);
		// 	chunks_outlines[i] = new std::vector<std::vector<std::pair<float, float>>>();
		// 	threads[i] = std::thread(do_outlining, c, chunks_outlines[i]);
		// 	// threads.push_back(std::thread((c->create_outlines), chunks_outlines[i]));
		// }

		// printf("Physics Task count 2: %d\n", world_sys->task_scheduler_pointer->get_active_task_count());

		// for (int i = 0; i < game::NUM_CHUNKS; i++)
		// {
		// 	if (modified_chunks.at(i) == 0)
		// 		continue;

		// 	world_sys->set_modified_chunk(i % game::CHUNKS_WIDTH, i / game::CHUNKS_WIDTH, 0);

		// 	entity e = chunk_entities[i];
		// 	threads[i].join();
		// 	b2d_sys->update_static_outlines(e, chunks_outlines[i]);
		// 	delete chunks_outlines[i];
		// }
		// printf("Physics Task count 3: %d\n", world_sys->task_scheduler_pointer->get_active_task_count());

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		// total_time += duration;
		// world_sys->add_time(duration);
		// sleep for 1 / tick_rate seconds
		if (duration < 1000.0 / tick_rate)
			std::this_thread::sleep_for(std::chrono::milliseconds(uint64_t((1000 - duration / 1000.0) / tick_rate)));
	}
}

void run_game(GLFWwindow *window)
{
	// Create the engine and systems
	game_engine::engine eng;

	game_engine::task_scheduler task_sc;
	task_sc.set_max_threads(16);
	game_engine::task_scheduler_pointer = &task_sc;

	task_sc.add_task_name((uint64_t)&game_engine::task_scheduler::print_task_counter, "print_task_counter");
	// task name here
	task_sc.add_task_name((uint64_t)&game::shutdown_task_schedular_task, "shutdown_task_schedular_task");
	task_sc.add_task_name((uint64_t)&game::create_debris_task, "create_debris_task");
	task_sc.add_task_name((uint64_t)&game::create_single_debris_task, "create_single_debris_task");
	task_sc.add_task_name((uint64_t)&game::delete_circle_task, "delete_circle_task");
	task_sc.add_task_name((uint64_t)&game::update_tile_task, "update_tile_task");
	task_sc.add_task_name((uint64_t)&game::update_tile_flush_task, "update_tile_flush_task");
	task_sc.add_task_name((uint64_t)&game::create_flying_creature_nest_task, "create_flying_creature_nest_task");
	task_sc.add_task_name((uint64_t)&game::flying_creature_eat_task, "flying_creature_eat_task");
	task_sc.add_task_name((uint64_t)&game::flying_creature_deposit_task, "flying_creature_deposit_task");

	task_sc.add_task_name((uint64_t)&game::update_chunk_tiles_task_wrapper, "update_chunk_tiles_task_wrapper");
	task_sc.add_task_name((uint64_t)&game::recalculate_chunk_outlines_task, "recalculate_chunk_outlines_task");
	task_sc.add_task_name((uint64_t)&game::update_chunk_tiles_texture_task, "update_chunk_tiles_texture_task");
	task_sc.add_task_name((uint64_t)&game::generate_chunk_task_wrapper, "generate_chunk_task_wrapper");

	std::thread task_runner(&game_engine::task_scheduler::start, &task_sc);

	game_engine::shader_programs = load_shaders(glsl_helper::vert_0(), glsl_helper::frag_0());

	// use the shader program
	glUseProgram(game_engine::shader_programs[0]);
	GLint colours_location = glGetUniformLocation(game_engine::shader_programs[0], "colours");
	glUniform4fv(colours_location, (GLsizei)glsl_helper::colours.size() / 4, glsl_helper::colours.data());

	// generic shader
	game_engine::shader_programs.push_back(load_shaders(glsl_helper::vert_1(), glsl_helper::frag_1())[0]);

	// generic texture shader
	game_engine::shader_programs.push_back(load_shaders(glsl_helper::vert_2(), glsl_helper::frag_2())[0]);

	printf("Error_1.5: x%d\n", glGetError());
	game_engine::shader_programs.push_back(load_shaders(glsl_helper::vert_2(), glsl_helper::temperature_overlay_shader())[0]);
	printf("Error_1.5: x%d\n", glGetError());

	GLuint compute_shader = load_compute_shader(glsl_helper::light_compute_shader());

	GLuint light_blurring_compute_shader = load_compute_shader(glsl_helper::light_blurring_compute_shader());

	game_engine::box_system *box_sys = new game_engine::box_system();
	eng.add_system(game_engine::family::type<game_engine::box_system>(), box_sys);

	game_engine::render_system *render_sys = new game_engine::render_system(window);
	eng.add_system(game_engine::family::type<game_engine::render_system>(), render_sys);
	render_sys->set_key_callback(custom_key_callback);
	render_sys->set_mouse_callback(custom_mouse_callback);

	std::unordered_map<uint32_t, std::string> system_names;

	game_engine::texture_vbo_system *texture_vbo_sys = new game_engine::texture_vbo_system(box_sys);
	eng.add_system(game_engine::family::type<game_engine::texture_vbo_system>(), texture_vbo_sys);
	// system_names[game_engine::family::type<game_engine::texture_vbo_system>()] = "texture_vbo_system";

	game::world_tile_system *world_sys = new game::world_tile_system();
	eng.add_system(game_engine::family::type<game::world_tile_system>(), world_sys);
	system_names[game_engine::family::type<game::world_tile_system>()] = "world_tile_system";

	game::projectile_system *projectile_sys = new game::projectile_system();
	eng.add_system(game_engine::family::type<game::projectile_system>(), projectile_sys);
	system_names[game_engine::family::type<game::projectile_system>()] = "projectile_system";

	game::box2d_system *box2d_sys = new game::box2d_system();
	eng.add_system(game_engine::family::type<game::box2d_system>(), box2d_sys);
	system_names[game_engine::family::type<game::box2d_system>()] = "box2d_system      ";

	game::tree_system *tree_sys = new game::tree_system(world_sys);
	eng.add_system(game_engine::family::type<game::tree_system>(), tree_sys);
	system_names[game_engine::family::type<game::tree_system>()] = "tree_system       ";

	game::chunk_frame_system *chunk_frame_sys = new game::chunk_frame_system(box2d_sys, render_sys, world_sys);
	eng.add_system(game_engine::family::type<game::chunk_frame_system>(), chunk_frame_sys);
	// system_names[game_engine::family::type<game::chunk_frame_system>()] = "chunk_frame_system";

	game::tile_pathfinding_system *tile_pathfinding_sys = new game::tile_pathfinding_system(world_sys);
	eng.add_system(game_engine::family::type<game::tile_pathfinding_system>(), tile_pathfinding_sys);
	system_names[game_engine::family::type<game::tile_pathfinding_system>()] = "tile_pathfinding_system";

	game::legged_creature_system *legged_creature_sys = new game::legged_creature_system(box2d_sys, render_sys, box_sys, texture_vbo_sys, world_sys, tile_pathfinding_sys);
	eng.add_system(game_engine::family::type<game::legged_creature_system>(), legged_creature_sys);
	// system_names[game_engine::family::type<game::legged_creature_system>()] = "legged_creature_system";

	game::tile_arcing_system *tile_arcing_sys = new game::tile_arcing_system(world_sys, box_sys);
	eng.add_system(game_engine::family::type<game::tile_arcing_system>(), tile_arcing_sys);
	// system_names[game_engine::family::type<game::tile_arcing_system>()] = "tile_arcing_system";

	game::building_component_system *building_component_sys; // = new game::building_component_system(world_sys, box_sys);
	// eng.add_system(game_engine::family::type<game::building_component_system>(), building_component_sys);

	std::thread tree_thread(&game::tree_system::start, tree_sys);
	std::thread tile_pathfinding_thread(&game::tile_pathfinding_system::start_thread, tile_pathfinding_sys);

	game::flying_creature_system *flying_creature_sys = new game::flying_creature_system(box2d_sys, render_sys, box_sys, texture_vbo_sys, tile_pathfinding_sys, world_sys);
	eng.add_system(game_engine::family::type<game::flying_creature_system>(), flying_creature_sys);
	system_names[game_engine::family::type<game::flying_creature_system>()] = "flying_creature_system";

	game::tile_conveyor_system *tile_conveyor_sys = new game::tile_conveyor_system(world_sys, tile_pathfinding_sys);
	eng.add_system(game_engine::family::type<game::tile_conveyor_system>(), tile_conveyor_sys);
	// system_names[game_engine::family::type<game::tile_conveyor_system>()] = "tile_conveyor_system";
	box2d_sys->world->SetContactListener(new game::b2_contact_listener());

	game::inventory_system *inventory_sys = new game::inventory_system();
	eng.add_system(game_engine::family::type<game::inventory_system>(), inventory_sys);

	world_sys->generate_world();
	printf("here: %d\n", glGetError());

	GLuint background_texture;
	glGenTextures(1, &background_texture);
	glBindTexture(GL_TEXTURE_2D, background_texture);
	// set data and size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	std::array<uint8_t, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH * game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH> *background_data = new std::array<uint8_t, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH * game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH>();
	for (int y = 0; y < game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH; y++)
	{
		for (int x = 0; x < game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH; x++)
		{
			// if((x % 12) == 0 || (y % 5) == 0) {
			/*
			if (((x + 8 * abs(((y / 6) % 6) - 3)) % 16) == 0 || (y % 6) == 0)
			{
				(*background_data)[y * game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH + x] = game::MORTAR;
			}
			else
			{
				if (((x + 8 * abs(((y / 6) % 6) - 3)) / 16) % 2 == 0)
					(*background_data)[y * game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH + x] = game::BRICK_3;
				else
					(*background_data)[y * game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH + x] = game::BRICK_4;
			}
			*/
			
			if (((x + 12 * abs(((y / 10) % 10) - 4)) % 25) == 0 || (y % 10) == 0)
			{
				(*background_data)[y * game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH + x] = game::MORTAR;
			}
			else
			{
				if (((x + 12 * abs(((y / 10) % 10) - 4)) / 25) % 2 == 0)
					(*background_data)[y * game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH + x] = game::BRICK_3;
				else
					(*background_data)[y * game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH + x] = game::BRICK_4;
			}

			// (*background_data)[y * game::CHUNK_SIZE * game::CHUNKS_WIDTH + x] = 55;

			// if(((x + 8 * ((y / 6) % 16) + 16 * ((y / 6) % 6 - 3)) % 16) == 0 || (y % 6) == 0) {
			// 	(*background_data)[y * game::CHUNK_SIZE * game::CHUNKS_WIDTH + x] = game::MORTAR;
			// } else {
			// 	if(((x + 8 * ((y / 6) % 16) + 16 * ((y / 6) % 6 - 3)) / 16) % 2 == 0)
			// 		(*background_data)[y * game::CHUNK_SIZE * game::CHUNKS_WIDTH + x] = game::BRICK_3;
			// 	else
			// 		(*background_data)[y * game::CHUNK_SIZE * game::CHUNKS_WIDTH + x] = game::BRICK_4;
			// }
			// if(((x + 6 * ((y / 5) % 12) + 24 * ((y / 15) % 12 - 6)) % 12) == 0 || (y % 5) == 0) {
			// 	(*background_data)[y * game::CHUNK_SIZE * game::CHUNKS_WIDTH + x] = game::MORTAR;
			// } else {
			// 	if(((x + 6 * ((y / 5) % 12) + 24 * ((y / 15) % 12 - 6)) / 12) % 2 == 0)
			// 		(*background_data)[y * game::CHUNK_SIZE * game::CHUNKS_WIDTH + x] = game::BRICK_3;
			// 	else
			// 		(*background_data)[y * game::CHUNK_SIZE * game::CHUNKS_WIDTH + x] = game::BRICK_4;
			// }
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, 0, GL_RED, GL_UNSIGNED_BYTE, background_data->data());
	delete background_data;

	entity background_entity = eng.create_entity();
	printf("here1: %d\n", glGetError());
	box_sys->add(background_entity, {0.f, 0.f, -6.0, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH});
	// sleep thread for 100 ms

	printf("here2. texture_vbo_sys pointer: %llu\n", (texture_vbo_sys));
	// texture_vbo_sys ->get_counter();
	printf("Texture VBO system counter: %llu\n", texture_vbo_sys->get_counter());
	fflush(stdout);
	texture_vbo_sys->increment_counter();
	printf("Texture VBO system counter: %llu\n", texture_vbo_sys->get_counter());
	fflush(stdout);
	printf("background entity id: %llu\n", background_entity);
	fflush(stdout);

	printf("Box system family type: %u\n", game_engine::family::type<game_engine::box_system>());
	fflush(stdout);
	game_engine::box_system *box_sys_2 = (game_engine::box_system *)eng.get_system(game_engine::family::type<game_engine::box_system>());
	printf("Box system test: %p, %p\n", box_sys, box_sys_2);
	fflush(stdout);

	game_engine::box b = box_sys->get(background_entity).get_box();
	printf("Background box: %f, %f, %f, %f, %f\n", b.x, b.y, b.z, b.w, b.h);
	fflush(stdout);

	texture_vbo_sys->add(background_entity);
	printf("here3: %d\n", glGetError());
	fflush(stdout);
	game_engine::sprite sprt(game_engine::shader_programs[0], 0);
	printf("Texture is_ui: %d\n", sprt.is_ui);
	printf("here4: %d\n", glGetError());
	sprt.add_texture({background_texture, 0, GL_R8, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH});
	printf("here5: %d\n", glGetError());
	render_sys->add(background_entity, sprt);
	// (GLuint id, GLuint binding, GLuint format, uint32_t width, uint32_t height)

	printf("here: %d\n", glGetError());
	GLuint temperature_overlay_texture;
	glGenTextures(1, &temperature_overlay_texture);
	glBindTexture(GL_TEXTURE_2D, temperature_overlay_texture);
	// set data and size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, 0, GL_RED, GL_FLOAT, 0);

	printf("Error_1.15: x%d\n", glGetError());
	entity temperature_overlay_entity = eng.create_entity();
	box_sys->add(temperature_overlay_entity, {0.f, 0.f, -4.7, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH});
	texture_vbo_sys->add(temperature_overlay_entity);
	game_engine::sprite temp_sprt(game_engine::shader_programs[3], 0);
	temp_sprt.add_texture({temperature_overlay_texture, 0, GL_R32F, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH});
	render_sys->add(temperature_overlay_entity, temp_sprt);
	printf("Error_1.5: x%d\n", glGetError());

	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint chunk_texture;
	glGenTextures(1, &chunk_texture);
	glBindTexture(GL_TEXTURE_2D, chunk_texture);
	// set data and size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	printf("before settingh chunk textures: %d\n", glGetError());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	printf("before settingh chunk textures: %d\n", glGetError());

	// for (int i = 0; i < game::NUM_CHUNKS; i++)
	// {
	// 	int x = i % game::CHUNKS_WIDTH;
	// 	int y = i / game::CHUNKS_WIDTH;

	// 	glTexSubImage2D(GL_TEXTURE_2D, 0, x * game::CHUNK_SIZE, y * game::CHUNK_SIZE, game::CHUNK_SIZE, game::CHUNK_SIZE, GL_RED, GL_UNSIGNED_BYTE, chunks_data[i]->data());
	// }

	printf("after settingh chunk textures: %d\n", glGetError());

	glBindTexture(GL_TEXTURE_2D, 0);

	entity all_chunks_entity = eng.create_entity();
	box_sys->add(all_chunks_entity, {0.f, 0.f, -5.0, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH});
	texture_vbo_sys->add(all_chunks_entity);
	sprt = game_engine::sprite(game_engine::shader_programs[0], 0);
	sprt.add_texture({chunk_texture, 0, GL_R8, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH});
	render_sys->add(all_chunks_entity, sprt);

	world_sys->set_all_chunk_ent(all_chunks_entity);

	// for(int i = 0; i < game::NUM_CHUNKS; i++) {
	for (int y = 0; y < game::RENDERED_CHUNKS_WIDTH; y++)
	{
		for (int x = 0; x < game::RENDERED_CHUNKS_WIDTH; x++)
		{
			// if (x != 0 || y != 0)
			// {
			entity chunk_entity = world_sys->get_chunk_entity(x, y);
			std::vector<std::vector<std::pair<float, float>>> *outlines = world_sys->create_outlines(x, y);

			box2d_sys->create_static_bodies(chunk_entity, outlines);
			delete outlines;
			// chunk_outlines.push_back(outlines);
			// chunk_outlines.push_back(world_sys->create_outlines(x, y));
			float top_left_x = game::CHUNK_SIZE * x * 1.0f;
			float top_left_y = game::CHUNK_SIZE * y * 1.0f;
			// float top_left_x = 2 * x * game::CHUNK_SIZE;
			// float top_left_y = 2 * y * game::CHUNK_SIZE;

			box_sys->add(chunk_entity, {top_left_x * 1.0f, top_left_y * 1.0f, -5.0, game::CHUNK_SIZE * 1.0, game::CHUNK_SIZE * 1.0});

			// box_sys->add(chunk_entity, { top_left_x, top_left_y, 0.0, 1.0/3.0, 1.0/3.0});
			// box_sys->add(chunk_entity, { top_left_x, top_left_y, 1.0f, 1.0/3.0, 1.0/3.0});
			// box_sys->add(chunk_entity, { top_left_x, top_left_y, 1.0f, 8 * game::CHUNK_SIZE, 8 * game::CHUNK_SIZE});

			// box_sys->add(chunk_entity, { top_left_x, top_left_y, 0.50f, 8 * game::CHUNK_SIZE, 8 * game::CHUNK_SIZE});
			// texture_vbo_sys->add(chunk_entity);
			// render_sys->add(chunk_entity, {chunk_textures[y * game::CHUNKS_WIDTH + x]}, game_engine::shader_programs[0]);
			// }
		}
	}
	printf("after setting up chunks: %d\n", glGetError());
	fflush(stdout);
	// create player
	entity player_entity = eng.create_entity();
	GLuint player_texture;
	glsl_helper::create_character_texture(player_texture);
	sprt = game_engine::sprite(game_engine::shader_programs[0], 0);
	sprt.add_texture({player_texture, 0, GL_R8, glsl_helper::character_width, glsl_helper::character_height});
	render_sys->add(player_entity, sprt);
	// box_sys->add(player_entity, {0.f, 0.f, -1.f, 4 * 8.f, 1 * 85.333333333f});
	box_sys->add(player_entity, {0.f, 0.f, -4.9f, glsl_helper::character_width, glsl_helper::character_height});
	texture_vbo_sys->add(player_entity);
	eng.player_entitiy = player_entity;

	std::array<uint8_t, 4> bee_texture = {game::BEE_YELLOW, game::BEE_YELLOW, game::BEE_BLACK, game::BEE_BLACK};
	glsl_helper::create_texture_from_data("bee", bee_texture.data(), 2, 2);
	std::array<uint8_t, 16> spider_texture = {game::tile_type::AIR, game::tile_type::BEE_BLACK, game::tile_type::BEE_BLACK, game::tile_type::AIR,
											  game::tile_type::BEE_BLACK, game::tile_type::WHITE, game::tile_type::WHITE, game::tile_type::BEE_BLACK,
											  game::tile_type::BEE_BLACK, game::tile_type::GREY, game::tile_type::DARK_GREY, game::tile_type::BEE_BLACK,
											  game::tile_type::AIR, game::tile_type::BEE_BLACK, game::tile_type::BEE_BLACK, game::tile_type::AIR};
	glsl_helper::create_texture_from_data("spider", spider_texture.data(), 4, 4);

	// // create box2d body
	// std::vector<std::pair<float, float>> player_shape = {
	// 	{0.f, 0.f},
	// 	{glsl_helper::character_width, glsl_helper::character_height},
	// 	{glsl_helper::character_width, 0.f},
	// 	{glsl_helper::character_width, glsl_helper::character_height},
	// 	{0.f, 0.f},
	// 	{0.f, glsl_helper::character_height}};
	// std::vector<std::pair<float, float>> player_shape = {
	// 	{0.f, 0.f},
	// 	{(float)glsl_helper::character_width, (float)glsl_helper::character_height},
	// 	{(float)glsl_helper::character_width, 0.f},
	// 	{(float)glsl_helper::character_width, (float)glsl_helper::character_height},
	// 	{0.f, 0.f},
	// 	{0.f, (float)glsl_helper::character_height}};
	float edge_width = 0.1f;
	std::vector<std::pair<float, float>> player_shape = {
		{glsl_helper::character_width - edge_width, 0}, {edge_width, 0}, {0, edge_width}, {0, edge_width}, {glsl_helper::character_width, edge_width}, {glsl_helper::character_width - edge_width, 0}, {0, edge_width}, {0, glsl_helper::character_height - edge_width}, {glsl_helper::character_width, edge_width}, {0, glsl_helper::character_height - edge_width}, {glsl_helper::character_width, glsl_helper::character_height - edge_width}, {glsl_helper::character_width, edge_width}, {0, glsl_helper::character_height - edge_width}, {edge_width, glsl_helper::character_height}, {glsl_helper::character_width, glsl_helper::character_height - edge_width}, {edge_width, glsl_helper::character_height}, {glsl_helper::character_width - edge_width, glsl_helper::character_height}, {glsl_helper::character_width, glsl_helper::character_height - edge_width}

	};

	chunk_outlines.push_back({player_shape});
	box2d_sys->create_dynamic_body(player_entity, player_shape);
	b2Body *player_body = box2d_sys->get_dynamic_body(player_entity);
	b2Vec2 player_pos = player_body->GetPosition();
	world_sys->delete_circle((int)(player_pos.x * game::box2d_scale), (int)(player_pos.y * game::box2d_scale), 18, {});

	// create light components
	uint16_t light_texture_count = 48;
	std::vector<entity> light_entities;
	std::vector<GLuint> light_textures;
	// std::vector<GLuint> colour_textures;

	std::vector<uint32_t> light_data(game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH * game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH * 2, 0);

	for (int i = 0; i < light_texture_count + 1; i++)
	{
		GLuint light_texture;
		glGenTextures(1, &light_texture);
		glBindTexture(GL_TEXTURE_2D, light_texture);
		// set data and size
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, light_data.data());
		glBindTexture(GL_TEXTURE_2D, 0);
		light_textures.push_back(light_texture);

		// GLuint colour_texture;
		// glGenTextures(1, &colour_texture);
		// glBindTexture(GL_TEXTURE_2D, colour_texture);
		// // set data and size
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, light_data.data());
		// glBindTexture(GL_TEXTURE_2D, 0);
		// colour_textures.push_back(colour_texture);
	}

	GLuint master_light_texture;
	glGenTextures(1, &master_light_texture);
	glBindTexture(GL_TEXTURE_2D, master_light_texture);
	// set data and size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint master_light_texture_backup;
	glGenTextures(1, &master_light_texture_backup);
	glBindTexture(GL_TEXTURE_2D, master_light_texture_backup);
	// set data and size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// GLuint colour_light_texture;
	// glGenTextures(1, &colour_light_texture);
	// glBindTexture(GL_TEXTURE_2D, colour_light_texture);
	// // set data and size
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	// glBindTexture(GL_TEXTURE_2D, 0);
	GLuint blurred_light_texture;
	glGenTextures(1, &blurred_light_texture);
	glBindTexture(GL_TEXTURE_2D, blurred_light_texture);
	// set data and size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint blurred_light_texture_second;
	glGenTextures(1, &blurred_light_texture_second);
	glBindTexture(GL_TEXTURE_2D, blurred_light_texture_second);
	// set data and size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	entity mater_light_entity = eng.create_entity();

	box_sys->add(mater_light_entity, {0.f, 0.f, -4.5f, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH});
	texture_vbo_sys->add(mater_light_entity);
	sprt = game_engine::sprite(game_engine::shader_programs[2], 0);
	sprt.add_texture({master_light_texture, 0, GL_R32UI, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH});
	sprt.add_texture({blurred_light_texture, 1, GL_R32UI, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH});
	sprt.add_texture({chunk_texture, 2, GL_R8, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH});
	// glBindImageTexture(1, chunk_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);

	// sprt.add_texture({colour_light_texture, 2, GL_R32UI, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH});
	render_sys->add(mater_light_entity, sprt);

	// printf("Error_after: %d\n", glGetError());
	// std::vector<game_engine::vec2> normal_vectors(256);
	// for(int i = 0; i < normal_vectors.size(); i+=1){
	// 	normal_vectors[i] = {0.f, 1.f};
	// }
	printf("Error_before_normals_texture_creation: %d\n", glGetError());
	GLuint normal_vectors_texture;
	glGenTextures(1, &normal_vectors_texture);
	glBindTexture(GL_TEXTURE_1D, normal_vectors_texture);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	std::vector<game_engine::vec2> normal_vectors = game::load_normal_vectors();
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RG32F, 256, 0, GL_RG, GL_FLOAT, normal_vectors.data());
	printf("Error_after_normals_texture_creation: %d\n", glGetError());
	glBindTexture(GL_TEXTURE_1D, 0);

	// GLuint ssbo;
	// glGenBuffers(1, &ssbo);
	// glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	// glBufferData(GL_SHADER_STORAGE_BUFFER, 256 * sizeof(game_engine::vec2), normal_vectors.data(), GL_STATIC_DRAW );
	// glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);

	printf("Error_before_loading_compute_shader_2: %d\n", glGetError());
	GLuint blend_shader = load_compute_shader(glsl_helper::light_blending_compute_shader());
	printf("Error_after: %d\n", glGetError());

	// GLuint blend_colour_shader = load_compute_shader(glsl_helper::colour_blending_compute_shader());

	// GLuint projection_location = glGetUniformLocation(shader_programs[0], "projection");
	// glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection_matrix);
	// GLuint view_location = glGetUniformLocation(shader_programs[0], "view");
	// glUniformMatrix4fv(view_location, 1, GL_FALSE, view_matrix);
	// do this ^^ but with generic shader
	// GLuint projection_location = glGetUniformLocation(game_engine::shader_programs[1], "projection");
	// glUniformMatrix4fv(projection_location, 1, GL_FALSE, game_engine::projection_matrix);
	// GLuint view_location = glGetUniformLocation(game_engine::shader_programs[1], "view");
	// glUniformMatrix4fv(view_location, 1, GL_FALSE, game_engine::view_matrix);

	std::thread world_thread(start_physics_thread);
	std::thread box_2d_thread(&game::box2d_system::start_thread, box2d_sys);
	std::thread flying_creature_thread(&game::flying_creature_system::start_thread, flying_creature_sys);
	// std::thread chunk_frame_thread(&game::chunk_frame_system::start_thread, chunk_frame_sys);
	std::thread legged_creature_thread(&game::legged_creature_system::start_thread, legged_creature_sys);
	std::thread tile_conveyor_thread(&game::tile_conveyor_system::start_thread, tile_conveyor_sys);
	// std::thread tile_arcing_thread(&game::tile_arcing_system::start_thread, tile_arcing_sys);

	uint16_t saved_light_textures = 0;
	uint16_t light_texture_index = 0;
	uint64_t last_time_taken_micro = 0;
	uint64_t counter = 0;
	b2Vec2 player_vel = b2Vec2(0.0f, 0.0f);

	uint16_t frame_c = 0;
	// store start time
	auto last_fps_start = std::chrono::high_resolution_clock::now();

	// printf("before bee: %d\n", glGetError());
	entity new_bee = eng.create_entity();
	// flying_creature_sys -> create_flying_creature(new_bee, 10, 10, game::flying_creature_type::BEE);

	entity new_spider = 0;

	uint16_t centipede_length = 7;

	// for(int i = 0; i < centipede_length; i++)
	// {
	// 	entity old_spider = new_spider;
	// 	new_spider = eng.create_entity();

	// 	// if(old_spider == 0)
	// 	// {
	// 	// 	entity tile_arc = eng.create_entity();
	// 	// 	game::tile_arc tile_arc_to_spider = {game::tile_arc_type::ELECTRIC, player_entity, new_spider};
	// 	// 	tile_arcing_sys -> add(tile_arc, tile_arc_to_spider);
	// 	// }
	// 	world_sys->delete_circle(150, 50 + i * 10, 25, {});
	// 	legged_creature_sys -> create_legged_creature(new_spider, 150, 50 + i * 10, game::legged_creature_type::SPIDER, old_spider, 0);
	// }
	// legged_creature_sys -> create_legged_creature(new_spider, 50, 50, game::legged_creature_type::SPIDER);

	// printf("here: %d\n", glGetError());

	std::pair<int32_t, int32_t> last_player_chunk = {-1, -1};

	// inventory_sys
	entity hotbar_entity = eng.create_entity();
	{
		// std::array<uint8_t, 9> hotbar_texture_data = {game::tile_type::WOOD, game::tile_type::LAVA, game::tile_type::WOOD, game::tile_type::WOOD, game::tile_type::GLASS, game::tile_type::WOOD, game::tile_type::WOOD, game::tile_type::BEE_YELLOW, game::tile_type::WOOD};
		// glsl_helper::create_texture_from_data("hotbar_texture", hotbar_texture_data.data(), 9, 1);
		
		const uint16_t hotbar_texture_width = 9 * 9;
		const uint16_t hotbar_texture_height = 9;

		std::array<uint8_t, hotbar_texture_width * hotbar_texture_height> hotbar_texture_data;
		for(int i = 0; i < hotbar_texture_width * hotbar_texture_height; i++)
		{

			uint16_t x = i % (9 * 9);
			uint16_t y = i / (9 * 9);

			if (x % 9 == 0 || x % 9 == 8 || y % 9 == 0 || y % 9 == 8)
			{
				hotbar_texture_data[i] = game::GREY;
			}
			else
			{
				hotbar_texture_data[i] = game::VACCUUM;
			}
		}
		glsl_helper::create_texture_from_data("hotbar_texture", hotbar_texture_data.data(), hotbar_texture_width, hotbar_texture_height);


		float taskbar_width = (game_engine::window_width / PIXEL_SCALE) / 3.5f;
		float taskbar_height = taskbar_width / 9.f;

		// box_sys->add(hotbar_entity, {0.f, 0.f, -2.f, taskbar_width, taskbar_height});
		box_sys->add(hotbar_entity, {(game_engine::window_width / PIXEL_SCALE - taskbar_width) / 2.f, game_engine::window_height / PIXEL_SCALE - taskbar_height - 10.f, -2.f, taskbar_width, taskbar_height});
		texture_vbo_sys->add(hotbar_entity);
		game_engine::sprite hotbar_sprt(game_engine::shader_programs[0], 1);
		

		hotbar_sprt.add_texture({glsl_helper::texture_map["hotbar_texture"], 0, GL_R8, 9 * 9, 9});

		render_sys->add(hotbar_entity, hotbar_sprt);


		std::array<uint8_t, 11 * 11> selected_hotbar_texture_data;
		for(int i = 0; i < 11 * 11; i++)
		{
			uint16_t x = i % 11;
			uint16_t y = i / 11;

			if (x % 11 == 0 || x % 11 == 10 || y % 11 == 0 || y % 11 == 10)
			{
				selected_hotbar_texture_data[i] = game::WHITE;
			}
			else
			{
				selected_hotbar_texture_data[i] = game::AIR;
			}
		}
		glsl_helper::create_texture_from_data("selected_hotbar_texture", selected_hotbar_texture_data.data(), 11, 11);
		selected_hotbar_entity = eng.create_entity();

		float selected_taskbar_width = taskbar_width / 9.f;
		float selected_taskbar_height = taskbar_height;
		box_sys->add(selected_hotbar_entity, {(game_engine::window_width / PIXEL_SCALE - taskbar_width) / 2.f + 0 * (taskbar_width / 9.f), game_engine::window_height / PIXEL_SCALE - taskbar_height - 10.f, -1.9f, selected_taskbar_width, selected_taskbar_height});
		texture_vbo_sys->add(selected_hotbar_entity);
		game_engine::sprite selected_hotbar_sprt(game_engine::shader_programs[0], 1);
		selected_hotbar_sprt.add_texture({glsl_helper::texture_map["selected_hotbar_texture"], 0, GL_R8, 11, 11});
		render_sys->add(selected_hotbar_entity, selected_hotbar_sprt);




		game::inventory inventory(9, 1);
		
		inventory.add_item(0, 0, game::inventory_item(1, game::tile_type::WATER));
		inventory_sys->add_inventory(hotbar_entity, inventory);

	}


	// Run the game loop
	while (!glfwWindowShouldClose(window))
	{
		auto start_rendering_loop = std::chrono::high_resolution_clock::now();
		uint64_t start_rendering_loop_micro = (std::chrono::time_point_cast<std::chrono::microseconds>(start_rendering_loop)).time_since_epoch().count();

		game::b2d_mutex.lock();
		// set player velocity again
		player_body->SetLinearVelocity(player_vel);
		// box2d_sys->update(last_time_taken_micro);
		projectile_sys->update(counter);
		game::b2d_mutex.unlock();
		flying_creature_sys->update_rendering(last_time_taken_micro);
		// legged_creature_sys->update_rendering(last_time_taken_micro);


		game_engine::box b = box_sys->get(player_entity).get_box_lerped(start_rendering_loop_micro);
		int player_x = (int)(b.x + b.w / 2);
		int player_y = (int)(b.y + b.h / 2);
		int32_t player_chunk_x = (int32_t)(player_x) / (game::CHUNK_SIZE);
		int32_t player_chunk_y = (int32_t)(player_y) / (game::CHUNK_SIZE);

		// game::chunk * player_chunk = world_sys->get_chunk(player_chunk_x, player_chunk_y);
		// player_chunk->lock_chunk_copy_shared();
		// render_sys->update_texture_section(world_sys->all_chunk_ent, (uint8_t *)(player_chunk->get_data_copy()), (player_chunk_x) * game::CHUNK_SIZE, (player_chunk_y) * game::CHUNK_SIZE, game::CHUNK_SIZE, game::CHUNK_SIZE);
		// player_chunk->unlock_chunk_copy_shared();

		// int chunk_x = player_x / game::CHUNK_SIZE;
		// int chunk_y = player_y / game::CHUNK_SIZE;
		// uint16_t intra_chunk_x = player_x % game::CHUNK_SIZE;
		// uint16_t intra_chunk_y = player_y % game::CHUNK_SIZE;

		if (player_x < 0)
		{
			player_chunk_x -= 1;
		}
		if (player_y < 0)
		{
			player_chunk_y -= 1;
		}

		int32_t start_chunk_x = player_chunk_x - game::RENDERED_CHUNKS_WIDTH / 2;
		int32_t start_chunk_y = player_chunk_y - game::RENDERED_CHUNKS_WIDTH / 2;

		if (last_player_chunk.first != player_chunk_x || last_player_chunk.second != player_chunk_y)
		{
			// printf("Player chunk changed to: %d, %d\n", player_chunk_x, player_chunk_y);
			// need to redraw the master_light_texture so that the old lighting from previous frames lines up with the new lighting
			// copy to backup texture
			glCopyImageSubData(master_light_texture, GL_TEXTURE_2D, 0, 0, 0, 0,
							   master_light_texture_backup, GL_TEXTURE_2D, 0, 0, 0, 0,
							   game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, 1);

			// now clear master_light_texture
			glClearTexImage(master_light_texture, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);

			int source_x = max((player_chunk_x - last_player_chunk.first) * game::CHUNK_SIZE, 0);
			int source_y = max((player_chunk_y - last_player_chunk.second) * game::CHUNK_SIZE, 0);
			int dest_x = max((player_chunk_x - last_player_chunk.first) * -game::CHUNK_SIZE, 0);
			int dest_y = max((player_chunk_y - last_player_chunk.second) * -game::CHUNK_SIZE, 0);

			int copy_width = game::CHUNK_SIZE * (game::RENDERED_CHUNKS_WIDTH - abs(player_chunk_x - last_player_chunk.first));
			int copy_height = game::CHUNK_SIZE * (game::RENDERED_CHUNKS_WIDTH - abs(player_chunk_y - last_player_chunk.second));

			// printf("Copying light texture from backup to master with source (%d, %d), dest (%d, %d), size (%d, %d)\n", source_x, source_y, dest_x, dest_y, copy_width, copy_height);

			// now copy from backup to master_light_texture with offset
 			glCopyImageSubData(master_light_texture_backup, GL_TEXTURE_2D, 0,
							   source_x,
							   source_y,
							   0,
							   master_light_texture, GL_TEXTURE_2D, 0,
							   dest_x,
							   dest_y,
							   0,
							   copy_width,
							   copy_height,
							   1);

			// do the same for all the textures in light_textures
			for (uint16_t i = 0; i < light_texture_count; i++)
			{
				glCopyImageSubData(light_textures[i], GL_TEXTURE_2D, 0,
								   0,
								   0,
								   0,
								   master_light_texture_backup, GL_TEXTURE_2D, 0, 0, 0, 0,
								   game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, 1);

				glClearTexImage(light_textures[i], 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
				glCopyImageSubData(master_light_texture_backup, GL_TEXTURE_2D, 0,
								   source_x,
								   source_y,
								   0,
								   light_textures[i], GL_TEXTURE_2D, 0, 
								   dest_x,
								   dest_y, 
								   0,
								   copy_width,
								   copy_height,
								   1);
			}

			last_player_chunk.first = player_chunk_x;
			last_player_chunk.second = player_chunk_y;

			// move all_chunks_entity
			game_engine::box_lerp all_chunks_box = box_sys->get(world_sys->all_chunk_ent);
			all_chunks_box.x = start_chunk_x * game::CHUNK_SIZE;
			all_chunks_box.y = start_chunk_y * game::CHUNK_SIZE;
			all_chunks_box.x2 = start_chunk_x * game::CHUNK_SIZE;
			all_chunks_box.y2 = start_chunk_y * game::CHUNK_SIZE;
			box_sys->update_box(world_sys->all_chunk_ent, all_chunks_box);
			texture_vbo_sys->update(world_sys->all_chunk_ent, all_chunks_box.get_box());
			all_chunks_box.z = -4.5f;
			box_sys->update_box(mater_light_entity, all_chunks_box);
			texture_vbo_sys->update(mater_light_entity, all_chunks_box.get_box());

			all_chunks_box.z = -6.0f;
			box_sys->update_box(background_entity, all_chunks_box);
			texture_vbo_sys->update(background_entity, all_chunks_box.get_box());

			// temperature overlay
			all_chunks_box.z = -4.7f;
			box_sys->update_box(temperature_overlay_entity, all_chunks_box);
			texture_vbo_sys->update(temperature_overlay_entity, all_chunks_box.get_box());
		}

		for (int32_t y = 0; y < game::RENDERED_CHUNKS_WIDTH; y++)
		{
			for (int32_t x = 0; x < game::RENDERED_CHUNKS_WIDTH; x++)
			{
				int32_t current_chunk_x = start_chunk_x + x;
				int32_t current_chunk_y = start_chunk_y + y;
				if (world_sys->get_chunk(current_chunk_x, current_chunk_y) == nullptr)
					continue;
				game::chunk *chunk = world_sys->get_chunk(current_chunk_x, current_chunk_y);
				chunk->lock_chunk_copy_shared();
				render_sys->update_texture_section(world_sys->all_chunk_ent, (uint8_t *)(chunk->get_data_copy()), (x)*game::CHUNK_SIZE, (y)*game::CHUNK_SIZE, game::CHUNK_SIZE, game::CHUNK_SIZE);
				render_sys->update_texture_section_float(temperature_overlay_entity, (float *)((chunk->get_temperature_data()->data())), (x)*game::CHUNK_SIZE, (y)*game::CHUNK_SIZE, game::CHUNK_SIZE, game::CHUNK_SIZE);
				chunk->unlock_chunk_copy_shared();
			}
		}

		// game::chunk *player_chunk = world_sys->get_chunk(player_chunk_x, player_chunk_y);
		// player_chunk->lock_chunk_copy_shared();
		// render_sys->update_texture_section(world_sys->all_chunk_ent, (uint8_t *)(player_chunk->get_data_copy()), 2 * game::CHUNK_SIZE, 2 * game::CHUNK_SIZE, game::CHUNK_SIZE, game::CHUNK_SIZE);
		// player_chunk->unlock_chunk_copy_shared();

		if (counter % 40 == 0)
		{
			printf("\t\tPlayer chunk: %d, %d\n", player_chunk_x, player_chunk_y);
		}

		// for (int i = 0; i < game::NUM_CHUNKS; i++)
		// {
		// 	game::chunk *chunk = world_sys->get_chunks_base()->at(i);
		// 	chunk->lock_chunk_copy_shared();
		// 	render_sys->update_texture_section(world_sys->all_chunk_ent, (uint8_t *)(chunk->get_data_copy()), (i % game::CHUNKS_WIDTH) * game::CHUNK_SIZE, (i / game::CHUNKS_WIDTH) * game::CHUNK_SIZE, game::CHUNK_SIZE, game::CHUNK_SIZE);
		// 	chunk->unlock_chunk_copy_shared();
		// }

		// printf("b2d_time: %lums\n", duration_b2d.count() / 1000);

		game_engine::view_matrix[12] = float(-b.x - 0.5 * glsl_helper::character_width + game_engine::window_width * (1.0 / (2 * PIXEL_SCALE)));
		game_engine::view_matrix[13] = float(-b.y - 0.5 * glsl_helper::character_height + game_engine::window_height * (1.0 / (2 * PIXEL_SCALE)));

		// printf("before_update_call: %d\n", glGetError());

		// printf("before_clearing_l_texture: %d\n", glGetError());
		// glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, light_textures[light_texture_index]);
		glClearTexImage(light_textures[light_texture_index], 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
		glBindTexture(GL_TEXTURE_2D, blurred_light_texture);
		glClearTexImage(blurred_light_texture, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		// trace lights with compute shader
		glUseProgram(compute_shader);
		// bind world textures
		// printf("before_binding_textures: %d\n", glGetError());
		glBindImageTexture(0, light_textures[light_texture_index], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glBindImageTexture(1, chunk_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
		GLuint texture_size = glGetUniformLocation(compute_shader, "texture_size");
		glUniform2i(texture_size, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH);
		GLuint frame_count = glGetUniformLocation(compute_shader, "frame_count");

		glUniform1ui(frame_count, (uint32_t)counter);

		GLint player_pos = glGetUniformLocation(compute_shader, "player_pos");
		// glUniform2f(player_pos, (float)(b.x + glsl_helper::character_width / 2.0), (float)(b.y + glsl_helper::character_width / 2.0));

		glUniform2f(player_pos, (float)((b.x - start_chunk_x * game::CHUNK_SIZE) + glsl_helper::character_width / 2.0 + (rand() % 100 - 50) / 50.0), (float)((b.y - start_chunk_y * game::CHUNK_SIZE) + glsl_helper::character_width / 2.0 + (rand() % 100 - 50) / 50.0));

		// GLint vector_location = glGetUniformLocation(compute_shader, "normal_vectors");
		// glUniform2fv(vector_location, 256, (GLfloat*)game::noramal_vectors.data());
		glBindImageTexture(2, normal_vectors_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
		// printf("before_binding_col_texture: %d\n", glGetError());
		// glBindImageTexture(3, colour_textures[light_texture_index], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		// printf("after_binding_col_texture: %d\n", glGetError());

		glDispatchCompute(16000, 1, 1);
		// printf("after dispatch: %d\n", glGetError());
		glFinish();

		glUseProgram(blend_shader);
		glBindImageTexture(0, master_light_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glBindImageTexture(1, light_textures[light_texture_index], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		glBindImageTexture(2, light_textures[(light_texture_index + 1) % light_texture_count], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);

		glDispatchCompute(game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, 1);

		glFinish();

		// glUseProgram(blend_colour_shader);

		// glBindImageTexture(0, colour_light_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		// glBindImageTexture(1, colour_textures[light_texture_index], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		// glBindImageTexture(2, colour_textures[(light_texture_index + 1) % light_texture_count], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		// GLint total_frames = glGetUniformLocation(blend_colour_shader, "total_frames");
		// glUniform1i(total_frames, light_texture_count);
		// glDispatchCompute(game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH, 1);
		// glFinish();

		glUseProgram(light_blurring_compute_shader);
		glBindImageTexture(0, blurred_light_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glBindImageTexture(1, master_light_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		glDispatchCompute(game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, 1);
		glFinish();

		glBindImageTexture(0, blurred_light_texture_second, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glBindImageTexture(1, blurred_light_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		glDispatchCompute(game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, 1);
		glFinish();

		glBindImageTexture(0, blurred_light_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glBindImageTexture(1, blurred_light_texture_second, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		glDispatchCompute(game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, game::CHUNK_SIZE * game::RENDERED_CHUNKS_WIDTH, 1);
		glFinish();

		// glBindImageTexture(0, blurred_light_texture_second, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		// glBindImageTexture(1, blurred_light_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		// glDispatchCompute(game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH, 1);
		// glFinish();

		// glBindImageTexture(0, blurred_light_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		// glBindImageTexture(1, blurred_light_texture_second, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		// glDispatchCompute(game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH, 1);
		// glFinish();

		glUseProgram(0);

		// Update the engine
		// glFinish();
		render_sys->update();
		// game_engine::rendering_mutex.lock();

		// legged_creature_sys -> post_fx_update();

		glUseProgram(game_engine::shader_programs[1]);
		GLuint projection_location = glGetUniformLocation(game_engine::shader_programs[1], "projection");
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, game_engine::projection_matrix);
		GLuint view_location = glGetUniformLocation(game_engine::shader_programs[1], "view");
		glUniformMatrix4fv(view_location, 1, GL_FALSE, game_engine::view_matrix);
		// for (std::vector<std::vector<std::pair<float, float>>> &chunk_outline : chunk_outlines)
		// {
		// 	// printf("rendering outlines for a chunk\n");
		// 	for (std::vector<std::pair<float, float>> &outline : chunk_outline)
		// 	{
		// 		for (int i = 0; i < outline.size() - 2; i += 3)
		// 		{
		// 			// printf("Line: %d\n", i);
		// 			std::pair<float, float> p1 = outline[i];
		// 			std::pair<float, float> p2 = outline[i + 1];
		// 			std::pair<float, float> p3 = outline[i + 2];
		// 			render_sys->draw_line(p1.first, p1.second, -3.0f, p2.first, p2.second, -3.0f);
		// 			render_sys->draw_line(p2.first, p2.second, -3.0f, p3.first, p3.second, -3.0f);
		// 			render_sys->draw_line(p3.first, p3.second, -3.0f, p1.first, p1.second, -3.0f);
		// 		}
		// 		// for (int i = 0; i < outline.size() - 1; i += 1)
		// 		// {
		// 		//     // Non-triangulated outlines
		// 		//     std::pair<float, float> p1 = outline[i];
		// 		//     std::pair<float, float> p2 = outline[i + 1];
		// 		//     game_engine::draw_line(p1.first, p1.second, -2.0f, p2.first, p2.second, -2.0f);
		// 		// }
		// 		// game_engine::draw_line(outline[outline.size() - 1].first, outline[outline.size() - 1].second, -2.0f, outline[0].first, outline[0].second, -2.0f);
		// 	}
		// }

		glFinish();

		glfwSwapBuffers(window);
		// game_engine::rendering_mutex.unlock();
		light_texture_index++;
		if (light_texture_index == light_texture_count)
			light_texture_index = 0;

		// get player velocity
		player_vel = player_body->GetLinearVelocity();

		auto end_rendering_loop = std::chrono::high_resolution_clock::now();

		frame_c++;
		// if more than one second
		if (std::chrono::duration_cast<std::chrono::milliseconds>(end_rendering_loop - last_fps_start) >= std::chrono::milliseconds(1000))
		{

			printf("fps: %d\n", frame_c);
			task_sc.print_task_counter(0);

			frame_c = 0;
			last_fps_start = end_rendering_loop;

			printf("System name\tUpdates\tAvg time (ms)\n");
			// system_names
			for (const auto &[id, name] : system_names)
			{
				game_engine::system *sys = eng.get_system(id);

				uint64_t updates = sys->get_counter();
				printf("%s\t%llu\t%llu\n", name.c_str(), updates, updates ? sys->get_total_time_millis() / updates : 0);
				sys->reset_counter();
			}
		}

		auto loop_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_rendering_loop - start_rendering_loop);

		last_time_taken_micro = loop_duration.count();
		counter++;
	}

	flying_creature_sys->set_running(false);
	flying_creature_thread.join();

	tile_conveyor_sys->set_running(false);
	tile_conveyor_thread.join();

	legged_creature_sys->set_running(false);
	legged_creature_thread.join();

	tile_arcing_sys->running = false;
	// tile_arcing_thread.join();

	physics_loop_running = false;
	box2d_sys->stop_thread();
	box_2d_thread.join();
	tile_pathfinding_sys->set_running(false);
	tile_pathfinding_thread.join();
	world_thread.join();
	tree_sys->set_running(false);
	tree_thread.join();

	delete projectile_sys;
	delete box2d_sys;
	delete world_sys;
	delete texture_vbo_sys;
	delete render_sys;
	delete box_sys;
	delete tree_sys;

	task_sc.shutdown({&game::shutdown_task_schedular_task, 0});
	task_runner.join();
}

void error_callback(int error, const char *description)
{
	fprintf(stderr, "Error %d: %s", error, description);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		physics_loop_running = false;
	}

	game_engine::game_engine_pointer->key_callback(window, key, scancode, action, mods);
	// game_engine::game_engine_pointer -> key_callback(window, key, scancode, action, mods);
	// if (key == GLFW_KEY_W && action == GLFW_PRESS) {
	//     printf("W pressed\n");
	// }
}

int main()
{
	printf("Hello World!\n");

	timeBeginPeriod(1);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Init OpenGL stuff: glfw, glew,
	GLuint err = glfwInit();
	if (err != GL_TRUE)
	{
		printf("Failed to init GLFW\n");
		return 1;
	}

	// // Create window
	GLFWwindow *window = glfwCreateWindow(game_engine::window_width, game_engine::window_height, "Game", NULL, NULL);
	// Create window
	// GLFWwindow *window = glfwCreateWindow(game_engine::window_width, game_engine::window_height, "Game", glfwGetPrimaryMonitor(), NULL);

	if (window == NULL)
	{
		printf("Failed to create window\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	printf("Error_0: %d\n", glGetError());

	// glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, game_engine::window_width, game_engine::window_height, GLFW_DONT_CARE);

	glfwSwapInterval(0);

	// Init glew
	err = glewInit();
	if (err != GLEW_OK)
	{
		printf("Failed to init GLEW\n");
		return 1;
	}
	printf("Error_0: %d\n", glGetError());

	GLint currnet_matrix_mode;
	glGetIntegerv(GL_MATRIX_MODE, &currnet_matrix_mode);
	printf("Error_1.25: x%d\n", glGetError());
	if (currnet_matrix_mode == GL_PROJECTION)
	{
		printf("Error: already matrix\n");
	}
	printf("Error_0: %d\n", glGetError());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, game_engine::window_width, game_engine::window_height, 0.0f, 0.0f, 100.0f);
	glGetFloatv(GL_PROJECTION_MATRIX, game_engine::projection_matrix);
	glGetFloatv(GL_PROJECTION_MATRIX, game_engine::projection_matrix_ortho);
	// Print matrix
	printf("Projection Matrix:\n");
	for (int i = 0; i < 16; i++)
	{
		printf("%f ", game_engine::projection_matrix[i]);
		if (i % 4 == 3)
		{
			printf("\n");
		}
	}
	printf("\n");

	game_engine::projection_matrix[5] *= PIXEL_SCALE;
	game_engine::projection_matrix_ortho[5] *= PIXEL_SCALE;
	game_engine::projection_matrix[0] *= PIXEL_SCALE;
	game_engine::projection_matrix_ortho[0] *= PIXEL_SCALE;
	// game_engine::projection_matrix[10] = -1.f;
	// game_engine::projection_matrix[15] = 1.f;
	// game_engine::projection_matrix[5] *= 2.0f;
	// game_engine::projection_matrix[10] *= 2.0f;
	// game_engine::projection_matrix[12] *= 2.0f;
	// game_engine::projection_matrix[13] *= 2.0f;
	// game_engine::projection_matrix[14] *= 2.0f;
	// game_engine::projection_matrix[15] *= 2.0f;
	printf("Projection Matrix:\n");
	for (int i = 0; i < 16; i++)
	{
		printf("%f ", game_engine::projection_matrix[i]);
		if (i % 4 == 3)
		{
			printf("\n");
		}
	}
	printf("\n");

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, game_engine::window_width, game_engine::window_height);
	printf("Error_0: %d\n", glGetError());

	glfwSetErrorCallback(error_callback);
	// glfwSetKeyCallback(window, key_callback);
	// do_tests();
	// return 0;
	printf("Error_1: %d\n", glGetError());

	// Run the game
	run_game(window);
	printf("Error_0: %d\n", glGetError());

	printf("Program exiting\n");

	return 0;
}
// 0.001042 0.000000 0.000000 0.000000 0.000000 -0.001852 0.000000 0.000000 0.000000 0.000000 -0.020000 0.000000 -1.000000 1.000000 -1.000000 1.000000
// 0.008333 0.000000 0.000000 0.000000 0.000000 0.014815 0.000000 0.000000 0.000000 0.000000 -1.000000 0.000000 -8.000000 -8.000000 0.000000 1.000000