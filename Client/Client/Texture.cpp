#include "stdafx.h"
#include "Texture.h"

#pragma warning(push)
#pragma warning(disable: 26451)
#pragma warning(disable: 6011)
#pragma warning(disable: 6262)
#pragma warning(disable: 6308)
#pragma warning(disable: 28182)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#pragma warning(pop)

GLuint load_texture_file(const char* path, const string& directory)
{
	stbi_set_flip_vertically_on_load(true); 
	
	string filename = string(path);
	filename = directory + "/"s + filename;

	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLint format = GL_GREEN + nrComponents;
		//if (nrComponents == 1)
		//	format = GL_RED;
		//else if (nrComponents == 3)
		//	format = GL_RGB;
		//else if (nrComponents == 4)
		//	format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
	}

	return textureID;
}

