#include <stdio.h>
#include <GL/glew.h>
#include <gl\GL.h>
#include <GLFW/glfw3.h>

#include "engine_comp.hpp"
#include "game_core.hpp"

void run_game(GLFWwindow * window) {
    // Create the engine and systems
    game_engine::engine eng;
    
    game_engine::box_system * box_sys = new game_engine::box_system();
    eng.add_system(game_engine::family::type<game_engine::box_system>(), box_sys);
    
    game_engine::render_system * render_sys = new game_engine::render_system(window);
    eng.add_system(game_engine::family::type<game_engine::render_system>(), render_sys);

    game::world_tile_system * world_sys = new game::world_tile_system();
    eng.add_system(game_engine::family::type<game::world_tile_system>(), world_sys);
 
    world_sys->generate_world();

    game_engine::texture background_texture;
    game_engine::texture foreground_texture;
    game_engine::texture light_texture;

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
