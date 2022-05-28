#include "stdafx.h"
#include "Chat.h"

inline string VisualizationId(ID id)
{
	if (id < MAX_PLAYER) return "P_" + to_string(id);
	else if (id < MAX_PLAYER + MAX_MONSTER) return "M_" + to_string(id - MAX_PLAYER);
	else if (id < MAX_PLAYER + MAX_MONSTER + MAX_NPC) return "NPC_" + to_string(id - MAX_PLAYER - MAX_MONSTER);
	else return "ERR_ID";
}

void ChatManager::RenderChat() const
{
	gui::Begin("Chat", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar);
	for (auto& c : chats_)//| std::views::reverse
	{
		auto time = system_clock::to_time_t(c.timestamp);
		tm t; localtime_s(&t, &time);
		string str;
		str += "[";
		str += to_string(t.tm_hour);
		str += ":";
		str += to_string(t.tm_min);
		str += ":";
		str += to_string(t.tm_sec);
		str += "] ";
		str += VisualizationId(c.speaker);
		str += " :: ";
		str += c.mess;
		gui::Text(str.c_str());
	}
	gui::SetScrollHereY(1);
	gui::End();
}
