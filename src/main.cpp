#include <stdio.h>

#include "engine_comp.hpp"
#include "game_core.hpp"

void run_game(GLFWwindow * window) {
    // Create the engine and systems
    game_engine::engine eng{};
    game_engine::shader_programs = load_shaders();
    
    game_engine::box_system * box_sys = new game_engine::box_system();
    eng.add_system(game_engine::family::type<game_engine::box_system>(), box_sys);
    
    game_engine::render_system * render_sys = new game_engine::render_system(window);
    eng.add_system(game_engine::family::type<game_engine::render_system>(), render_sys);

    game_engine::texture_vbo_system * texture_vbo_sys = new game_engine::texture_vbo_system();
    eng.add_system(game_engine::family::type<game_engine::texture_vbo_system>(), texture_vbo_sys);

    game::world_tile_system * world_sys = new game::world_tile_system();
    eng.add_system(game_engine::family::type<game::world_tile_system>(), world_sys);
 
    world_sys->generate_world();
    std::array<GLuint, game::NUM_CHUNKS> chunk_textures = world_sys -> create_chunk_textures();
    // for(int i = 0; i < game::NUM_CHUNKS; i++) {
    for(int y = 0; y < game::NUM_CHUNKS; y++) {
        for(int x = 0; x < game::NUM_CHUNKS; x++) {
            entity chunk_entity = eng.create_entity();
            float top_left_x = 1 / 3.0f * x;
            float top_left_y = 1 / 3.0f * y; 

            box_sys->add(chunk_entity, { top_left_x, top_left_y, 0.0f, 1.0f / 3.0f, 1.0f / 3.0f});
            texture_vbo_sys->add(chunk_entity);
            render_sys->add(chunk_entity, {chunk_textures[y * game::NUM_CHUNKS + x]});
        }
    }


    // game_engine::create_texture_from_data(background_texture, 800, 600, 1);
    

    // Create the game world

    // Create the entities

    // Add components to entities

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

    // Init OpenGL stuff: glfw, glew,
    GLuint err = glfwInit();
    if (err != GL_TRUE) {
        printf("Failed to init GLFW\n");
        return 1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback(error_callback);

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Game", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    // Init glew
    err = glewInit();
    if (err != GLEW_OK) {
        printf("Failed to init GLEW\n");
        return 1;
    }

    // Run the game
    run_game(window);


    printf("Program exiting\n");
    return 0;
}
