#include <stdio.h>
#include <assert.h>

#include "engine_comp.hpp"
#include "game_core.hpp"

void run_game(GLFWwindow *window)
{
    // Create the engine and systems
    game_engine::engine eng{};
    game_engine::shader_programs = load_shaders(glsl_helper::vert_0(), glsl_helper::frag_0());
    printf("Error_0: %d\n", glGetError());

    game_engine::box_system *box_sys = new game_engine::box_system();
    eng.add_system(game_engine::family::type<game_engine::box_system>(), box_sys);

    game_engine::render_system *render_sys = new game_engine::render_system(window);
    eng.add_system(game_engine::family::type<game_engine::render_system>(), render_sys);
    printf("Error_0: %d\n", glGetError());

    game_engine::texture_vbo_system *texture_vbo_sys = new game_engine::texture_vbo_system();
    eng.add_system(game_engine::family::type<game_engine::texture_vbo_system>(), texture_vbo_sys);
    printf("Error_0: %d\n", glGetError());

    game::world_tile_system *world_sys = new game::world_tile_system();
    eng.add_system(game_engine::family::type<game::world_tile_system>(), world_sys);
    printf("Error_0: %d\n", glGetError());

    world_sys->generate_world();
    printf("Error_0: %d\n", glGetError());
    std::array<GLuint, game::NUM_CHUNKS> chunk_textures = world_sys->create_chunk_textures();
    // for(int i = 0; i < game::NUM_CHUNKS; i++) {
    std::vector<std::vector<std::vector<std::pair<float, float>>>> chunk_outlines;
    for (int y = 0; y < game::CHUNKS_WIDTH; y++)
    {
        for (int x = 0; x < game::CHUNKS_WIDTH; x++)
        {
            chunk_outlines.push_back(world_sys->create_outlines(x, y));
            entity chunk_entity = eng.create_entity();
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
        }
    }

    entity player_entity = eng.create_entity();
    GLuint player_texture;
    glsl_helper::create_character_texture(player_texture);
    render_sys->add(player_entity, {player_texture});
    // box_sys->add(player_entity, {0.f, 0.f, -1.f, 4 * 8.f, 1 * 85.333333333f});
    box_sys->add(player_entity, {8.f, 8.f, -1.f, glsl_helper::character_width, glsl_helper::character_height});
    texture_vbo_sys->add(player_entity);
    eng.player_entitiy = player_entity;
    printf("Error_0: %d\n", glGetError());

    // Run the game loop
    while (!glfwWindowShouldClose(window))
    {

        // draw lines for chunk outlines

        // Update the engine
        render_sys->update();
        uint16_t chunk_num = 0;
        uint64_t line_number = 0;
        // printf("Rendering\n");
        for (std::vector<std::vector<std::pair<float, float>>> &chunk_outline : chunk_outlines)
        {
            uint16_t chunk_x = chunk_num % game::CHUNKS_WIDTH;
            uint16_t chunk_y = chunk_num / game::CHUNKS_WIDTH;
            // printf("Chunk: %d, %d\n", chunk_x, chunk_y);
            for (std::vector<std::pair<float, float>> &outline : chunk_outline)
            {
                if(outline.size() == 0) continue;
                // if(outline.size() %2 != 0) continue;
                // printf("Outlining: %d\n", outline.size());
                for (int i = 0; i < outline.size() - 2; i+=3)
                {

                    // printf("Line: %d\n", i);
                    std::pair<float, float> p1 = outline[i];
                    std::pair<float, float> p2 = outline[i + 1];
                    std::pair<float, float> p3 = outline[i + 2];
                    game_engine::draw_line(p1.first, p1.second, -2.0f, p2.first, p2.second, -2.0f);
                    game_engine::draw_line(p2.first, p2.second, -2.0f, p3.first, p3.second, -2.0f);
                    game_engine::draw_line(p3.first, p3.second, -2.0f, p1.first, p1.second, -2.0f);

                    // printf("Line: %f %f %f %f\n", i);
                    // game_engine::draw_line(p1.first, p1.second, -2.0f, p2.first, p2.second, -2.0f);
                    // line_number++;
                    // printf("Line: %f %f %f %f\n", i);
                    // game_engine::draw_line(p1.first, p1.second, -2.0f, p2.first, p2.second, -2.0f);
                }
                // game_engine::draw_line(outline[outline.size() - 1].first, outline[outline.size() - 1].second, -2.0f, outline[0].first, outline[0].second, -2.0f);
                // break;
            }
            chunk_num++;
        }
        // printf("Number of lines: %d\n", line_number);
        glfwSwapBuffers(window);
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

    glfwSwapInterval( 1 );

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
    printf("Error_0: %d\n", glGetError());
    glfwSetKeyCallback(window, key_callback);
    printf("Error_0: %d\n", glGetError());


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