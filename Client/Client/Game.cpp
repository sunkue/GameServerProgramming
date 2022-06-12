#include "stdafx.h"
#include "Game.h"
#include "Renderer.h"


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
	auto now = clk::now();

	auto& effects = Renderer::Get().GetEffects();
	for (auto it = effects.cbegin(); it != effects.cend();)
	{
		if (it->_Myfirst._Val + it->_Get_rest()._Myfirst._Val < now)
			it = effects.erase(it);
		else it++;
	}
	
	static auto GarbageCollectingTimeInterval = now;
	if (now < GarbageCollectingTimeInterval) return;
	GarbageCollectingTimeInterval = now + 1s;
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
