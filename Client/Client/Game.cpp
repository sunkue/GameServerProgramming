#include "stdafx.h"
#include "Game.h"



void Game::Init()
{
	KeyboardEventManager::Get().BindMainKeyFunc(
		[this](const KeyboardEventManager::KeyEvent& key)->bool
		{ return Characters_[Id_].ProcessInput(key); });
	MouseEventManager::Get().BindDefaultScrollFunc(
		[this](const MouseEventManager::ScrollEvent& scroll)->bool
		{ return Characters_[Id_].ProcessInput(scroll); });
	MouseEventManager::Get().BindDefaultButtonFunc(
		[this](const MouseEventManager::ButtonEvent& button)->bool
		{ return Characters_[Id_].ProcessInput(button); });
	MouseEventManager::Get().BindDefaultPosFunc(
		[this](const MouseEventManager::PosEvent& pos)->bool
		{ return Characters_[Id_].ProcessInput(pos); });
}

void Game::Update()
{
	GarbageCollect();
}

void Game::GarbageCollect()
{
	static auto GarbageCollectingTimeInterval = clk::now();
	if (clk::now() < GarbageCollectingTimeInterval) return;
	GarbageCollectingTimeInterval = clk::now() + 1s;
DanglingIterator:
	for (auto& c : Characters_)
	{
		if (c.second.GetDisabled())
		{
			Characters_.erase(c.first);
			goto DanglingIterator;
		}
	}
}
