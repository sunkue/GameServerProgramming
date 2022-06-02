#pragma once

#include "Game.h"

struct Chat
{
	ID speaker{ -1 };
	char mess[MAX_CHAT_SIZE + 1]{};
	system_clock::time_point timestamp;
};

inline string VisualizationId(ID id);

class ChatManager
{
	SINGLE_TON(ChatManager) = default;
public:
	void AddChat(const Chat& chat) { chats_.push_back(chat); }
	void RenderChat() const;
private:

public:

private:
	vector<Chat> chats_;
};

