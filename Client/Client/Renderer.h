#pragma once

#include "Texture.h"
#include "Shader.h"

class Renderer
{
	SINGLE_TON(Renderer) { init(); };

public:
	void draw();
	void ready_draw();

private:
	class ScreenQuad
	{
		SINGLE_TON(ScreenQuad);
	public:
		void draw_quad();
	private:
		GLuint quad_vao;
	};

private:
	void init();
	void load_shader();
	void load_texture();

private:
	bool start_with_dark{ false };
	bool focus_center{ false };
	TexturePtr bg_tiles;
	ShaderPtr bg_shader;

	TexturePtr obj_tiles;
	ShaderPtr obj_shader;
};


