#pragma once
#include <GL/glew.h>
#include <gl\GL.h>
#include <cstdint>
#include <stdio.h>

namespace game_engine {

	void create_texture_from_data(GLuint texture_id, const uint8_t *data, int width, int height, int channels)
	{
		// create texture
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		// Set texture data to values from data
		if (channels == 1) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
		} else {
			printf("Error: Unsupported number of channels: %d\n", channels);
		}
		
		// generate mipmaps for the currently bound texture
		glGenerateMipmap(GL_TEXTURE_2D);

		// unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);

		// check for errors
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			printf("OpenGL error: %d\n", err);
		}

		
	}

}