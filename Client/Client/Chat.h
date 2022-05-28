#pragma once

struct Chat
{
	ID speaker{ -1 };
	char mess[MAX_CHAT_SIZE + 1]{};
	system_clock::time_point timestamp;
};

inline string VisualizationId(ID id)
{
	if (id < MAX_PLAYER) return "P_" + to_string(id);
	else if (id < MAX_PLAYER + MAX_MONSTER) return "M_" + to_string(id - MAX_PLAYER);
	else if (id < MAX_PLAYER + MAX_MONSTER + MAX_NPC) return "NPC_" + to_string(id - MAX_PLAYER - MAX_MONSTER);
	else return "ERR_ID";
}

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

