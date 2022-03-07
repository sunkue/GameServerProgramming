#pragma once

GLuint load_texture_file(const char* path, const string& directory);

using TexturePtr = shared_ptr<struct Texture>;
struct Texture
{
	CREATE_SHARED(TexturePtr, Texture);

	GLuint id;
private:
	Texture(const char* path, const string& directory = "Resource/Texture")
	{
		id = load_texture_file(path, directory);
	}
};