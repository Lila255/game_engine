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
             << "uniform sampler2D u_Texture;\n"
             << "out vec4 out_Color;\n"
             << "void main()\n"
             << "{\n"
             << "float intensity = texture(u_Texture, v_TexCoord).r;\n"
             << "if(intensity == 0.0) {\n"
             << "out_Color = vec4(0.0, 0.0, 1.0, 1.0);\n"
             << "} else if(intensity == 1.0 / 255) {\n"
             << "out_Color = vec4(1.0, 0.5, 0.5, 1.0);\n"
             << "} else if(intensity == 2.0 / 255) {\n"
             << "out_Color = vec4(0.5, 0.5, 0.5, 1.0);\n"
             << "} else {\n"
             << "out_Color = vec4(1.0, 0.41, 0.71, 1.0);\n"
             << "}\n"
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
        vert_0 << "#version 330\n"
             << "layout (location = 0) in vec3 in_Position;\n"
             << "layout (location = 1) in vec2 in_TexCoord;\n"
             << "out vec2 v_TexCoord;\n"
             << "void main()\n"
             << "{\n"
             << "v_TexCoord = in_TexCoord;\n"
             << "gl_Position = vec4(in_Position, 1.0);\n"
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
};

std::vector<GLuint> load_shaders()
{
    std::vector<GLuint> programs;
    uint16_t shader_number = 1;

    // programs.push_back(glsl_helper::load_shader(glsl_helper::vert_0.str(), GL_VERTEX_SHADER));
    for(int i = 0; i < shader_number; i++)
    {
        GLuint vert_shader = glsl_helper::load_shader(glsl_helper::vert_0(), GL_VERTEX_SHADER);
        GLuint frag_shader = glsl_helper::load_shader(glsl_helper::frag_0(), GL_FRAGMENT_SHADER);
        
        GLuint program = glCreateProgram();
        if(program == 0)
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

        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);
        
        if(glGetError() != GL_NO_ERROR)
        {
            std::printf("Failed to load shader %d with error %d\n", i, glGetError());
        }
        // programs.push_back(glsl_helper::load_shader(glsl_helper::vert_0(), GL_VERTEX_SHADER));
        // programs.push_back(glsl_helper::load_shader(glsl_helper::frag_0(), GL_FRAGMENT_SHADER));
    }
    return programs;
}