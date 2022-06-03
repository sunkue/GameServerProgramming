#include "stdafx.h"
#include "Game.h"



void Game::Update()
{
}

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
