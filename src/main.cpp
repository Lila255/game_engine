#include <stdio.h>
#include <assert.h>

#include "engine_comp.hpp"
#include "game_core.hpp"

static std::vector<std::vector<std::vector<std::pair<float, float>>>> chunk_outlines;

void custom_key_callback(std::unordered_set<int> &keys)
{
	if (keys.find(GLFW_KEY_ESCAPE) != keys.end())
	{

		glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
	}
	// w
	if (keys.count(GLFW_KEY_W) > 0)
	{
		game::box2d_system *b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::box2d_system>()));
		entity player = game_engine::game_engine_pointer->player_entitiy;
		b2Body *body = b2d_sys->get_dynamic_body(player);
		// if body is touching something
		if (body->GetContactList() != NULL)
		{
			for (b2ContactEdge *ce = body->GetContactList(); ce != NULL; ce = ce->next)
			{
				if (!ce->contact->IsTouching())
					continue;
				printf("normal: %f, %f\n", ce->contact->GetManifold()->localNormal.x, ce->contact->GetManifold()->localNormal.y);
				// if the normal is pointing up
				if (ce->contact->GetManifold()->localNormal.y <= 0)
				{
					b2Vec2 impulse = b2Vec2(0.0f, -1000.1f);
					body->ApplyLinearImpulseToCenter(impulse, true);
					// apply force
					// keys.erase(GLFW_KEY_W);
					// body->ApplyForceToCenter(impulse, true);
					break;
				}
			}
			// b2Vec2 impulse = b2Vec2(0.0f, -1000.1f);
			// body->ApplyLinearImpulseToCenter(impulse, true);
		}
		// b2Vec2 impulse = b2Vec2(0.0f, -100.1f);
		// body->ApplyLinearImpulseToCenter(impulse, true);
	}
	// a
	if (keys.count(GLFW_KEY_A) > 0)
	{
		game::box2d_system *b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::box2d_system>()));
		entity player = game_engine::game_engine_pointer->player_entitiy;
		b2Body *body = b2d_sys->get_dynamic_body(player);
		b2Vec2 impulse = b2Vec2(-15.1f, 0.0f);
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
		body->ApplyAngularImpulse(50.f, true);
	}

	// d
	if (keys.count(GLFW_KEY_D) > 0)
	{
		game::box2d_system *b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::box2d_system>()));
		entity player = game_engine::game_engine_pointer->player_entitiy;
		b2Body *body = b2d_sys->get_dynamic_body(player);
		b2Vec2 impulse = b2Vec2(15.1f, 0.0f);
		body->ApplyLinearImpulseToCenter(impulse, true);
	}

	// shift
	if (keys.count(GLFW_KEY_LEFT_SHIFT) > 0)
	{
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
		game::box2d_system *b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::box2d_system>()));
		entity player = game_engine::game_engine_pointer->player_entitiy;
		b2Body *body = b2d_sys->get_dynamic_body(player);
		b2Vec2 impulse = b2Vec2(direction * 3.f, 0.f);
		body->ApplyLinearImpulseToCenter(impulse, true);
	}
}

