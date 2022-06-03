#include "stdafx.h"
#include "Chat.h"
#include "Networker.h"

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
	if (!gui::IsWindowFocused())
		gui::SetScrollHereY(1);
	gui::End();

	gui::Begin("ChatInput", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
	static char chatBuffer[MAX_CHAT_BUFFER_SIZE]{};
	if (gui::InputText("ENTER", chatBuffer, MAX_CHAT_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		if (auto chatLen = strlen(chatBuffer))
		{
			cs_chat chat;
			strcpy_s(chat.chat, MAX_CHAT_SIZE, chatBuffer);
			chat.size -= static_cast<decltype(chat.size)>(MAX_CHAT_SIZE - chatLen);
			Networker::Get().DoSend(&chat);
		}
		ZeroMemory(chatBuffer, sizeof(chatBuffer));
	}
	gui::End();
}

