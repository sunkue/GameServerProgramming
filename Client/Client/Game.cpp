#include "stdafx.h"
#include "Game.h"



void Game::update()
{
}

void Game::init()
{
	KEY_BOARD_EVENT_MANAGER::Get().BindMainKeyFunc(
		[this](const KEY_BOARD_EVENT_MANAGER::key_event& key)->bool
		{ return Characters_[Id_].ProcessInput(key); });
}