void custom_mouse_callback(GLFWwindow *window, std::unordered_set<int> &buttons)
{
	// if mouse click
	if (buttons.count(GLFW_MOUSE_BUTTON_LEFT) > 0)
	{
		// get mouse position
		double xpos, ypos = -1;
		glfwGetCursorPos(window, &xpos, &ypos);
		printf("Cursor pos: x: %f, y: %f\n", xpos, ypos);

		// translate to world coordinates
		// int width, height;
		// glfwGetWindowSize(window, &width, &height);

		// get character position
		game::box2d_system *b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::box2d_system>()));
		entity player = game_engine::game_engine_pointer->player_entitiy;
		b2Body *body = b2d_sys->get_dynamic_body(player);
		b2Vec2 player_pos = body->GetPosition();

		// get mouse position
		int world_x = (int)((1.0 * xpos / PIXEL_SCALE) - 0.5 * (1.0 * game_engine::window_width / PIXEL_SCALE) + player_pos.x);
		int world_y = (int)((1.0 * ypos / PIXEL_SCALE) - 0.5 * (1.0 * game_engine::window_height / PIXEL_SCALE) + player_pos.y);
		printf("x: %d, y: %d\n", world_x, world_y);

		// get world tile system
		game::world_tile_system *world_sys = (game::world_tile_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::world_tile_system>()));
		// std::array<game::chunk *, game::NUM_CHUNKS> *chunks = world_sys->get_chunks();
		// // for(game::chunk * c : chunks)

		// for (int i = 0; i < game::NUM_CHUNKS; i++)
		// {
		//     chunks->at(i) -> delete_circle(world_x, world_y, 8);
		//     entity e = world_sys->get_chunk_entity(i);
		// }
		world_sys->delete_circle(world_x, world_y, 8, &chunk_outlines);

		buttons.erase(GLFW_MOUSE_BUTTON_LEFT);
	}
}

