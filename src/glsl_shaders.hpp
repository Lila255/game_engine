#pragma once
// Store strings for GLSL shaders and load into programs
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <gl\GL.h>
#include <GLFW/glfw3.h>

namespace glsl_helper
{

    std::string frag_0()
    {
        std::stringstream frag_0;
        frag_0 << "#version 330\n"
               << "in vec2 v_TexCoord;\n"
               << "uniform sampler2D tex;\n"
               << "out vec4 out_Color;\n"
               << "void main()\n"
               << "{\n"
               << "float intensity = texture(tex, v_TexCoord).r;\n"
               << "if(intensity == 0.0) {\n"
               << "out_Color = vec4(0.0, 0.0, 0.0, 0.0);\n"
               << "} else if(intensity == 1.0 / 255) {\n"
               << "out_Color = vec4(0.4118, 0.7333, 0.0039, 1.0);\n"
               << "} else if(intensity == 2.0 / 255) {\n"
               << "out_Color = vec4(0.75, 0.8, 0.1, 1.0);\n"
               << "} else if(intensity == 100.0 / 255) {\n"
               << "out_Color = vec4(0.9, 0.7, 0.4, 1.0);\n"
               << "} else if(intensity == 101.0 / 255) {\n"
               << "out_Color = vec4(0.19, 0.09, 0.197, 1.0);\n"
               << "} else {\n"
               << "out_Color = vec4(1.0, 0.41, 0.71, 1.0);\n"
               << "}\n"
               //    << "gl_FragDepth = gl_FragCoord.z;\n"
               << "}\n";
        return frag_0.str();
    }
    // #version 330

    // layout (location = 0) in vec3 in_Position;
    // layout (location = 1) in vec2 in_TexCoord;

    // out vec2 v_TexCoord;

    // void main()
    // {
    //     v_TexCoord = in_TexCoord;
    //     gl_Position = vec4(in_Position, 1.0);
    // }
    std::string vert_0()
    {
        std::stringstream vert_0;
        vert_0
            << "#version 330\n"
            << "layout (location = 0) in vec3 in_Position;\n"
            << "layout (location = 1) in vec2 in_TexCoord;\n"
            << "uniform mat4 projection;\n"
            << "uniform mat4 view;\n"
            << "out vec2 v_TexCoord;\n"
            << "void main()\n"
            << "{\n"
            << "v_TexCoord = in_TexCoord;\n"
            << "gl_Position = projection * view * vec4(in_Position, 1.0);\n"
            // << "gl_Position = vec4(in_Position, 1.0);\n"
            // << "gl_Position = projection * vec4(in_Position, 1.0);\n"
            << "}\n";
        return vert_0.str();
    }

    GLuint load_shader(const std::string &source, GLenum type)
    {
        GLuint shader = glCreateShader(type);
        const char *src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            std::printf("Failed to compile shader %d\n", glGetError());
            return 0;
        }
        return shader;
    }

    void create_character_texture(GLuint &texture)
    {
        // Create a texture for the character
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // // Set texture filtering
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Set texture data
        // std::array<std::array<uint8_t, 3>, 8> data = {
        //     std::array<uint8_t, 3>{101, 100, 101},
        //     std::array<uint8_t, 3>{0, 100, 0},
        //     std::array<uint8_t, 3>{100, 100, 0},
        //     std::array<uint8_t, 3>{100, 100, 100},
        //     std::array<uint8_t, 3>{100, 0, 100},
        //     std::array<uint8_t, 3>{100, 100, 100},
        //     std::array<uint8_t, 3>{0, 100, 101},
        //     std::array<uint8_t, 3>{100, 101, 100},
        // };
        // std::array<std::array<uint8_t, 8>, 3> data = {
        //     std::array<uint8_t, 8>{101, 100, 101, 0, 100, 0, 100, 100},
        //     std::array<uint8_t, 8>{100, 100, 100, 100, 0, 100, 100, 100},
        //     std::array<uint8_t, 8>{100, 101, 100, 0, 100, 101, 100, 101},
        // };
        std::array<uint8_t, 24>  data = {
            101, 100, 101, 0, 100, 0, 100, 100,
            100, 100, 100, 100, 0, 100, 100, 100,
            100, 101, 100, 0, 100, 101, 100, 101,
        };
        
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 3, 8, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 3, 8, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
    }

};

std::vector<GLuint> load_shaders()
{
    std::vector<GLuint> programs;
    uint16_t shader_number = 1;

    // programs.push_back(glsl_helper::load_shader(glsl_helper::vert_0.str(), GL_VERTEX_SHADER));
    for (int i = 0; i < shader_number; i++)
    {
        GLuint vert_shader = glsl_helper::load_shader(glsl_helper::vert_0(), GL_VERTEX_SHADER);
        GLuint frag_shader = glsl_helper::load_shader(glsl_helper::frag_0(), GL_FRAGMENT_SHADER);

        GLuint program = glCreateProgram();
        if (program == 0)
        {
            std::printf("Failed to create shader program %d with error %d\n", i, glGetError());
        }
        glAttachShader(program, vert_shader);
        printf("Error_0: %d\n", glGetError());
        glAttachShader(program, frag_shader);
        printf("Error_1: %d\n", glGetError());
        glLinkProgram(program);
        printf("Error_2: %d\n", glGetError());
        programs.push_back(program);

        GLint linkStatus;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus == GL_FALSE)
        {
            printf("Failed to link shader program\n");
            GLint logLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
            char *log = (char *)malloc(logLength);
            glGetProgramInfoLog(program, logLength, NULL, log);
            printf("Log: %s\n", log);
            free(log);
            // return -1;
        }
        else
        {
            printf("Shader program %d linked\n", i);
        }

        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);

        if (glGetError() != GL_NO_ERROR)
        {
            std::printf("Failed to load shader %d with error %d\n", i, glGetError());
        }
        // programs.push_back(glsl_helper::load_shader(glsl_helper::vert_0(), GL_VERTEX_SHADER));
        // programs.push_back(glsl_helper::load_shader(glsl_helper::frag_0(), GL_FRAGMENT_SHADER));
    }
    return programs;
}