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
		// std::stringstream frag_0;
		// frag_0 << "#version 330\n"
		// 	   << "in vec2 v_TexCoord;\n"
		// 	   << "uniform sampler2D tex;\n"
		// 	   << "out vec4 out_Color;\n"
		// 	   << "void main()\n"
		// 	   << "{\n"
		// 	   << "float intensity = texture(tex, v_TexCoord).r;\n"
		// 	   << "if(intensity == 0.0) {\n"
		// 	   << "out_Color = vec4(0.0, 0.0, 0.0, 0.0);\n"
		// 	   << "} else if(intensity == 1.0 / 255) {\n"
		// 	   << "out_Color = vec4(0.4118, 0.7333, 0.0039, 1.0);\n"
		// 	   << "} else if(intensity == 2.0 / 255) {\n"
		// 	   << "out_Color = vec4(0.75, 0.8, 0.1, 1.0);\n"
		// 	   << "} else if(intensity == 100.0 / 255) {\n"
		// 	   << "out_Color = vec4(0.9, 0.7, 0.4, 1.0);\n"
		// 	   << "} else if(intensity == 101.0 / 255) {\n"
		// 	   << "out_Color = vec4(0.19, 0.09, 0.197, 1.0);\n"
		// 	   << "} else {\n"
		// 	   << "out_Color = vec4(1.0, 0.41, 0.71, 1.0);\n"
		// 	   << "}\n"
		// 	   << "}\n";

		// return
		// 	"#version 330\n"
		// 	"in vec2 v_TexCoord;\n"
		// 	"uniform sampler2D tex;\n"
		// 	"out vec4 out_Color;\n"
		// 	"void main()\n"
		// 	"{"
		// 	"float intensity = texture(tex, v_TexCoord).r;"
		// 	"if(intensity == 0.0) {\n"
		// 	"out_Color = vec4(0.0, 0.0, 0.0, 0.0);\n"
		// 	"} else if(intensity == 1.0 / 255) {\n"
		// 	"out_Color = vec4(0.4118, 0.7333, 0.0039, 1.0);\n"
		// 	"} else if(intensity == 2.0 / 255) {\n"
		// 	"out_Color = vec4(0.75, 0.8, 0.1, 1.0);\n"
		// 	"} else if(intensity == 100.0 / 255) {\n"
		// 	"out_Color = vec4(0.9, 0.7, 0.4, 1.0);\n"
		// 	"} else if(intensity == 101.0 / 255) {\n"
		// 	"out_Color = vec4(0.19, 0.09, 0.197, 1.0);\n"
		// 	"} else {\n"
		// 	"out_Color = vec4(1.0, 0.41, 0.71, 1.0);\n"
		// 	"}\n"
		// 	"}\n";
		return
			R"(
			#version 430
			in vec2 v_TexCoord;
			uniform sampler2D tex;
			out vec4 out_Color;
			void main()
			{
				float intensity = texture(tex, v_TexCoord).r;
				if(intensity == 0.0) {
					out_Color = vec4(0.0, 0.0, 0.0, 0.0);
				} else if(intensity == 1.0 / 255) {
					out_Color = vec4(0.4118, 0.7333, 0.0039, 1.0);
				} else if(intensity == 2.0 / 255) {
					out_Color = vec4(0.75, 0.8, 0.1, 1.0);
				} else if(intensity == 100.0 / 255) {
					out_Color = vec4(0.9, 0.7, 0.4, 1.0);
				} else if(intensity == 101.0 / 255) {
					out_Color = vec4(0.19, 0.09, 0.197, 1.0);
				} else {
					out_Color = vec4(1.0, 0.41, 0.71, 1.0);
				}
			}
		)";

		// return frag_0.str();
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
			<< "#version 430\n"
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

	// generic shader program for drawing lines and points using the projection and view matrix uniforms
	std::string vert_1()
	{
		std::stringstream vert_1;
		vert_1
			<< "#version 430\n"
			<< "layout (location = 0) in vec3 in_Position;\n"
			<< "uniform mat4 projection;\n"
			<< "uniform mat4 view;\n"
			<< "void main()\n"
			<< "{\n"
			<< "gl_Position = projection * view * vec4(in_Position, 1.0);\n"
			<< "}\n";
		return vert_1.str();
	}
	std::string frag_1()
	{
		std::stringstream frag_1;
		frag_1
			<< "#version 430\n"
			<< "out vec4 out_Color;\n"
			<< "void main()\n"
			<< "{\n"
			<< "out_Color = vec4(1.0, 1.0, 1.0, 1.0);\n"
			<< "}\n";
		return frag_1.str();
	}

	// generic shader that renders a texture to the screen using the projection and view matrix uniforms
	std::string vert_2()
	{
		return
			R"(
			#version 430
			layout (location = 0) in vec3 in_Position;
			layout (location = 1) in vec2 texCoord;
			uniform mat4 projection;
			uniform mat4 view;
			void main()
			{
    			v_TexCoord = texCoord;
				gl_Position = projection * view * vec4(in_Position, 1.0);
			}
		)";
	}
	std::string frag_2()
	{
		return
			R"(
			#version 430
			in vec2 v_TexCoord;
			uniform sampler2D tex;
			out vec4 out_Color;
			void main()
			{
				out_Color = texture(tex, v_TexCoord);
			}
		)";
	}

	std::string light_compute_shader()
	{
		return R"(
			#version 430

			layout(binding = 0, rgba8) uniform image2D lightingTex; // 3x3 grid of 128x128 textures, lays on top of worldChunkTex textures
			layout(binding = 1, R8) uniform readonly image2D world_chunks;  // 3x3 grid of 128x128 textures, lays on top of worldChunkTex textures

			const int CHUNK_SIZE = 128;  // width and height of a chunk
			const int CHUNKS_WIDTH = 3;	// number of chunks in a row

			// texCoord is position in lightingTex
			int sampleWorld(vec2 texCoord) {
				if (texCoord.x < 0.0 || texCoord.x > 1.0 || texCoord.y < 0.0 || texCoord.y > 1.0) {
					return 0;
				}

				// // determine which texture to sample based on texCoord
				// int chunkX = int(texCoord.x / CHUNK_SIZE);
				// int chunkY = int(texCoord.y / CHUNK_SIZE);
				// int index = chunkX + chunkY * CHUNKS_WIDTH;
				// //vec2 offset = vec2(mod(float(index), float(CHUNKS_WIDTH)), floor(float(index) / float(CHUNKS_WIDTH)));
				// vec2 offset = vec2(chunkX, chunkY);
				// vec2 chunkTexCoord = vec2(mod(texCoord.x, CHUNK_SIZE), mod(texCoord.y, CHUNK_SIZE));
				// int value = int(imageLoad(worldChunkTex[index], ivec2(chunkTexCoord)).r);
				int value = int(imageLoad(world_chunks, ivec2(texCoord)).r);
				return value;
			}

			uniform vec2 player_pos;    // relative to the lighting texture, center of the screen but not center of lighting texture
			const int max_ray_length = 128;

			// raycast from player_pos to the edge of the screen, 360 invocations
			layout(local_size_x = 1, local_size_y = 1) in;
			void main() {
				if(player_pos.x < 0.0 || player_pos.x > CHUNK_SIZE * CHUNKS_WIDTH || player_pos.y < 0.0 || player_pos.y > CHUNK_SIZE * CHUNKS_WIDTH) {
					return;
				}
				int ray_index = int(gl_GlobalInvocationID.x);
				float ray_angle = float(ray_index) / float(360) * 2.0 * 3.1415926535897932384626433832795;

				vec2 ray_dir = vec2(cos(ray_angle), sin(ray_angle));

				vec2 ray_pos = player_pos;

				float ray_dist = 0.0;

				// loop until we hit something or we reach max_ray_length
				for (int i = 0; i < max_ray_length; i++) {
					ray_pos += ray_dir;

					int sample_v = sampleWorld(ray_pos);

					if (sample_v > 0.0) {     // hit something, stop
						ray_dist = float(i);
						break;
					}

					// imageStore(lightingTex, ivec2(ray_pos), imageLoad(lightingTex, ivec2(ray_pos)) + vec4(32, 32, 32, 255));
					imageAtomicAdd(lightingTex, ivec2(ray_pos), vec4(32, 32, 32, 255));
				}
			};
		)";
	}

	GLuint load_shader(const std::string &source, GLenum type)
	{
		printf("load_shader_0: %d\n", glGetError());
		GLuint shader = glCreateShader(type);
		const char *src = source.c_str();
		glShaderSource(shader, 1, &src, nullptr);
		printf("load_shader_1: %d\n", glGetError());
		glCompileShader(shader);
		printf("load_shader_2: %d\n", glGetError());

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		printf("load_shader_3: %d\n", glGetError());
		if (isCompiled == GL_FALSE)
		{
			std::printf("Failed to compile shader %d\n", glGetError());

			// Get shader compilation log
			int maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<char> errorLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

			// Print error log
			std::printf("Failed to compile shader: %s\n", &errorLog[0]);
			
			return 0;
		}
		return shader;
	}

	const uint32_t character_height = 8;
	const uint32_t character_width = 3;
	void create_character_texture(GLuint &texture)
	{
		// Create a texture for the character
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
		std::array<uint8_t, 24> data = {
			101,
			100,
			101,
			0,
			100,
			0,
			100,
			100,
			100,
			100,
			100,
			100,
			0,
			100,
			100,
			100,
			100,
			101,
			100,
			0,
			100,
			101,
			100,
			101,
		};

		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 3, 8, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, character_width, character_height, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
	}

};

