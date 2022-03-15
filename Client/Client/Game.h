#pragma once

#include "Obj.h"
#include "Player.h"

class Game
{
	SINGLE_TON(Game)
	{
		player_ = make_shared<Player>();
		objs_.emplace_back(player_);

		KEY_BOARD_EVENT_MANAGER::get().BindMainKeyFunc(
			[this](const KEY_BOARD_EVENT_MANAGER::key_event& key)->bool
			{ return player_->process_input(key); });
	};

public:
	void update()
	{
		
	}

public:
	GET(n);
	SET(n);
	GET(objs);
	GET(player);

private:
	int n_;
	shared_ptr<Player> player_;
	vector<shared_ptr<Obj>> objs_;
};