void run_game(GLFWwindow *window)
{
	// Create the engine and systems
	game_engine::engine eng;
	game_engine::shader_programs = load_shaders(glsl_helper::vert_0(), glsl_helper::frag_0());

	// generic shader
	game_engine::shader_programs.push_back(load_shaders(glsl_helper::vert_1(), glsl_helper::frag_1())[0]);

	// generic texture shader
	game_engine::shader_programs.push_back(load_shaders(glsl_helper::vert_2(), glsl_helper::frag_2())[0]);

	GLuint compute_shader = load_compute_shader(glsl_helper::light_compute_shader());

	GLuint light_blurring_compute_shader = load_compute_shader(glsl_helper::light_blurring_compute_shader());

	game_engine::box_system *box_sys = new game_engine::box_system();
	eng.add_system(game_engine::family::type<game_engine::box_system>(), box_sys);

	game_engine::render_system *render_sys = new game_engine::render_system(window);
	eng.add_system(game_engine::family::type<game_engine::render_system>(), render_sys);
	render_sys->set_key_callback(custom_key_callback);
	render_sys->set_mouse_callback(custom_mouse_callback);

	game_engine::texture_vbo_system *texture_vbo_sys = new game_engine::texture_vbo_system();
	eng.add_system(game_engine::family::type<game_engine::texture_vbo_system>(), texture_vbo_sys);

	game::world_tile_system *world_sys = new game::world_tile_system();
	eng.add_system(game_engine::family::type<game::world_tile_system>(), world_sys);

	game::box2d_system *box2d_sys = new game::box2d_system();
	eng.add_system(game_engine::family::type<game::box2d_system>(), box2d_sys);

	world_sys->generate_world();
	// std::array<GLuint, game::NUM_CHUNKS> chunk_textures = world_sys->create_chunk_textures();
	std::array<std::array<std::array<uint8_t, game::CHUNK_SIZE>, game::CHUNK_SIZE> *, game::NUM_CHUNKS> chunks_data = world_sys->get_chunks_data();

	GLuint background_texture;
	glGenTextures(1, &background_texture);
	glBindTexture(GL_TEXTURE_2D, background_texture);
	// set data and size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	std::array<uint8_t, game::CHUNK_SIZE * game::CHUNKS_WIDTH * game::CHUNK_SIZE * game::CHUNKS_WIDTH> *background_data = new std::array<uint8_t, game::CHUNK_SIZE * game::CHUNKS_WIDTH * game::CHUNK_SIZE * game::CHUNKS_WIDTH>();
	for(int y = 0; y < game::CHUNK_SIZE * game::CHUNKS_WIDTH; y++)
	{
		for(int x = 0; x < game::CHUNK_SIZE * game::CHUNKS_WIDTH; x++)
		{
			if((x % 8) == 0 || (y % 3) == 0) {
				(*background_data)[y * game::CHUNK_SIZE * game::CHUNKS_WIDTH + x] = 102;
			} else {

				(*background_data)[y * game::CHUNK_SIZE * game::CHUNKS_WIDTH + x] = 103;
			}
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH, 0, GL_RED, GL_UNSIGNED_BYTE, background_data->data());
	delete background_data;
	
	entity background_entity = eng.create_entity();
	box_sys->add(background_entity, {0.f, 0.f, -6.0, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH});
	texture_vbo_sys->add(background_entity);
	game_engine::sprite sprt(game_engine::shader_programs[0]);
	sprt.add_texture({background_texture, 0, GL_R8, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH});
	render_sys->add(background_entity, sprt);
// (GLuint id, GLuint binding, GLuint format, uint32_t width, uint32_t height)

	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint chunk_texture;
	glGenTextures(1, &chunk_texture);
	glBindTexture(GL_TEXTURE_2D, chunk_texture);
	// set data and size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	printf("before settingh chunk textures: %d\n", glGetError());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	printf("before settingh chunk textures: %d\n", glGetError());

	for (int i = 0; i < game::NUM_CHUNKS; i++)
	{
		int x = i % game::CHUNKS_WIDTH;
		int y = i / game::CHUNKS_WIDTH;

		glTexSubImage2D(GL_TEXTURE_2D, 0, x * game::CHUNK_SIZE, y * game::CHUNK_SIZE, game::CHUNK_SIZE, game::CHUNK_SIZE, GL_RED, GL_UNSIGNED_BYTE, chunks_data[i]->data());
	}
	printf("after settingh chunk textures: %d\n", glGetError());

	glBindTexture(GL_TEXTURE_2D, 0);

	entity all_chunks_entity = eng.create_entity();
	box_sys->add(all_chunks_entity, {0.f, 0.f, -5.0, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH});
	texture_vbo_sys->add(all_chunks_entity);
	sprt = game_engine::sprite(game_engine::shader_programs[0]);
	sprt.add_texture({chunk_texture, 0, GL_R8, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH});
	render_sys->add(all_chunks_entity, sprt);

	world_sys->set_all_chunk_ent(all_chunks_entity);

	// for(int i = 0; i < game::NUM_CHUNKS; i++) {
	for (int y = 0; y < game::CHUNKS_WIDTH; y++)
	{
		for (int x = 0; x < game::CHUNKS_WIDTH; x++)
		{
			// if (x != 0 || y != 0)
			// {
			entity chunk_entity = world_sys->get_chunk_entity(x, y);
			std::vector<std::vector<std::pair<float, float>>> outlines = world_sys->create_outlines(x, y);
			box2d_sys->create_static_bodies(chunk_entity, outlines);
			chunk_outlines.push_back(outlines);
			// chunk_outlines.push_back(world_sys->create_outlines(x, y));
			float top_left_x = game::CHUNK_SIZE * x * 1.0;
			float top_left_y = game::CHUNK_SIZE * y * 1.0;
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

	// create player
	entity player_entity = eng.create_entity();
	GLuint player_texture;
	glsl_helper::create_character_texture(player_texture);
	sprt = game_engine::sprite(game_engine::shader_programs[0]);
	sprt.add_texture({player_texture, 0, GL_R8, glsl_helper::character_width, glsl_helper::character_height});
	render_sys->add(player_entity, sprt);
	// box_sys->add(player_entity, {0.f, 0.f, -1.f, 4 * 8.f, 1 * 85.333333333f});
	box_sys->add(player_entity, {0.f, 0.f, -4.5f, glsl_helper::character_width, glsl_helper::character_height});
	texture_vbo_sys->add(player_entity);
	eng.player_entitiy = player_entity;

	// // create box2d body
	// std::vector<std::pair<float, float>> player_shape = {
	// 	{0.f, 0.f},
	// 	{glsl_helper::character_width, glsl_helper::character_height},
	// 	{glsl_helper::character_width, 0.f},
	// 	{glsl_helper::character_width, glsl_helper::character_height},
	// 	{0.f, 0.f},
	// 	{0.f, glsl_helper::character_height}};
	std::vector<std::pair<float, float>> player_shape = {
		{0.f, 0.f},
		{glsl_helper::character_width, glsl_helper::character_height},
		{glsl_helper::character_width, 0.f},
		{glsl_helper::character_width, glsl_helper::character_height},
		{0.f, 0.f},
		{0.f, glsl_helper::character_height}};

	chunk_outlines.push_back({player_shape});
	box2d_sys->create_dynamic_body(player_entity, player_shape);
	b2Body *player_body = box2d_sys->get_dynamic_body(player_entity);

	// create light components
	uint16_t light_texture_count = 32;
	std::vector<entity> light_entities;
	std::vector<GLuint> light_textures;
	// std::vector<GLuint> colour_textures;

	std::vector<uint32_t> light_data(game::CHUNK_SIZE * game::CHUNKS_WIDTH * game::CHUNK_SIZE * game::CHUNKS_WIDTH * 2, 0);

	for (int i = 0; i < light_texture_count + 1; i++)
	{
		GLuint light_texture;
		glGenTextures(1, &light_texture);
		glBindTexture(GL_TEXTURE_2D, light_texture);
		// set data and size
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, light_data.data());
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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint blurred_light_texture_second;
	glGenTextures(1, &blurred_light_texture_second);
	glBindTexture(GL_TEXTURE_2D, blurred_light_texture_second);
	// set data and size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	entity mater_light_entity = eng.create_entity();

	box_sys->add(mater_light_entity, {0.f, 0.f, -4.5f, game::CHUNK_SIZE * 3.0, game::CHUNK_SIZE * 3.0});
	texture_vbo_sys->add(mater_light_entity);
	sprt = game_engine::sprite(game_engine::shader_programs[2]);
	sprt.add_texture({master_light_texture, 0, GL_R32UI, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH});
	sprt.add_texture({blurred_light_texture, 1, GL_R32UI, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH});
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

	uint16_t saved_light_textures = 0;
	uint16_t light_texture_index = 0;
	uint64_t last_time_taken = 0;
	uint64_t counter = 0;
	// Run the game loop
	while (!glfwWindowShouldClose(window))
	{
		// printf("start_of_loop: %d\n", glGetError());

		uint64_t start_time = glfwGetTimerValue();
		// draw lines for chunk outlines
		box2d_sys->update(last_time_taken);
		world_sys->update(counter);
		// update the player's outlines
		// std::vector<std::pair<float, float>> player_outline;
		// b2Fixture *fixture = player_body->GetFixtureList();
		// while (fixture != NULL)
		// {
		// 	b2PolygonShape *shape = (b2PolygonShape *)fixture->GetShape();
		// 	for (int i = 0; i < shape->m_count; i++)
		// 	{
		// 		b2Vec2 vertex = player_body->GetWorldPoint(shape->m_vertices[i]);
		// 		// b2Vec2 vertex = shape->m_vertices[i] + player_body->GetPosition() - b2Vec2(glsl_helper::character_width / 2.0f, glsl_helper::character_height / 2.0f);
		// 		player_outline.push_back({vertex.x, vertex.y});
		// 	}
		// 	fixture = fixture->GetNext();
		// }
		// chunk_outlines[game::NUM_CHUNKS] = {player_outline};

		game_engine::box &b = box_sys->get(player_entity);
		game_engine::view_matrix[12] = -b.x - 0.5 * glsl_helper::character_width + game_engine::window_width * (1.0 / (2 * PIXEL_SCALE));
		game_engine::view_matrix[13] = -b.y - 0.5 * glsl_helper::character_height + game_engine::window_height * (1.0 / (2 * PIXEL_SCALE));

		// printf("before_update_call: %d\n", glGetError());


		// printf("after_update_call: %d\n", glGetError());
		// printf("Rendering\n");
		// glUseProgram(0);
		// glUseProgram(game_engine::shader_programs[1]);
		// GLuint projection_location = glGetUniformLocation(game_engine::shader_programs[1], "projection");
		// glUniformMatrix4fv(projection_location, 1, GL_FALSE, game_engine::projection_matrix);
		// GLuint view_location = glGetUniformLocation(game_engine::shader_programs[1], "view");
		// glUniformMatrix4fv(view_location, 1, GL_FALSE, game_engine::view_matrix);
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

		// printf("before_clearing_l_texture: %d\n", glGetError());
		// glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, light_textures[light_texture_index]);
		glClearTexImage(light_textures[light_texture_index], 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
		glBindTexture(GL_TEXTURE_2D, blurred_light_texture);
		glClearTexImage(blurred_light_texture, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		// printf("after_clearing_l_texture: %d\n", glGetError());

		// trace lights with compute shader
		glUseProgram(compute_shader);
		// bind world textures
		// printf("before_binding_textures: %d\n", glGetError());
		glBindImageTexture(0, light_textures[light_texture_index], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glBindImageTexture(1, chunk_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
		GLuint texture_size = glGetUniformLocation(compute_shader, "texture_size");
		glUniform2i(texture_size, game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH);

		GLint player_pos = glGetUniformLocation(compute_shader, "player_pos");
		glUniform2f(player_pos, (float)(player_body->GetPosition().x + glsl_helper::character_width / 2.0), (float)(player_body->GetPosition().y + glsl_helper::character_width / 2.0));

		// GLint vector_location = glGetUniformLocation(compute_shader, "normal_vectors");
		// glUniform2fv(vector_location, 256, (GLfloat*)game::noramal_vectors.data());
		glBindImageTexture(2, normal_vectors_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
		// printf("before_binding_col_texture: %d\n", glGetError());
		// glBindImageTexture(3, colour_textures[light_texture_index], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		// printf("after_binding_col_texture: %d\n", glGetError());

		glDispatchCompute(720, 1, 1);
		// printf("after dispatch: %d\n", glGetError());
		glFinish();

		glUseProgram(blend_shader);
		glBindImageTexture(0, master_light_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glBindImageTexture(1, light_textures[light_texture_index], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		glBindImageTexture(2, light_textures[(light_texture_index + 1) % light_texture_count], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);

		
		glDispatchCompute(game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH, 1);
		
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
		glDispatchCompute(game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH, 1);
		glFinish();

		glBindImageTexture(0, blurred_light_texture_second, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glBindImageTexture(1, blurred_light_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		glDispatchCompute(game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH, 1);
		glFinish();

		glBindImageTexture(0, blurred_light_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glBindImageTexture(1, blurred_light_texture_second, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		glDispatchCompute(game::CHUNK_SIZE * game::CHUNKS_WIDTH, game::CHUNK_SIZE * game::CHUNKS_WIDTH, 1);
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
		glFinish();

		glfwSwapBuffers(window);
		light_texture_index++;
		if (light_texture_index == light_texture_count)
			light_texture_index = 0;

		last_time_taken = glfwGetTimerValue() - start_time;
		counter++;
	}
	// Learn how to do audio
	// Learn how to do chunk switching
}

void error_callback(int error, const char *description)
{
	fprintf(stderr, "Error %d: %s", error, description);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	game_engine::game_engine_pointer->key_callback(window, key, scancode, action, mods);
	// game_engine::game_engine_pointer -> key_callback(window, key, scancode, action, mods);
	// if (key == GLFW_KEY_W && action == GLFW_PRESS) {
	//     printf("W pressed\n");
	// }
}

int main()
{
	printf("Hello World!\n");

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

	// Create window
	GLFWwindow *window = glfwCreateWindow(game_engine::window_width, game_engine::window_height, "Game", NULL, NULL);
	if (window == NULL)
	{
		printf("Failed to create window\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	printf("Error_0: %d\n", glGetError());

	// glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, game_engine::window_width, game_engine::window_height, GLFW_DONT_CARE);

	// glfwSwapInterval(0);

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
	game_engine::projection_matrix[0] *= PIXEL_SCALE;
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