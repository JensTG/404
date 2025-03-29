#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif // !STB_IMAGE_IMPLEMENTATION
using namespace std;

// Texture2D is able to store and configure a texture in OpenGL.
// It also hosts utility functions for easy management.
class Texture2D
{
public:
	// holds the ID of the texture object, used for all texture operations to reference to this particular texture
	unsigned int ID;
	// texture image dimensions
	unsigned int Width, Height; // width and height of loaded image in pixels
	// texture Format
	unsigned int Internal_Format; // format of texture object
	unsigned int Image_Format; // format of loaded image
	// texture configuration
	unsigned int Wrap_S; // wrapping mode on S axis
	unsigned int Wrap_T; // wrapping mode on T axis
	unsigned int Filter_Min; // filtering mode if texture pixels < screen pixels
	unsigned int Filter_Max; // filtering mode if texture pixels > screen pixels
	// constructor (sets default texture modes)
	Texture2D() {
		glGenTextures(1, &ID);

		Wrap_S = GL_CLAMP_TO_BORDER;
		Wrap_T = GL_CLAMP_TO_BORDER;

		Filter_Min = GL_NEAREST;
		Filter_Max = GL_NEAREST;

		Internal_Format = GL_RGBA;
		Image_Format = GL_RGBA;

		Width = 0, Height = 0;
	}

	// generates texture from image data
	void Generate(unsigned int width, unsigned int height, unsigned char* data) {
		Width = width, Height = height;

		glBindTexture(GL_TEXTURE_2D, ID);
		glTexImage2D(GL_TEXTURE_2D, 0, Internal_Format, width, height, 0, Image_Format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Wrap_S);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Wrap_T);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter_Min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter_Max);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void Load(const char* path) {
		// load and generate the texture
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path, &width, &height, &nrChannels, STBI_rgb_alpha);
		if (data)
		{
			Generate(width, height, data);
			loaded = true;
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}

	// binds the texture as the current active GL_TEXTURE_2D texture object
	void Bind() const {
		if (loaded) {
			glBindTexture(GL_TEXTURE_2D, ID);
		}
		else {
			std::cout << "ERROR: Texture not loaded\n";
		}
	}

private:
	bool loaded = false;
};

#endif