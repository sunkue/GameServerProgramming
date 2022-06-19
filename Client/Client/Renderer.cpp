#include "stdafx.h"
#include "Renderer.h"
#include "System.h"
#include "Game.h"
#include "Texture.h"
#include "GameGui.h"

////////////////////////////////////////////////////////////////

Renderer::ScreenQuad::ScreenQuad()
{
	glGenVertexArrays(1, &QuadVao_);
	GLuint vbo;
	glGenBuffers(1, &vbo);

	struct QUAD
	{
		glm::vec2 pos;
		glm::vec2 tex;
	};

	QUAD quadv[] =
	{
		{ {-1,-1},{0,0} }
		,{{ 1, 1},{1,1} }
		,{{-1, 1},{0,1} }
		,{{-1,-1},{0,0} }
		,{{ 1,-1},{1,0} }
		,{{ 1, 1},{1,1} }
	};

	glBindVertexArray(QuadVao_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadv), quadv, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QUAD), (const GLvoid*)offsetof(QUAD, pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QUAD), (const GLvoid*)offsetof(QUAD, tex));

	glBindVertexArray(0);
}

void Renderer::ScreenQuad::DrawQuad()
{
	glBindVertexArray(QuadVao_);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

////////////////////////////////////////////////////////////////
void Renderer::ReadyDraw()
{
	glClearColor(0.2f, 0, 0.3f, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw()
{
	ReadyDraw();
	auto& player = Game::Get().GetPlayer();
	auto myPos = player.GetPos();
	bool startWithDark = (myPos.x + myPos.y) % 2;

	BgShader_->Use();
	BgShader_->Set("u_texture", BgTiles_);
	BgShader_->Set("u_start_with_dark", startWithDark);
	BgShader_->Set("u_focus_center", FocusCenter_);
	BgShader_->Set("u_raw_col", WINDOW_SIZE);
	ScreenQuad::Get().DrawQuad();

	ObjShader_->Use();
	ObjShader_->Set("u_texture", ObjTiles_);
	ObjShader_->Set("u_start_with_dark", startWithDark);
	ObjShader_->Set("u_focus_center", FocusCenter_);
	ObjShader_->Set("u_raw_col", WINDOW_SIZE);
	ObjShader_->Set("u_units", glm::vec2{ 5, 4 });
	for (const auto& other : Game::Get().GetCharacters())
	{
		auto& id = other.first;
		auto& obj = other.second;
		
		if (obj.GetState() != eCharacterState::enable) continue;

		eObjType Type{};
		if (id < MAX_PLAYER) Type = eObjType::Wlook;  // player
		else if (id < MAX_PLAYER + MAX_MONSTER)
		{
			switch (id % 4)
			{
			case 0:
			{
				Type = eObjType::Bpawn; // monster
			}
			CASE 1 :
			{
				Type = eObjType::Bbishop; // monster
			}
			CASE 2 :
			{
				Type = eObjType::Bqueen; // monster
			}
			CASE 3 :
			{
				Type = eObjType::Bknight; // monster
			}
			}
		}
		else if (id < MAX_CHARACTER) Type = eObjType::Wbishop; //npc
		else if (id < MAX_OBJECT) Type = eObjType::Bking; // obstacle

		if (id == Game::Get().GetId()) Type = eObjType::Wknight;

		ObjShader_->Set("u_type", static_cast<int>(Type));
		ObjShader_->Set("u_position", obj.GetPos() - myPos + Position{ WINDOW_SIZE / 2 } + Position{ 0, -1 });
		ScreenQuad::Get().DrawQuad();
	}

	ObjShader_->Set("u_texture", EffectTiles_);
	for (auto& e : Effects_)
	{
		if (clk::now() < e._Myfirst._Val) continue;
		auto effectType = e._Get_rest()._Get_rest()._Myfirst._Val;
		auto effectPos = e._Get_rest()._Get_rest()._Get_rest()._Myfirst._Val;
		ObjShader_->Set("u_type", static_cast<int>(effectType));
		ObjShader_->Set("u_position", effectPos - myPos + Position{ WINDOW_SIZE / 2 } + Position{ 0, -1 });
		ScreenQuad::Get().DrawQuad();
	}

	ObjShader_->Set("u_texture", ItemTiles_);
	ObjShader_->Set("u_units", glm::vec2{ 5, 5 });
	for (auto& item : Game::Get().GetItemInstances())
	{
		ObjShader_->Set("u_type", static_cast<int>(item.Type));
		ObjShader_->Set("u_position", item.Pos - myPos + Position{ WINDOW_SIZE / 2 } + Position{ 0, -1 });
		ScreenQuad::Get().DrawQuad();
	}

	GameGuiManager::Get().DrawGui();
}

void Renderer::Init()
{
	glEnable(GL_BLEND);
	glViewport(10, 10, System::Get().Screen.width - 20, System::Get().Screen.height - 20);
	LoadTexture();
	LoadShader();
}

void Renderer::LoadShader()
{
	vector<string> VS; VS.emplace_back("./Resource/Shader/background.vert");
	vector<string> FS; FS.emplace_back("./Resource/Shader/background.frag");
	vector<string> GS;

	BgShader_ = Shader::Create(VS, FS, GS);

	VS.clear(); VS.emplace_back("./Resource/Shader/obj.vert");
	FS.clear(); FS.emplace_back("./Resource/Shader/obj.frag");
	GS.clear();
	ObjShader_ = Shader::Create(VS, FS, GS);
}

void Renderer::LoadTexture()
{
	BgTiles_ = Texture::Create("background_CND1.png");
	ObjTiles_ = Texture::Create("character_CND.png");
	ItemTiles_ = Texture::Create("Items_CND.png");
	EffectTiles_ = Texture::Create("effect_CND.png");
}
