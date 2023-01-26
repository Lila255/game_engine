#include <stdio.h>

#include "engine_comp.hpp"
#include "game_core.hpp"

void run_game(GLFWwindow * window) {
    // Create the engine and systems
    game_engine::engine eng{};
    game_engine::shader_programs = load_shaders();
    printf("Error_0: %d\n", glGetError());
    
    game_engine::box_system * box_sys = new game_engine::box_system();
    eng.add_system(game_engine::family::type<game_engine::box_system>(), box_sys);
    
    game_engine::render_system * render_sys = new game_engine::render_system(window);
    eng.add_system(game_engine::family::type<game_engine::render_system>(), render_sys);
    printf("Error_0: %d\n", glGetError());

    game_engine::texture_vbo_system * texture_vbo_sys = new game_engine::texture_vbo_system();
    eng.add_system(game_engine::family::type<game_engine::texture_vbo_system>(), texture_vbo_sys);
    printf("Error_0: %d\n", glGetError());

    game::world_tile_system * world_sys = new game::world_tile_system();
    eng.add_system(game_engine::family::type<game::world_tile_system>(), world_sys);
    printf("Error_0: %d\n", glGetError());

    world_sys->generate_world();
    printf("Error_0: %d\n", glGetError());
    std::array<GLuint, game::NUM_CHUNKS> chunk_textures = world_sys -> create_chunk_textures();
    // for(int i = 0; i < game::NUM_CHUNKS; i++) {
    for(int y = 0; y < game::CHUNKS_WIDTH; y++) {
        for(int x = 0; x < game::CHUNKS_WIDTH; x++) {
            entity chunk_entity = eng.create_entity();
            float top_left_x = game::CHUNK_SIZE * x * 1.0;
            float top_left_y = game::CHUNK_SIZE * y * 1.0;
            // float top_left_x = 2 * x * game::CHUNK_SIZE; 
            // float top_left_y = 2 * y * game::CHUNK_SIZE;

            box_sys->add(chunk_entity, {top_left_x * 2.0f, top_left_y * 2.0f, 0.0, game::CHUNK_SIZE * 2.0, game::CHUNK_SIZE * 2.0});
            // box_sys->add(chunk_entity, { top_left_x, top_left_y, 0.0, 1.0/3.0, 1.0/3.0});
            // box_sys->add(chunk_entity, { top_left_x, top_left_y, 1.0f, 1.0/3.0, 1.0/3.0});
            // box_sys->add(chunk_entity, { top_left_x, top_left_y, 1.0f, 8 * game::CHUNK_SIZE, 8 * game::CHUNK_SIZE});

            // box_sys->add(chunk_entity, { top_left_x, top_left_y, 0.50f, 8 * game::CHUNK_SIZE, 8 * game::CHUNK_SIZE});
            texture_vbo_sys->add(chunk_entity);
            render_sys->add(chunk_entity, {chunk_textures[y * game::CHUNKS_WIDTH + x]});
        }
    }
    printf("Error_0: %d\n", glGetError());

    
    // Run the game loop
    while(!glfwWindowShouldClose(window)) {

    //    Move entities/do physics
    //    Raycast from player
    //    Create light texture
    //    Render: background -> entities -> lights -> UI
    //    Update UI(?)
    
        render_sys->update();
    }
    
    // Learn how to do audio
    // Learn how to do chunk switching

}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s", error, description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main()
{
    printf("Hello World!\n");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Init OpenGL stuff: glfw, glew,
    GLuint err = glfwInit();
    if (err != GL_TRUE) {
        printf("Failed to init GLFW\n");
        return 1;
    }
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(game_engine::window_width, game_engine::window_height, "Game", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    printf("Error_0: %d\n", glGetError());


    glfwSwapInterval( 0 );


    // Init glew
    err = glewInit();
    if (err != GLEW_OK) {
        printf("Failed to init GLEW\n");
        return 1;
    }
    printf("Error_0: %d\n", glGetError());

    GLint currnet_matrix_mode;
    glGetIntegerv(GL_MATRIX_MODE, &currnet_matrix_mode);
    printf("Error_1.25: x%d\n", glGetError());
    if(currnet_matrix_mode == GL_PROJECTION) {
        printf("Error: already matrix\n");
    }
    printf("Error_0: %d\n", glGetError());

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f);
    glOrtho(0.0f, 1920.0, 1080.0, 0.0f, 0.0f, 100.0f);
    glGetFloatv(GL_PROJECTION_MATRIX, game_engine::projection_matrix);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, 1920.0, 1080.0);
    printf("Error_0: %d\n", glGetError());



    glfwSetErrorCallback(error_callback);
    printf("Error_0: %d\n", glGetError());
    glfwSetKeyCallback(window, key_callback);
    printf("Error_0: %d\n", glGetError());

    // Run the game
    run_game(window);
    printf("Error_0: %d\n", glGetError());


    printf("Program exiting\n");
    return 0;
}
