#ifndef GLSL_HELPER_H
#define GLSL_HELPER_H

// Store strings for GLSL shaders and load into programs
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <gl\GL.h>
#include <GLFW/glfw3.h>
#include <array>
#include <unordered_map>

namespace glsl_helper
{
	extern std::array<float, 256 * 4> colours;
	extern std::unordered_map<std::string, GLuint> texture_map;

	std::string frag_0();

	std::string vert_0();

	// generic shader program for drawing lines and points using the projection and view matrix uniforms
	std::string vert_1();

	std::string frag_1();

	// shader that renders a r32ui texture as a rgba8 texture to the screen using the projection and view matrix uniforms
	std::string vert_2();

	std::string frag_2();

	std::string light_compute_shader();

	// Average the light textures over the last n frames
	// Take in three textures, one to add, one to minus, and the result
	// each frame, minus the last frame and add the new frame
	// uniform to keep track of how many frames have been added, only minus if equal to 15
	std::string light_blending_compute_shader();
	


	std::string light_blurring_compute_shader();

	GLuint load_shader(const std::string &source, GLenum type);

	const uint32_t character_height = 12;
	const uint32_t character_width = 4;

	void create_character_texture(GLuint &texture);

	const uint16_t projectile_height = 4;
	const uint16_t projectile_width = 4;


	// create_projectile_texture
	void create_projectile_texture(GLuint &texture);

	GLuint create_texture_from_data(std::string texture_name, const uint8_t *data, int width, int height);
};

std::vector<GLuint> load_shaders(std::string vert, std::string frag);

GLuint load_compute_shader(std::string shader_src);


#endif