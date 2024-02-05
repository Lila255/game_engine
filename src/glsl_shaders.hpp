#pragma once
// Store strings for GLSL shaders and load into programs
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <gl\GL.h>
#include <GLFW/glfw3.h>

namespace glsl_helper
{
	std::array<float, 256 * 4> colours {
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.039f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.9425f, 1.0f,
		0.077f, 0.66f, 0.10f, 1.0f,
		0.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.6102f, 0.988f, 1.0f, 1.0f,
		0.102f, 0.988f, 1.0f, 1.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.039f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.039f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.123f, 0.8f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.039f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.039f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.039f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.123f, 0.8f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.039f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.039f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.039f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.039f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.123f, 0.8f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.039f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.039f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.039f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.123f, 0.8f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.977f, 0.66f, 0.10f, 1.0f,
		1.0f, 0.7647058824f, 0.0f, 1.0f,
		0.9f, 0.7f, 0.4f, 1.0f,
		0.19f, 0.09f, 0.197f, 1.0f,
		0.7f, 0.7f, 0.7f, 1.0f,
		0.319f, 0.319f, 0.352f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.15f, 0.15f, 0.15f, 0.65f,
		0.01f, 0.01f, 0.98f, 0.65f,
		1.0f, 1.0f, 1.0f, 0.2f,
		1.0f, 0.861f, 0.360f, 1.0f,
		0.45f, 0.157f, 0.039f, 1.0f,
		0.5f, 0.446f, 0.425f, 1.0f,
		0.102f, 0.988f, 1.0f, 1.0f
	};

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
			uniform vec4 colours[255];
			out vec4 out_Color;

