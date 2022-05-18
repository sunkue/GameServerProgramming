#include "stdafx.h"
#include "Obj.h"
#include "World.h"
#include "Server.h"

bool DynamicObj::Move(const Position diff)
{
	Position pos;
	Position sector;

	{
		shared_lock lck(PositionLock);
		pos = GetPos();
		sector = GetSectorIdx();
	}

	pos += diff;

	if (IsOverFlowedPosition(pos))
	{
		pos -= diff;
		return false;
	}

	auto newSector = GetSectorByPosition(pos);

	{
		unique_lock lck(PositionLock);
		SetPos(pos);
	}

	if (sector != newSector)
	{
		World::Get().ChangeSector(this, newSector);
	}

	return true;
}

void DynamicObj::Update()
{

}

bool DynamicObj::Enable()
{
	if (Enable_)return false;
	auto sector = GetSectorByPosition(GetPos());
	SetSectorIdx(sector);
	Enable_ = true;
	World::Get().ChangeSector(this, sector);
	return true;
}

bool DynamicObj::Disable()
{
	if (!Enable_)return false;
	Enable_ = false;

	{
		auto pos = GetPos();
		auto nearSectors = World::Get().GetNearSectors4(pos, GetSectorIdx());

		sc_remove_obj remove;
		remove.id = Id_;

		for (auto& ns : nearSectors)
		{
			shared_lock lck{ ns->PlayerLock };
			auto& players = ns->GetPlayers();
			for (auto& p : players)
			{
				if (p->GetId() == Id_)continue;
				Server::Get().GetClients()[p->Id_].DoSend(&remove);
			}
		}
	}
	return true;
}

bool StaticObj::IsInSightAndEnable(Position target)
{
	if (!Enable_)return false;
	auto diff = target - Pos_;
	auto maxCompDiff = glm::compMax(glm::abs(diff));
	return maxCompDiff <= SightRange_ / 2;
}

bool StaticObj::IsInSight(Position target)
{
	auto diff = target - Pos_;
	auto maxCompDiff = glm::compMax(glm::abs(diff));
	return maxCompDiff <= SightRange_ / 2;
}


