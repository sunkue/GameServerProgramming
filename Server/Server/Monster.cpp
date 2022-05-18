#include "stdafx.h"
#include "Monster.h"
#include "World.h"
#include "TimerEvent.h"
#include "CharacterManager.h"

bool Monster::Move(Position diff)
{
	constexpr size_t NearListReserveHint = 20;
	unordered_set<Player*> oldNearList; oldNearList.reserve(NearListReserveHint);
	unordered_set<Player*> newNearList; newNearList.reserve(NearListReserveHint);

	{
		auto oldPos = GetPos();
		auto oldSector = GetSectorIdx();
		auto nearSector = World::Get().GetNearSectors4(oldPos, oldSector);
		for (auto& ns : nearSector)
		{
			shared_lock lck{ ns->PlayerLock };
			auto& players = ns->GetPlayers();
			for (auto p : players)
			{
				if (p->IsInSightAndEnable(oldPos))
				{
					oldNearList.insert(p);
				}
			}
		}
	}

	DynamicObj::Move(diff);

	Position newPos;
	{
		newPos = GetPos();
		auto newSector = GetSectorIdx();
		auto nearSector = World::Get().GetNearSectors4(newPos, newSector);
		for (auto& ns : nearSector)
		{
			shared_lock lck{ ns->PlayerLock };
			auto& players = ns->GetPlayers();
			for (auto p : players)
			{
				if (p->IsInSightAndEnable(newPos))
				{
					newNearList.insert(p);
				}
			}
		}
	}

	if (newNearList.empty())
	{
		Disable();
	}

	for (auto& p : newNearList)
	{
		p->InsertToViewList(Id_);
	}

	for (auto& p : oldNearList)
	{
		if (!newNearList.contains(p))
		{
			p->EraseFromViewList(Id_);
		}
	}
}

void Monster::Update()
{
	CharacterManager::Get().Move(Id_, static_cast<move_oper>(rand() % 4));

	if (GetEnable())
		EventManager::Get().AddEvent({ [this]() { Update(); }, 1s });
}

bool Monster::Enable()
{
	if (!DynamicObj::Enable()) return false;
	EventManager::Get().AddEvent({ [this]() { Update(); }, 1s });
	return true;
}

bool Monster::Disable()
{
	if (!DynamicObj::Disable()) return false;


	return true;
}
