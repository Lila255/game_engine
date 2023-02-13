#include <stdio.h>
#include <assert.h>

#include "engine_comp.hpp"
#include "game_core.hpp"

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
				// if the normal is pointing up
				if (ce->contact->GetManifold()->localNormal.y < 0)
				{
					b2Vec2 impulse = b2Vec2(0.0f, -5.1f);
					body->ApplyLinearImpulseToCenter(impulse, true);
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
		b2Vec2 impulse = b2Vec2(-2.1f, 0.0f);
		body->ApplyLinearImpulseToCenter(impulse, true);
	}
	// s
	if (keys.count(GLFW_KEY_S) > 0)
	{
		// if in water or something, idk
	}

	// d
	if (keys.count(GLFW_KEY_D) > 0)
	{
		game::box2d_system *b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::box2d_system>()));
		entity player = game_engine::game_engine_pointer->player_entitiy;
		b2Body *body = b2d_sys->get_dynamic_body(player);
		b2Vec2 impulse = b2Vec2(2.1f, 0.0f);
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
		int world_x = (int)((xpos / 8.0) - 0.5 * (game_engine::window_width / 8.0) + player_pos.x);
		int world_y = (int)((ypos / 8.0) - 0.5 * (game_engine::window_height / 8.0) + player_pos.y);
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
		world_sys->delete_circle(world_x, world_y, 8);

		buttons.erase(GLFW_MOUSE_BUTTON_LEFT);
	}
}

