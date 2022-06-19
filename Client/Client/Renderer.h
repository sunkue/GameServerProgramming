#pragma once

#include "Texture.h"
#include "Shader.h"

enum class eEffectType : int
{
	none = -1,

	booster0 = 0,
	booster1 = 1,
	booster2 = 2,
	booster3 = 3,
	booster4 = 4,

	explosion0 = 10,
	explosion1 = 11,
	explosion2 = 12,
	explosion3 = 13,
	explosion4 = 14,
	explosion5 = 5,

	attack0 = 15,
	attack1 = 16,
	attack2 = 17
};

enum class eObjType : int
{
	Wlook = 0,
	Wknight = 1,
	Wbishop = 2,
	Wking = 3,
	Wqueen = 4,
	Wpawn = 5,

	Blook = 15,
	Bknight = 16,
	Bbishop = 17,
	Bking = 18,
	Bqueen = 19,
	Bpawn = 10,
};

class Renderer
{
	friend class Game;
	SINGLE_TON(Renderer) = default;
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

protected:
	void Init();
private:
	void LoadShader();
	void LoadTexture();

private:
	bool FocusCenter_{ true };
	TexturePtr BgTiles_;
	ShaderPtr BgShader_;

	TexturePtr ObjTiles_;
	TexturePtr ItemTiles_;
	TexturePtr EffectTiles_;
	ShaderPtr ObjShader_;
public:
	GET_REF_UNSAFE(Effects);
private:
	list<tuple<clk::time_point, milliseconds, eEffectType, Position>> Effects_;
};

