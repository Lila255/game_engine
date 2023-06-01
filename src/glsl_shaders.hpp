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
			// uniform sampler2D tex;
			layout(binding = 0, r8ui) uniform readonly uimage2D tex;
			uniform ivec2 texture_size; 
			out vec4 out_Color;

			void main()
			{
				// float intensity = texture(tex, gl_FragCoord).r;
				// uint value = imageLoad(tex, ivec2(2, 2)).r;
				// uint value = imageLoad(tex, ivec2(gl_FragCoord.xy * vec2(128 * 3, 128 * 3))).r;
				uint value = imageLoad(tex, ivec2(v_TexCoord.xy * vec2(texture_size))).r;
				// uint value = uint(texture(tex, gl_FragCoord).r * 255);

				if(value == 0) {
					out_Color = vec4(0.0, 0.0, 0.0, 0.0);
				} else if(value == 1) {
					out_Color = vec4(0.4118, 0.7333, 0.0039, 1.0);
				} else if(value == 2) {
					out_Color = vec4(0.75, 0.8, 0.1, 1.0);
				} else if(value == 100) {
					out_Color = vec4(0.9, 0.7, 0.4, 1.0);
				} else if(value == 101) {
					out_Color = vec4(0.19, 0.09, 0.197, 1.0);
				} else if(value == 102) {
					out_Color = vec4(.7, .7, .7, 1.0);
				} else if(value == 103) {
					out_Color = vec4(0.760, 0.139, 0.0152, 1.0);
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
		return R"(
			#version 430
			layout (location = 0) in vec3 in_Position;
			layout (location = 1) in vec2 in_TexCoord;
			uniform mat4 projection;
			uniform mat4 view;
			out vec2 v_TexCoord;
			void main()
			{
				v_TexCoord = in_TexCoord;
				gl_Position = projection * view * vec4(in_Position, 1.0);
				// gl_Position = vec4(in_Position, 1.0);
				// gl_Position = projection * vec4(in_Position, 1.0);
			}
		)";
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

	// shader that renders a r32ui texture as a rgba8 texture to the screen using the projection and view matrix uniforms
	std::string vert_2()
	{
		return
			R"(
			#version 430
			layout (location = 0) in vec3 in_Position;
			layout (location = 1) in vec2 texCoord;
			uniform mat4 projection;
			uniform mat4 view;
			out vec2 v_TexCoord;
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
			// uniform usampler2D tex;
			layout(binding = 0, r32ui) uniform readonly uimage2D tex;
			uniform ivec2 texture_size; 
			out vec4 out_Color;

			const float weights[5] = float[](0.1201, 0.2339, 0.2931, 0.2339, 0.1201);

			void main()
			{
				// out_Color = vec4(val >> 24, (val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF) / 255.0;
				
				// vec2 texelSize = 1.0 / vec2(texture_size);
				// vec4 blurredColor = vec4(0.0);
				
				// for (int i = -2; i <= 2; i++)
				// {
				// 	vec2 offset = vec2(float(i) * texelSize.x, 0.0);
				// 	uint value = imageLoad(tex, ivec2((v_TexCoord + offset) * texture_size)).r;
				// 	blurredColor += vec4(vec3(value), 1.0) * weights[i + 2];
				// }
				// out_Color  = vec4(0.0, 0.0, 0.0, 1 - (blurredColor.r / 100.0));

				uint value = imageLoad(tex, ivec2(v_TexCoord * texture_size)).r;
				out_Color = vec4(0.0, 0.0, 0.0, 1 - (value / 256.0));


			}
		)";
	}

	std::string light_compute_shader()
	{
		return R"(
			#version 430
			layout(binding = 0, r32ui) uniform uimage2D lightingTex; // 3x3 grid of 128x128 textures, lays on top of worldChunkTex textures
			
			// layout(binding = 0, rg32ui) uniform coherent uimage2D lightingTex;
			layout(binding = 1, r8ui) uniform readonly uimage2D world_chunks;  // 3x3 grid of 128x128 textures, lays on top of worldChunkTex textures

			const int CHUNK_SIZE = 128;  // width and height of a chunk
			const int CHUNKS_WIDTH = 3;	// number of chunks in a row

			uniform ivec2 texture_size; // size of the lighting texture

			// layout(binding = 2) uniform NormalVectors {
    		// 	vec2 data[256];
			// } normal_vectors;
			//uniform vec2 normal_vectors[256];
			layout(binding = 2, rg32f) uniform readonly image1D normal_vectors;

			// texCoord is position in lightingTex
			uint sampleWorld(vec2 texCoord) {
				if (texCoord.x < 0.0 || texCoord.x >= texture_size.x || texCoord.y < 0.0 || texCoord.y >= texture_size.y) {
					return -1;	// -1 is out of bounds, uint max value so doing value > 0 will work
				}
				uint value = imageLoad(world_chunks, ivec2(texCoord.xy)).r;
				return value;
			}

			uniform vec2 player_pos;    // relative to the lighting texture, center of the screen but not center of lighting texture
			const int max_ray_length = 128;

			// raycast from player_pos to the edge of the screen, 360 invocations
			layout(local_size_x = 1, local_size_y = 1) in;
			void main() {
				if(player_pos.x < 0.0 || player_pos.x > texture_size.x || player_pos.y < 0.0 || player_pos.y > texture_size.y) {
					return;
				}
				int ray_index = int(gl_GlobalInvocationID.x);
				float ray_angle = float(ray_index) / float(360) * 2.0 * 3.1415926535897932384626433832795;

				vec2 ray_dir = vec2(cos(ray_angle), sin(ray_angle));

				vec2 ray_pos = player_pos;
				
				uint bounces = 0;

				// loop until we hit something or we reach max_ray_length
				for (int i = 0; i < max_ray_length;) {
					ivec2 ray_pos_int = ivec2(ray_pos);
					while (ray_pos_int == ivec2(ray_pos)) { // if we're still in the same pixel, move forward a bit
						ray_pos += ray_dir;
						i++;
					}

					// ray_pos += ray_dir;
					uint sample_v = sampleWorld(ray_pos);
					if (sample_v > 0.0) {     // hit something, bounce

						uint surround_values = 0;
						// for(int j = 0; j < 9; j++)
						// {
						// 	if(j == 4) continue; // skip center
						// 	int x = j % 3;
						// 	int y = j / 3;
						// 	uint world_sample = sampleWorld(ray_pos + vec2(x - 1, y - 1));
						// 	if(world_sample > 0) {
						// 		uint bit_to_shift_by = j;
						// 		if(j > 4) bit_to_shift_by--;
						// 		surround_values |= 1 << (8-bit_to_shift_by);
						// 	}
						// }
						uint counter = 0;
						for(int y = 0; y < 3; y++)
						{
							for(int x = 0; x < 3; x++)
							{
								if(x == 1 && y == 1) continue;
								counter++;
								uint world_sample = sampleWorld(ray_pos + vec2(x - 1, y - 1));
								if(world_sample > 0) {
									surround_values |= 1 << (8-counter);
								}

							}
						}

						vec2 normal_vec = imageLoad(normal_vectors, int(surround_values)).rg;
						// vec2 normal_vec = normal_vectors[surround_values];
						// vec2 normal_vec = normal_vectors.data[surround_values % 256];
						// vec2 normal_vec = vec2(0.0, 1.0);


						float dot_val = dot(normal_vec, ray_dir);

						if(dot_val < 0.0) {
							normal_vec = -normal_vec;
							dot_val = -dot_val;
						}
						vec2 reflection = ray_dir - 2.0 * dot_val * normal_vec;
						
						ray_dir = reflection;
						ray_pos += ray_dir;
						// if(sampleWorld(ray_pos) > 0) {
						// 	break;
						// }
						bounces++;
						i++;
					}

					// imageStore(lightingTex, ivec2(ray_pos), imageLoad(lightingTex, ivec2(ray_pos)) + vec4(0, 0, 0, 255));
					imageAtomicAdd(lightingTex,  ivec2(ray_pos.xy), 1);
					// imageAtomicAdd(lightingTex,  ivec2(ray_pos.xy), int(bounces > 0));
					// imageAtomicExchange(lightingTex, ivec3(ray_pos), 0x202020ff);

					// uint rgb_val = uint(texelFetch(tex, ivec2(ray_pos.xy), 1).r * 4294967295);

					//uint texelValue = uint(texture(lightingTex, vec3(ray_pos.xy, 0)).r * 4294967295);
					// uint texelValue = imageLoad(lightingTex, ivec2(ray_pos.xy)).g;

					// imageStore(lightingTex, ivec2(ray_pos.xy), uvec4(0, texelValue + 1, 0, 0));
				}
			};
		)";
	}

	// Average the light textures over the last 15 frames
	// Take in three textures, one to add, one to minus, and the result
	// each frame, minus the last frame and add the new frame
	// uniform to keep track of how many frames have been added, only minus if equal to 15
	std::string light_blending_compute_shader()
	{
		return R"(
			#version 430
			layout(binding = 0, r32ui) uniform uimage2D blended_lights;
			layout(binding = 1, r32ui) uniform uimage2D new_lights;
			layout(binding = 2, r32ui) uniform uimage2D old_lights;
			uniform int total_frames;

			layout(local_size_x = 1, local_size_y = 1) in;

			void main() {
				ivec2 texel = ivec2(gl_GlobalInvocationID.xy);
				uvec4 new_light = imageLoad(new_lights, texel);
				uvec4 blended_light = imageLoad(blended_lights, texel);

				uvec4 old_light = imageLoad(old_lights, texel);

				// blended_light -= uvec4(vec4(old_light) / total_frames);
				// blended_light += uvec4(vec4(new_light) / total_frames);
				blended_light -= old_light;
				blended_light += new_light;
				
				imageStore(blended_lights, texel, blended_light);
			}

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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
			1,
			100,
			1,
			100,
			100,
			100,
			100,
			100,
			100,
			1,
			100,
			100,
			100,
			100,
			101,
			100,
			1,
			100,
			101,
			100,
			101,
		};

		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 3, 8, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, character_width, character_height, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
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
	printf("load_compute_shader 0: %d\n", glGetError());

	GLuint compute_shader = glsl_helper::load_shader(shader_src, GL_COMPUTE_SHADER);
	printf("load_compute_shader 1: %d\n", glGetError());
	GLuint program = glCreateProgram();
	if (program == 0)
	{
		std::printf("Failed to create shader program with error %d\n", glGetError());
	}
	printf("load_compute_shader 2: %d\n", glGetError());

	glAttachShader(program, compute_shader);
	printf("load_compute_shader 3: %d\n", glGetError());
	glLinkProgram(program);
	printf("load_compute_shader 4: %d\n", glGetError());

	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		printf("Failed to link shader program\n");
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		char *log = (char *)malloc(logLength);
		glGetProgramInfoLog(program, logLength, NULL, log);
		printf("Error log: %s\n", log);
		free(log);
		// return -1;
	}
	else
	{
		printf("Shader program linked\n");
	}

	glDeleteShader(compute_shader);

	return program;
}