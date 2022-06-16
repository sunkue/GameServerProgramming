#pragma once

ImVec2 TileSize();
ImVec2 ImGuiGetCenterPosOfTile(Position tilePos);
Position GetTileIdxFromWindowPos(double x_, double y_);

class GameGuiManager
{
	SINGLE_TON(GameGuiManager) = default;
public:
	void DrawGui();
protected:
private:
	void DrawPlayerInfo();
	void DrawChat();
	void DrawSpeechBubble();
	void DrawSelectedObjInfo();
	void DrawMyInventory();
	void DrawMyEquiment();
	void DrawMyStatus();
	void DrawMySkill();
	void DrawSelectedInventory();
public:
	SET(SelectedObjId);
	TOGGLE(ShowMyInventory);
	TOGGLE(ShowMyEquipment);
	TOGGLE(ShowMyStatus);
	TOGGLE(ShowSelectedInventory);
	TOGGLE(ShowMySkill);
private:
	ID SelectedObjId_{ -1 };
	bool ShowMyInventory_{};
	bool ShowMyEquipment_{};
	bool ShowMyStatus_{};
	bool ShowMySkill_{};
	bool ShowSelectedInventory_{};
};