			void main()
			{
				// float intensity = texture(tex, gl_FragCoord).r;
				// uint value = imageLoad(tex, ivec2(2, 2)).r;
				// uint value = imageLoad(tex, ivec2(gl_FragCoord.xy * vec2(128 * 3, 128 * 3))).r;
				uint value = imageLoad(tex, ivec2(v_TexCoord.xy * vec2(texture_size))).r;
				// uint value = uint(texture(tex, gl_FragCoord).r * 255);

				out_Color = colours[value];
				

			// 	if(value == 0) {
			// 		out_Color = vec4(0.0, 0.0, 0.0, 0.0);
			// 	} else if(value == 1) {
			// 		out_Color = vec4(0.15, 0.15, 0.15, 0.65);
			// 	} else if(value == 2) {
			// 		out_Color = vec4(0.01, 0.01, 0.98, 0.65);
			// 	} else if(value == 3) {
			// 		out_Color = vec4(1.0, 1.0, 1.0, 0.2);
			// 	} else if(value == 4) {
			// 		out_Color = vec4(1.0, 0.861, 0.360, 1.0);
			// 	} else if(value == 5) {
			// 		out_Color = vec4(0.45, 0.157, 0.039, 1.0);
			// 	} else if(value == 6) {
			// 		out_Color = vec4(0.5, 0.446, 0.425, 1.0);
			// 	} else if(value == 7) {
			// 		out_Color = vec4(0.5, 0.446, 0.425, 1.0);
			// 	} else if(value == 9) {
			// 		out_Color = vec4(0.977, 0.66, 0.10, 1.0);
			// 	} else if(value == 10) {
			// 		out_Color = vec4(1.0, 0.7647058824, 0.0, 1.0);
			// // from here on, make the values random and vibrant colours that are easily identifiable. colourful colours like blue green pink
				
				
			// 	} else if(value == 100) {
			// 		out_Color = vec4(0.9, 0.7, 0.4, 1.0);
			// 	} else if(value == 101) {
			// 		out_Color = vec4(0.19, 0.09, 0.197, 1.0);
			// 	} else if(value == 102) {
			// 		out_Color = vec4(.7, .7, .7, 1.0);
			// 	} else if(value == 103) {
			// 		out_Color = vec4(0.319, 0.319, 0.352, 1.0);
			// 	} else {
			// 		out_Color = vec4(1.0, 0.41, 0.71, 1.0);
			// 	}

		//	switch (value)
		//	{
		//	case 0:
		//		out_Color = vec4(0.0, 0.0, 0.0, 0.0);
		//		break;
		//	case 1:
		//		out_Color = vec4(0.15, 0.15, 0.15, 0.65);
		//		break;
		//	case 2:
		//		out_Color = vec4(0.01, 0.01, 0.98, 0.65);
		//		break;
		//	case 3:
		//		out_Color = vec4(1.0, 1.0, 1.0, 0.2);
		//		break;
		//	case 4:
		//		out_Color = vec4(1.0, 0.861, 0.360, 1.0);
		//		break;
		//	case 5:
		//		out_Color = vec4(0.45, 0.157, 0.039, 1.0);
		//		break;
		//	case 6:
		//		out_Color = vec4(0.5, 0.446, 0.425, 1.0);
		//		break;
		//	case 9:
		//		out_Color = vec4(0.077, 0.66, 0.0, 1.0);
		//		break;
		//	case 10:
		//		out_Color = vec4(1.0, 0.7647058824, 0.0, 1.0);
		//		break;
		//	case 100:
		//		out_Color = vec4(0.9, 0.7, 0.4, 1.0);
		//		break;
		//	case 101:
		//		out_Color = vec4(0.19, 0.09, 0.197, 1.0);
		//		break;
		//	case 102:
		//		out_Color = vec4(.7, .7, .7, 1.0);
		//		break;
		//	case 103:
		//		out_Color = vec4(0.319, 0.319, 0.352, 1.0);
		//		break;
		//	default:
		//		out_Color = vec4(1.0, 0.41, 0.71, 1.0);
		//		break;
		//	}






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
			layout(binding = 1, r32ui) uniform readonly uimage2D blurred_tex;
			// layout(binding = 2, r32ui) uniform uimage2D colour_tex;
			layout(binding = 2, r8ui) uniform readonly uimage2D world_chunks;
			uniform ivec2 texture_size; 
			out vec4 out_Color;

			uint sampleWorld(vec2 texCoord) {
				if (texCoord.x < 0.0 || texCoord.x >= texture_size.x || texCoord.y < 0.0 || texCoord.y >= texture_size.y) {
					return -1;	// -1 is out of bounds, uint max value so doing value > 0 will work
				}
				uint value = imageLoad(world_chunks, ivec2(texCoord.xy)).r;
				return value;
			}

			vec3 hsv2rgb(float h) {
				float c = 1.0;
				float x = c * (1.0 - abs(mod(h / 60.0, 2.0) - 1.0));
				float m = 0.0;

				vec3 rgb;
				if (h < 60.0) {
					rgb = vec3(c, x, 0.0);
				} else if (h < 120.0) {
					rgb = vec3(x, c, 0.0);
				} else if (h < 180.0) {
					rgb = vec3(0.0, c, x);
				} else if (h < 240.0) {
					rgb = vec3(0.0, x, c);
				} else if (h < 300.0) {
					rgb = vec3(x, 0.0, c);
				} else {
					rgb = vec3(c, 0.0, x);
				}

				return rgb + m;
			}

			void main()
			{
				// out_Color = vec4(val >> 24, (val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF) / 255.0;

				uint value = imageLoad(tex, ivec2(v_TexCoord * texture_size)).r;
				uint blurred_value = imageLoad(blurred_tex, ivec2(v_TexCoord * texture_size)).r;

				// uint hue_value = imageLoad(colour_tex, ivec2(v_TexCoord * texture_size)).r;
				// float hue = (float(hue_value) / 4294967295.0) * 360.0;
				// vec3 rgb = hsv2rgb(hue);
				float inverse_alpha = 0.0;
				uint world_value = sampleWorld(v_TexCoord * texture_size);
				if(world_value < 3) {
					inverse_alpha = value / 192000.0;// + blurred_value / 512000.0;
				} else {
					inverse_alpha = blurred_value / 256000.0;
				}
				
				if(inverse_alpha > 1.0)
					inverse_alpha = 1.0;

				// rgb *= inverse_alpha;

				// out_Color = vec4(rgb.r, rgb.g, rgb.b, 1.0 - inverse_alpha);
				out_Color = vec4(0, 0, 0, 1.0 - inverse_alpha);
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

			const int CHUNK_SIZE = 64;  // width and height of a chunk
			const int CHUNKS_WIDTH = 3;	// number of chunks in a row

			uniform ivec2 texture_size; // size of the lighting texture

			// layout(binding = 2) uniform NormalVectors {
    		// 	vec2 data[256];
			// } normal_vectors;
			//uniform vec2 normal_vectors[256];
			layout(binding = 2, rg32f) uniform readonly image1D normal_vectors;
			
			// layout(binding = 3, r32ui) uniform uimage2D lighting_colour_tex;

			// texCoord is position in lightingTex
			uint sampleWorld(vec2 texCoord) {
				if (texCoord.x < 0.0 || texCoord.x >= texture_size.x || texCoord.y < 0.0 || texCoord.y >= texture_size.y) {
					return -1;	// -1 is out of bounds, uint max value so doing value > 0 will work
				}
				uint value = imageLoad(world_chunks, ivec2(texCoord.xy)).r;
				return value;
			}

			vec2 refract_ray(vec2 ray_dir, float current_ior, float next_ior)
			{
				// float cosThetaI = dot(ray_dir, vec2(0.0, 1.0));
				// float sinThetaI = sqrt(1.0 - cosThetaI * cosThetaI);
				// float sinThetaT = (current_ior / next_ior) * sinThetaI;
				// float cosThetaT = sqrt(1.0 - sinThetaT * sinThetaT);
				// // return vec2(-cosThetaT, sinThetaT);
				// // return normalize(vec2(-ray_dir.x, -ray_dir.y * cosThetaI / cosThetaR));
				// return normalize(vec2(-ray_dir.x * cosThetaT, -ray_dir.y * sinThetaT));
				// // return ray_dir;
				float cosThetaI = dot(normalize(ray_dir), vec2(0.0, 1.0));
				float sinThetaI = sqrt(1.0 - cosThetaI * cosThetaI);
				float sinThetaT = (current_ior / next_ior) * sinThetaI;
				float cosThetaT = sqrt(1.0 - sinThetaT * sinThetaT);

				// Handle total internal reflection
				if (sinThetaT > 1.0) {
					return reflect(ray_dir, vec2(0.0, 1.0));
				}

				return normalize(vec2(-ray_dir.x * cosThetaT, -ray_dir.y * sinThetaT));
			}

			uniform vec2 player_pos;    // relative to the lighting texture, center of the screen but not center of lighting texture
			float step_distance = 1.0;
			
			const int max_ray_length = 512;
			const float ior_values[6] = float[6](1.0, 1.01, 1.33, 1.52, 1.62, 1.65);

			// raycast from player_pos to the edge of the screen, 18000 invocations
			layout(local_size_x = 1, local_size_y = 1) in;
			void main() {
				if(player_pos.x < 0.0 || player_pos.x > texture_size.x || player_pos.y < 0.0 || player_pos.y > texture_size.y) {
					return;
				}
				int ray_index = int(gl_GlobalInvocationID.x);
				float ray_decimal = float(ray_index) / float(18000);
				float ray_angle = float(ray_index) / float(18000) * 2.0 * 3.1415926535897932384626433832795;
				// uint hue_val = uint((float(ray_index) / 18000.0) * 4294967295.0);
				vec2 ray_dir = vec2(cos(ray_angle), sin(ray_angle));

				vec2 ray_pos = player_pos;
				
				uint bounces = 0;
				uint metal_bounces = 0;
				uint refracted_bounces = 0;

				if (sampleWorld(ray_pos) > 0 && sampleWorld(ray_pos) < 3) {
					refracted_bounces = 1;
				}

				// loop until we hit something or we reach max_ray_length
				for (int i = 0; i < max_ray_length;) {
					ivec2 ray_pos_int = ivec2(ray_pos);
					uint sample_v0 = sampleWorld(ray_pos);
					// while (ray_pos_int == ivec2(ray_pos)) { // if we're still in the same pixel, move forward a bit
						ray_pos += step_distance * ray_dir;
						i++;
					// }

					// ray_pos += ray_dir;
					uint sample_v = sampleWorld(ray_pos);
					
					if(sample_v == 1 && sample_v0 != 1)
					{
						refracted_bounces = 1;
					}
					else if(sample_v == 2)
					{
						refracted_bounces = 1;
						
					// 	// hit water, refract
					// 	// ray_dir = refract_ray(ray_dir, 1.0003, 1.33);
						if(sample_v0 != sample_v) 
						{
							ray_dir = refract_ray(ray_dir, ior_values[sample_v0], ior_values[sample_v]);
						}

					// 		ray_dir = refract_ray(ray_dir, 1.0003, 1.33);
					// 		ray_pos += step_distance * ray_dir;
					// }
					} else
					if (sample_v >= 3) {     // hit something, bounce
						if(sample_v == 10) {
							metal_bounces = 1;
						}

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

						float dot_val = dot(normal_vec, ray_dir);

						if(dot_val < 0.0) {
							normal_vec = -normal_vec;
							dot_val = -dot_val;
						}
						vec2 reflection = ray_dir - 2.0 * dot_val * normal_vec;

						// Add a small random variation
						float noise_scale = 0.22; 
						reflection += noise_scale * ((ray_index % 100 - 50) / float(50.0));

						ray_dir = reflection;
						ray_pos += 2 * ray_dir;
						if(sampleWorld(ray_pos) > 0) {
							break;
						}
						bounces++;
						i++;
					}
					// if(bounces > 8) {
					// 	break;
					// }

					// imageAtomicExchange(lighting_colour_tex, ivec2(ray_pos.xy), hue_val);
					
					// imageAtomicAdd(lightingTex,  ivec2(ray_pos.xy),  6 * int(bounces > 0)); 

					// imageAtomicAdd(lightingTex,  ivec2(ray_pos.xy),  uint(8.0 / (bounces / 2.0 + 2))); 
					
					// imageAtomicAdd(lightingTex,  ivec2(ray_pos.xy),  1); 
					
					// imageAtomicAdd(lightingTex,  ivec2(ray_pos.xy),  bounces > 0 ? 0.5 : 1); 
					if(bounces > 0)
					{
						imageAtomicAdd(lightingTex,  ivec2(ray_pos.xy), 5 + metal_bounces - refracted_bounces); 
					} else {
						imageAtomicAdd(lightingTex,  ivec2(ray_pos.xy), 5 - refracted_bounces);
					}
				}
			};
		)";
	}

