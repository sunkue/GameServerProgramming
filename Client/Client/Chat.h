#pragma once

struct Chat
{
	ID speaker;
	char mess[MAX_CHAT_SIZE + 1]{};
	system_clock::time_point timestamp;
};


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

