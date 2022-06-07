#include "stdafx.h"
#include "GameGui.h"
#include "Game.h"
#include "Chat.h"
#include "Character.h"
#include "System.h"

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

Position GetTileIdxFromWindowPos(double x_, double y_)
{
	auto tileSize = TileSize();
	float x = static_cast<float>(x_);
	float y = static_cast<float>(y_);
	x -= 10;
	y -= 10;
	x -= tileSize.x / 2;
	y -= tileSize.y / 2;
	x /= tileSize.x;
	y /= tileSize.y;
	return { ceil(x) , ceil(y) };
}

void GameGuiManager::DrawGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	gui::NewFrame();

	DrawPlayerInfo();
	DrawChat();
	DrawSpeechBubble();
	DrawSelectedObjInfo();

	gui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(gui::GetDrawData());
}

void GameGuiManager::DrawPlayerInfo()
{
	gui::Begin("PlayerInfo", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	auto& player = Game::Get().GetPlayer();
	auto id = Game::Get().GetId();
	auto pos = player.GetPos();
	auto hp = player.GetHp();
	auto exp = player.GetExp();
	auto level = player.GetLevel();
	gui::Text(("ID :: "s + player.GetName()).c_str());
	gui::Text(("HP :: "s + to_string(hp) + "/"s + to_string(MaxHp(level))).c_str());
	gui::Text(("LEVEL :: "s + to_string(level) + "  EXP :: "s + to_string(exp) + "/"s + to_string(RequireExp(level))).c_str());
	gui::Text(("Positon :: "s + to_string(pos.x) + " "s + to_string(pos.y)).c_str());
	gui::End();
}

void GameGuiManager::DrawChat()
{
	ChatManager::Get().RenderChat();
}

void GameGuiManager::DrawSpeechBubble()
{
	gui::StyleColorsLight();
	auto playerPos = Game::Get().GetPlayer().GetPos();
	for (auto& c : Game::Get().GetCharacters())
	{
		auto& speechBubble = c.second.GetSpeechBubble();
		if (speechBubble.second <= system_clock::now())
			continue;

		auto str = "SpeechBubble"s + to_string(c.first);
		gui::Begin(str.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
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

void GameGuiManager::DrawSelectedObjInfo()
{
	if (SelectedObjId_ < 0) return;

	auto& selectedObj = Game::Get().GetCharacters()[SelectedObjId_];

	const string windowName = "SelectedObjWindow";

	if (SelectedObjId_ < MAX_PLAYER)
	{
		//	Player = > 이름 / 레벨 / 파티초대
		gui::Begin(windowName.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		string str = "ID : "s + VisualizationId(SelectedObjId_);
		gui::Text(str.c_str());
		str = "LV : "s + to_string(selectedObj.GetLevel());
		gui::Text(str.c_str());
		if (gui::Button("Invite to Party"))
		{
			// 파초
		}
		gui::End();
	}
	else if (SelectedObjId_ < MAX_PLAYER + MAX_MONSTER)
	{
		//	Monster = > 이름 / 타입 / 레벨 / HP
		gui::Begin(windowName.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		string str = "ID : "s + VisualizationId(SelectedObjId_);
		gui::Text(str.c_str());
		str = "LV : "s + to_string(selectedObj.GetLevel());
		gui::Text(str.c_str());
		gui::End();

	}
	else if (SelectedObjId_ < MAX_PLAYER + MAX_MONSTER + MAX_NPC)
	{
		//	Npc = > 이름 / 상점 / 퀘스트
		gui::Begin(windowName.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

		gui::End();
	}
	else if (SelectedObjId_ < MAX_OBJECT)
	{
		//	Npc = > 이름 / 상점 / 퀘스트
		gui::Begin(windowName.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		string str = "ID : "s + VisualizationId(SelectedObjId_);
		gui::Text(str.c_str());
		gui::End();
	}
	else cerr << "ERR_ID";

	{
		ImVec2 windowPos;
		auto& player = Game::Get().GetCharacters()[Game::Get().GetId()];
		windowPos = ImGuiGetCenterPosOfTile(Position{ selectedObj.GetPos() - player.GetPos() } + Position{ 10, 10 });
		gui::SetWindowPos(windowName.c_str(), windowPos);
	}
}