void run_game(GLFWwindow *window)
{
	// Create the engine and systems
	game_engine::engine eng;
	game_engine::shader_programs = load_shaders(glsl_helper::vert_0(), glsl_helper::frag_0());

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
	std::array<GLuint, game::NUM_CHUNKS> chunk_textures;
	for (int i = 0; i < game::NUM_CHUNKS; i++)
	{
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		// set data and size
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, game::CHUNK_SIZE, game::CHUNK_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, chunks_data[i]->data());
		glBindTexture(GL_TEXTURE_2D, 0);
		chunk_textures[i] = texture;
	}
	// for(int i = 0; i < game::NUM_CHUNKS; i++) {
	// std::vector<std::vector<std::vector<std::pair<float, float>>>> chunk_outlines;
	for (int y = 0; y < game::CHUNKS_WIDTH; y++)
	{
		for (int x = 0; x < game::CHUNKS_WIDTH; x++)
		{
			//if (x != 0 || y != 0)
		   // {
				entity chunk_entity = world_sys->get_chunk_entity(x, y);
				std::vector<std::vector<std::pair<float, float>>> outlines = world_sys->create_outlines(x, y);
				box2d_sys->create_static_bodies(chunk_entity, outlines);
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
				texture_vbo_sys->add(chunk_entity);
				render_sys->add(chunk_entity, {chunk_textures[y * game::CHUNKS_WIDTH + x]});
		   // }
		}
	}

	// for (std::vector<std::vector<std::pair<float, float>>> &chunk_outline : chunk_outlines)
	// {
	//     // for (std::vector<std::pair<float, float>> &outline : chunk_outline)
	//     // {
	//     //     entity outline_entity = eng.create_entity();
	//     //     box2d_sys->create_static_body(outline_entity, outline);
	//     // }
	// }

	// create player
	entity player_entity = eng.create_entity();
	GLuint player_texture;
	glsl_helper::create_character_texture(player_texture);
	render_sys->add(player_entity, {player_texture});
	// box_sys->add(player_entity, {0.f, 0.f, -1.f, 4 * 8.f, 1 * 85.333333333f});
	box_sys->add(player_entity, {8.f, 8.f, -1.f, glsl_helper::character_width, glsl_helper::character_height});
	texture_vbo_sys->add(player_entity);
	eng.player_entitiy = player_entity;
	// create box2d body
	std::vector<std::pair<float, float>> player_shape = {
		{0.f, 0.f},
		{glsl_helper::character_width, 0.f},
		{glsl_helper::character_width, glsl_helper::character_height},
		{0.f, glsl_helper::character_height},
		// {0.1f, 0.1f}
	};
	box2d_sys->create_dynamic_body(player_entity, player_shape);

	// generic shader
	GLuint generic_shader = load_shaders(glsl_helper::vert_1(), glsl_helper::frag_1())[0];
	glUseProgram(generic_shader);
	// GLuint projection_location = glGetUniformLocation(shader_programs[0], "projection");
	// glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection_matrix);
	// GLuint view_location = glGetUniformLocation(shader_programs[0], "view");
	// glUniformMatrix4fv(view_location, 1, GL_FALSE, view_matrix);
	// do this ^^ but with generic shader
	GLuint projection_location = glGetUniformLocation(generic_shader, "projection");
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, game_engine::projection_matrix);
	GLuint view_location = glGetUniformLocation(generic_shader, "view");
	glUniformMatrix4fv(view_location, 1, GL_FALSE, game_engine::view_matrix);
	
	uint64_t last_time_taken = 0;
	// Run the game loop
	while (!glfwWindowShouldClose(window))
	{
		uint64_t start_time = glfwGetTimerValue();
		// draw lines for chunk outlines
		box2d_sys->update(last_time_taken);
		// Update the engine
		render_sys->update();
		// printf("Rendering\n");
		// glUseProgram(0);
		glUseProgram(generic_shader);
		GLuint projection_location = glGetUniformLocation(generic_shader, "projection");
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, game_engine::projection_matrix);
		GLuint view_location = glGetUniformLocation(generic_shader, "view");
		glUniformMatrix4fv(view_location, 1, GL_FALSE, game_engine::view_matrix);
		// for (std::vector<std::vector<std::pair<float, float>>> &chunk_outline : chunk_outlines)
		// {
		//     // printf("rendering outlines for a chunk\n");
		//     for (std::vector<std::pair<float, float>> &outline : chunk_outline)
		//     {
		//         // printf("Rendering an outline\n");
		//         // for (int i = 0; i < outline.size() - 2; i+=3)
		//         // {
		//         //     // printf("Line: %d\n", i);
		//         //     std::pair<float, float> p1 = outline[i];
		//         //     std::pair<float, float> p2 = outline[i + 1];
		//         //     std::pair<float, float> p3 = outline[i + 2];
		//         //     game_engine::draw_line(p1.first, p1.second, -2.0f, p2.first, p2.second, -2.0f);
		//         //     game_engine::draw_line(p2.first, p2.second, -2.0f, p3.first, p3.second, -2.0f);
		//         //     game_engine::draw_line(p3.first, p3.second, -2.0f, p1.first, p1.second, -2.0f);
		//         // }

		//         for (int i = 0; i < outline.size() - 1; i += 1)
		//         {
		//             // Non-triangulated outlines
		//             std::pair<float, float> p1 = outline[i];
		//             std::pair<float, float> p2 = outline[i + 1];
		//             game_engine::draw_line(p1.first, p1.second, -2.0f, p2.first, p2.second, -2.0f);
		//         }
		//         game_engine::draw_line(outline[outline.size() - 1].first, outline[outline.size() - 1].second, -2.0f, outline[0].first, outline[0].second, -2.0f);
		//     }
		// }
		glUseProgram(0);
		glfwSwapBuffers(window);
		last_time_taken = glfwGetTimerValue() - start_time;
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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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

	game_engine::projection_matrix[0] *= 8.0f;
	game_engine::projection_matrix[5] *= 8.0f;
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

	// Run the game
	run_game(window);
	printf("Error_0: %d\n", glGetError());

	printf("Program exiting\n");

	return 0;
}
// 0.001042 0.000000 0.000000 0.000000 0.000000 -0.001852 0.000000 0.000000 0.000000 0.000000 -0.020000 0.000000 -1.000000 1.000000 -1.000000 1.000000
// 0.008333 0.000000 0.000000 0.000000 0.000000 0.014815 0.000000 0.000000 0.000000 0.000000 -1.000000 0.000000 -8.000000 -8.000000 0.000000 1.000000