std::vector<GLuint> load_shaders(std::string vert, std::string frag)
{
	std::vector<GLuint> programs;
	uint16_t shader_number = 1;

	// programs.push_back(glsl_helper::load_shader(glsl_helper::vert_0.str(), GL_VERTEX_SHADER));
	for (int i = 0; i < shader_number; i++)
	{
		GLuint vert_shader = glsl_helper::load_shader(vert, GL_VERTEX_SHADER);
		GLuint frag_shader = glsl_helper::load_shader(frag, GL_FRAGMENT_SHADER);

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

GLuint load_compute_shader(std::string shader_src)
{
	int max_image_units;
	glGetIntegerv(GL_MAX_IMAGE_UNITS, &max_image_units);
	printf("Max image units: %d\n", max_image_units);

	GLuint compute_shader = glsl_helper::load_shader(shader_src, GL_COMPUTE_SHADER);
	GLuint program = glCreateProgram();
	if (program == 0)
	{
		std::printf("Failed to create shader program with error %d\n", glGetError());
	}
	printf("Error_1: %d\n", glGetError());
	glAttachShader(program, compute_shader);
	printf("Error_0: %d\n", glGetError());
	glLinkProgram(program);
	printf("Error_2: %d\n", glGetError());

	glDeleteShader(compute_shader);

	return program;
}