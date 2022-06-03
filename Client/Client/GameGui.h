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
public:
	SET(SelectedObjId);
protected:
private:
	ID SelectedObjId_{ -1 };
};