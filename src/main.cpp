#include <stdio.h>
#include <GL/glew.h>
#include <gl\GL.h>
#include <GLFW/glfw3.h>

#include "engine_comp.hpp"

void run_game(GLFWwindow * window) {
    // Create the engine and systems
    game_engine::engine eng;
    game_engine::box_system * box_sys = new game_engine::box_system();
    eng.add_system(box_sys);
    


    // Create the game world

    // Create the entities

    // Add components to entities

    // Run the game loop

    //    Move entities/do physics
    //    Raycast from player
    //    Create light texture
    //    Render: background -> entities -> lights -> UI
    //    Update UI

    //    Repeat
    
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
