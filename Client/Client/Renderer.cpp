#include "stdafx.h"
#include "Renderer.h"
#include "System.h"
#include "Game.h"
#include "Texture.h"
#include "Chat.h"

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

ImVec2 TileSize()
{
	auto wOffsetPerUnit = static_cast<float>(System::Get().Screen.width - 20) / WINDOW_SIZE;
	auto hOffsetPerUnit = static_cast<float>(System::Get().Screen.height - 20) / WINDOW_SIZE;
	return { wOffsetPerUnit , hOffsetPerUnit };
}

ImVec2 ImGuiGetCenterPosOfTile(Position tilePos)
{
	ImVec2 ret{};
	ret.x += 10;
	ret.y += 10;
	auto tileSize = TileSize();
	ret.x += tilePos.x * tileSize.x;
	ret.y += tilePos.y * tileSize.y;
	return ret;
}

void DrawGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	gui::NewFrame();

	// PLAYER INFO
	{
		gui::Begin("PlayerInfo", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		auto& player = Game::Get().GetPlayer();
		auto id = Game::Get().GetId();
		auto pos = player.GetPos();
		auto hp = player.GetHp();
		auto exp = player.GetExp();
		auto level = player.GetLevel();
		gui::Text(("ID :: "s + VisualizationId(id)).c_str());
		gui::Text(("HP :: "s + to_string(hp) + "/"s + to_string(MaxHp(level))).c_str());
		gui::Text(("LEVEL :: "s + to_string(level) + "  EXP :: "s + to_string(exp) + "/"s + to_string(RequireExp(level))).c_str());
		gui::Text(("Positon :: "s + to_string(pos.x) + " "s + to_string(pos.y)).c_str());
		gui::End();
	}

	// CHAT
	{
		ChatManager::Get().RenderChat();
	}

	{
		gui::StyleColorsLight();
		auto playerPos = Game::Get().GetPlayer().GetPos();
		for (auto& c : Game::Get().GetCharacters())
		{
			auto& speechBubble = c.second.GetSpeechBubble();
			if (speechBubble.second <= system_clock::now())
				continue;

			auto str = "SpeechBubble"s + to_string(c.first);
			gui::Begin(str.c_str(), 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
			gui::Text(speechBubble.first.c_str());
			gui::End();

			ImVec2 bubblePos;
			bubblePos = ImGuiGetCenterPosOfTile(Position{ c.second.GetPos() - playerPos } + Position{ 10, 10 });
			bubblePos.x += (-10 - 3 * static_cast<int>(speechBubble.first.size())) * (TileSize().x / 25);
			bubblePos.y += -40 * (TileSize().y / 25);
			gui::SetWindowPos(str.c_str(), bubblePos);
		}
		gui::StyleColorsDark();
	}

	gui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(gui::GetDrawData());
}

void Renderer::Draw()
{
	ReadyDraw();
	auto pos = Game::Get().GetPlayer().GetPos();
	bool startWithDark = (pos.x + pos.y) % 2;

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

	for (const auto& other : Game::Get().GetCharacters())
	{
		auto& id = other.first;
		auto& obj = other.second;

		OBJ_TYPE Type{};
		if (id < MAX_PLAYER) Type = OBJ_TYPE::Wlook;
		if (id == Game::Get().GetId()) Type = OBJ_TYPE::Wknight;
		if (MAX_PLAYER <= id && id < MAX_PLAYER + MAX_MONSTER) Type = OBJ_TYPE::Bpawn;

		ObjShader_->Set("u_type", int(Type));
		ObjShader_->Set("u_position", obj.GetPos() - pos + Position{ WINDOW_SIZE / 2 } + Position{ 0, -1 });
		ScreenQuad::Get().DrawQuad();
	}

	DrawGui();
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
	BgTiles_ = Texture::Create("background.png");
	ObjTiles_ = Texture::Create("chess.png");
}