	// Average the light textures over the last n frames
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
				blended_light -= old_light;// * uvec4(1,1,1, .1);
				blended_light += new_light;// * uvec4(1,1,1, .1);
				
				imageStore(blended_lights, texel, blended_light);
			}

		)";
	}
	
	// std::string colour_blending_compute_shader()
	// {
	// 	return R"(
	// 		#version 430
	// 		layout(binding = 0, r32ui) uniform uimage2D blended_lights;
	// 		layout(binding = 1, r32ui) uniform uimage2D new_lights;
	// 		layout(binding = 2, r32ui) uniform uimage2D old_lights;
	// 		uniform int total_frames;

	// 		layout(local_size_x = 1, local_size_y = 1) in;

	// 		void main() {
	// 			ivec2 texel = ivec2(gl_GlobalInvocationID.xy);
	// 			uvec4 new_light = imageLoad(new_lights, texel);
	// 			uvec4 blended_light = imageLoad(blended_lights, texel);

	// 			uvec4 old_light = imageLoad(old_lights, texel);

	// 			blended_light -= uvec4(vec4(old_light) / float(total_frames));
	// 			blended_light += uvec4(vec4(new_light) / float(total_frames));
	// 			// blended_light -= old_light;
	// 			// blended_light += new_light;
				
	// 			imageStore(blended_lights, texel, blended_light);
	// 		}

	// 	)";
	// }

	std::string light_blurring_compute_shader()
	{
		return R"(
			#version 430
			layout(binding = 0, r32ui) uniform uimage2D blurred_lights;
			layout(binding = 1, r32ui) uniform uimage2D og_lights;

			#define KERNEL_SIZE 9
			const float kernel[KERNEL_SIZE] = float[KERNEL_SIZE](
				0.0625, 0.125, 0.0625,
				0.125,  0.25,  0.125,
				0.0625, 0.125, 0.0625
			);

			layout(local_size_x = 1, local_size_y = 1) in;
			void main() {
				ivec2 texel = ivec2(gl_GlobalInvocationID.xy);
				vec4 result = vec4(0.0);
				
				for(int i = 0; i < KERNEL_SIZE; i++) {
					for(int j = 0; j < KERNEL_SIZE; j++) {
						ivec2 offset = ivec2(i - KERNEL_SIZE / 2, j - KERNEL_SIZE / 2);
						result += imageLoad(og_lights, texel + offset) * kernel[i] * kernel[j];
					}
				}
				
				imageStore(blurred_lights, texel, uvec4(result));
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

	const uint32_t character_height = 12;
	const uint32_t character_width = 4;

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
		std::array<uint8_t, character_height * character_width> data = {
			4,
			5,
			6,
			7,
			4,
			5,
			6,
			7,
			4,
			5,
			61,
			37,
			154,
			52,
			26,
			7,
			42,
			5,
			56,
			7,
			49,
			56,
			69,
			74,
			34,
			54,
			66,
			7,
			4,
			5,
			6,
			87,
			47,
			43,
			96,
			78,
			46,
			51,
			63,
			77,
			49,
			50,
			61,
			17,
			24,
			15,
			16,
			17			
		};
		// std::array<uint8_t, 24> data = {
		// 	101,
		// 	100,
		// 	101,
		// 	1,
		// 	100,
		// 	1,
		// 	100,
		// 	100,
		// 	100,
		// 	100,
		// 	100,
		// 	100,
		// 	1,
		// 	100,
		// 	100,
		// 	100,
		// 	100,
		// 	101,
		// 	100,
		// 	1,
		// 	100,
		// 	101,
		// 	100,
		// 	101,
		// };

		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 3, 8, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, character_width, character_height, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
	}

	const uint16_t projectile_height = 4;
	const uint16_t projectile_width = 4;

	std::unordered_map<std::string, GLuint> texture_map;

	// create_projectile_texture
	void create_projectile_texture(GLuint &texture)
	{
		if(texture_map.count("projectile")) {
			texture = texture_map["projectile"];
			return;
		}
		// Create a texture for the character
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		std::array<uint8_t, 16> data = {
			0, 7, 7, 0,
			7, 0, 0, 7,
			7, 0, 0, 7,
			0, 7, 7, 0
		};
		// std::array<uint8_t, 16> data = {
		// 	7,7,7,7
		// };

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 4, 4, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
		texture_map["projectile"] = texture;
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