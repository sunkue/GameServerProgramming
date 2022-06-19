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
	DrawMyInventory();
	DrawMyEquiment();
	DrawMyStatus();
	DrawPartyCrewInfos();

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
	auto money = player.GetMoney();
	gui::Text(("ID :: "s + player.GetName()).c_str());
	gui::Text(("HP :: "s + to_string(hp) + "/"s + to_string(player.MaxHp())).c_str());
	gui::Text(("LEVEL :: "s + to_string(level) + "  EXP :: "s + to_string(exp) + "/"s + to_string(RequireExp(level))).c_str());
	gui::Text(("Money :: "s + to_string(money)).c_str());
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
		if (Position{ Position{ c.second.GetPos() - playerPos } + Position{ 10, 10 } }.x <= 0)
		{
			//	cout << VisualizationId(c.second.GetId()) << endl;
			//	cout << speechBubble.first.c_str() << endl;
			//	cout << endl;
		}
	}
	gui::StyleColorsDark();
}

void GameGuiManager::DrawMyInventory()
{
	if (!ShowMyInventory_) return;
	auto& inventory = Game::Get().GetPlayer().GetInventory();
	gui::Begin("Inventory", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar);
	gui::BeginTabBar("InventoryType", ImGuiTabBarFlags_NoTabListScrollingButtons);
	if (gui::BeginTabItem("Consumable"))
	{
		for (auto& item : inventory.GetItems())
		{
			if (!(eItemType::_ConsumableItemStartLine < item.first &&
				item.first < eItemType::_ConsumableItemEndLine)) continue;
			if (item.second <= 0) continue;
			string str = ItemTypeDecoder::toString(item.first) + " (" + to_string(item.second) + ")";
			gui::Text(str.c_str());
			gui::SameLine();
			if (gui::SmallButton(string{ "USE##"s + to_string(static_cast<int>(item.first)) }.c_str()))
			{
				cs_use_item useItem;
				useItem.type = item.first;
				Networker::Get().DoSend(&useItem);
			}
		}
		gui::EndTabItem();
	}
	if (gui::BeginTabItem("Equipment"))
	{
		for (auto& item : inventory.GetItems())
		{
			if (!(eItemType::_EquipmentItemStartLine < item.first &&
				item.first < eItemType::_EquipmentItemEndLine)) continue;
			if (item.second <= 0) continue;
			string str = ItemTypeDecoder::toString(item.first) + " (" + to_string(item.second) + ")";
			gui::Text(str.c_str());
			gui::SameLine();
			if (gui::SmallButton(string{ "EQUIP##"s + to_string(static_cast<int>(item.first)) }.c_str()))
			{
				cs_use_item useItem;
				useItem.type = item.first;
				Networker::Get().DoSend(&useItem);
			}
		}
	}
	gui::EndTabBar();

	gui::End();
}

void GameGuiManager::DrawMyEquiment()
{
	if (!ShowMyEquipment_) return;
	auto& equimentStates = Game::Get().GetPlayer().GetEquipment().GetEquimentStates();
	gui::Begin("EquipmentState", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	auto weapon = ItemTypeDecoder::toString(equimentStates[static_cast<int>(eEquimentablePart::weapon)]);
	auto head = ItemTypeDecoder::toString(equimentStates[static_cast<int>(eEquimentablePart::head)]);
	auto body = ItemTypeDecoder::toString(equimentStates[static_cast<int>(eEquimentablePart::body)]);
	auto shoes = ItemTypeDecoder::toString(equimentStates[static_cast<int>(eEquimentablePart::shoes)]);
	gui::Text(string{ "[Weapon] :: "s + weapon }.c_str());
	gui::Text(string{ "[Head]   :: "s + head }.c_str());
	gui::Text(string{ "[Body]   :: "s + body }.c_str());
	gui::Text(string{ "[Shoes]  :: "s + shoes }.c_str());
	gui::End();
}

void GameGuiManager::DrawMyStatus()
{
	if (!ShowMyStatus_) return;
	gui::Begin("Status", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	auto& player = Game::Get().GetPlayer();
	auto hp = player.GetHp();
	auto AttackPoint = player.GetAttackPoint();
	auto ArmorPoint = player.GetArmorPoint();
	auto MovementSpeed = player.GetMovemetSpeed();
	gui::Text(("AttackPoint   :: "s + to_string(AttackPoint)).c_str());
	gui::Text(("ArmorPoint    :: "s + to_string(ArmorPoint)).c_str());
	gui::Text(("HP / MAXHP    :: "s + to_string(hp) + "/"s + to_string(player.MaxHp())).c_str());
	gui::Text(("MovementSpeed :: "s + string{ format("{}", MovementSpeed) }).c_str());
	gui::End();
}

void GameGuiManager::DrawMySkill()
{
	// z, x 키 사용하기
	if (!ShowMySkill_) return;
	//gui::Begin("Skills", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	//gui::End();
}

void GameGuiManager::DrawPartyCrewInfos()
{
	if (!ShowMyParty_) return;
	auto& player = Game::Get().GetPlayer();
	auto& characters = Game::Get().GetCharacters();
	gui::Begin("PartyCrew", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	if (ShowPartyJoinRequest_)
	{
		auto name = characters[PartyInviter_].GetName();
		gui::Text((std::format("[{}] invite u to party.", name)).c_str());
		if (gui::SmallButton("JOIN"))
		{
			cs_accept_party_invite accept;
			accept.partyId = InvitedPartyId_;
			Networker::Get().DoSend(&accept);

			ShowPartyJoinRequest_ = false;
			PartyInviter_ = -1;
			InvitedPartyId_ = -1;
		}
		gui::SameLine();
		if (gui::SmallButton("NOPE"))
		{
			ShowPartyJoinRequest_ = false;
			PartyInviter_ = -1;
			InvitedPartyId_ = -1;
		}
	}
	else
	{
		for (auto crewId : player.GetPartyCrews())
		{
			if (crewId < 0) continue;
			auto& crew = characters[crewId];
			auto name = crew.GetName();
			auto lvl = crew.GetLevel();
			auto hp = crew.GetHp();
			auto maxHp = crew.MaxHp();
			gui::Text((name + "  Lvl::"s + to_string(lvl) + "  HP::"s + to_string(hp) + "/"s + to_string(maxHp)).c_str());
		}

		gui::SetCursorPos({ 149, 98 });
		if (gui::SmallButton("Leave Party"))
		{
			cs_leave_party leave;
			Networker::Get().DoSend(&leave);
		}
	}
	gui::End();
}

void GameGuiManager::DrawSelectedInventory()
{
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
			cs_invite_to_party inviteRequest;
			inviteRequest.targetId = SelectedObjId_;
			Networker::Get().DoSend(&inviteRequest);
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
		str = "HP : "s + to_string(selectedObj.GetHp());
		gui::Text(str.c_str());
		str = "st : "s + to_string(static_cast<int>(selectedObj.GetState()));
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
		//	Obstacles
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


