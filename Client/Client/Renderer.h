#pragma once

#include "Texture.h"
#include "Shader.h"

class Renderer
{
	SINGLE_TON(Renderer) { Init(); };

public:
	void Draw();
	void ReadyDraw();

private:
	class ScreenQuad
	{
		SINGLE_TON(ScreenQuad);
	public:
		void DrawQuad();
	private:
		GLuint QuadVao_;
	};

private:
	void Init();
	void LoadShader();
	void LoadTexture();

private:
	bool FocusCenter_{ true };
	TexturePtr BgTiles_;
	ShaderPtr BgShader_;

	TexturePtr ObjTiles_;
	ShaderPtr ObjShader_;
};

