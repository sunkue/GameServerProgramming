#include "stdafx.h"
#include "Character.h"
#include "TimerEvent.h"
#include "Monster.h"
#include "Player.h"
#include "Server.h"
#include "DataBase.h"

/////////////////////////////////
// 
//			CharacterManager
// 
/////////////////////////////////

CharacterManager::CharacterManager()
{
	// Players
	cerr << "initiallize Players..";
	for (ID _id = 0; _id < MAX_PLAYER; _id++)
	{
		auto id = _id;
		Characters_[id] = make_unique<Player>(id);
	}
	cerr << "..done" << endl;

	// Monsters
	cerr << "initiallize Monsters..";
	for (ID _id = 0; _id < MAX_MONSTER; _id++)
	{
		auto id = _id + MAX_PLAYER;
		Characters_[id] = make_unique<Monster>(id);
		InitialMove(id, { rand() % MAP_SIZE, rand() % MAP_SIZE });
	}
	cerr << "..done" << endl;

	// NPCs
	cerr << "initiallize NPCs..";
	for (ID _id = 0; _id < MAX_NPC; _id++)
	{
		auto id = _id + MAX_PLAYER + MAX_MONSTER;
		Characters_[id] = make_unique<Character>(id);
		//		Characters_[id]->Enable();
	}
	cerr << "..done" << endl;
}

bool CharacterManager::Move(ID Id_, eMoveOper oper)
{
	const Position MOVE_OPER_TABLE[4]
	{ { 0, -1 }, { 0, 1 } ,{ 1, 0 } ,{ -1, 0 } };
	auto moved = Characters_[Id_]->Move(MOVE_OPER_TABLE[static_cast<int>(oper)]);
	return moved;
}

bool CharacterManager::Move(ID Id_, Position to)
{
	auto moved = Characters_[Id_]->Move(to);
	return moved;
}

bool CharacterManager::InitialMove(ID Id_, Position to)
{
	Characters_[Id_]->StartPosition_ = to;
	return Move(Id_, to);
}

void CharacterManager::InitFromDataBase(ID id, DbCharacterID dbId)
{
	reinterpret_cast<Player*>(Characters_[id].get())->SetDbId(dbId);
	auto& client = Server::Get().GetClients()[id];

	QueryRequest q;
	q.Query = L"EXEC SelectCharacterDataById "s + to_wstring(dbId);
	q.Targets = make_shared<vector<any>>(); q.Targets->reserve(20);
	q.Targets->emplace_back(make_any<SQLINTEGER>()); // LVEL
	q.Targets->emplace_back(make_any<wstring>()); // NAME
	q.Targets->emplace_back(make_any<SQLINTEGER>()); // HP
	q.Targets->emplace_back(make_any<SQLINTEGER>()); // POSX
	q.Targets->emplace_back(make_any<SQLINTEGER>()); // POSY
	q.Targets->emplace_back(make_any<SQLINTEGER>()); // MONEY
	q.Targets->emplace_back(make_any<SQLINTEGER>()); // EXP

	q.Func = [this, id](const vector<any>& t)
	{
		auto level = any_cast<SQLINTEGER>(t[0]);
		auto name = any_cast<wstring>(t[1]);
		auto hp = any_cast<SQLINTEGER>(t[2]);
		auto posx = any_cast<SQLINTEGER>(t[3]);
		auto posy = any_cast<SQLINTEGER>(t[4]);
		auto money = any_cast<SQLINTEGER>(t[5]);
		auto exp = any_cast<SQLINTEGER>(t[6]);

		auto& player = reinterpret_cast<Player&>(*CharacterManager::Get().GetCharacters()[id].get());
		
		player.SetLevel(level);
		player.SetHp(hp);
		player.SetName(name);
		player.SetMoney(money);
		player.SetExp(exp);

		if (CharacterManager::Get().InitialMove(id, Position{ posx, posy }))
		{
			sc_set_position set_pos;
			auto pos = player.GetPos();
			set_pos.id = NetID(id);
			set_pos.pos = pos;
			Server::Get().GetClients()[id].DoSend(&set_pos);
		}
		else
		{
			cerr << "[!!!]OverflowedInitPosition" << GetPosition(id).x << " " << GetPosition(id).y << endl;
		}

		{
			sc_ready ready;
			ready.level = level;
			ready.hp = hp;
			ready.money = money;
			ready.exp = exp;
			size_t temp; wcstombs_s(&temp, ready.name, name.c_str(), name.size());
			ready.size -= static_cast<decltype(ready.size)>(MAX_CHARACTER_NAME_SIZE - strlen(ready.name));
			Server::Get().GetClients()[id].DoSend(&ready);
		}

		CharacterManager::Get().Enable(id);
	};
	DataBase::Get().AddQueryRequest(q);
}

void Character::Regen()
{
	Hp_ = 1;
	Move(StartPosition_ - GetPos());
}

void Character::Attack(const vector<ID>& target, int damage)
{
	if (!Attackable_ || damage < 0) return;
	Attackable_ = false;
	AttackImpl(target);
	EventManager::Get().AddEvent({ [&Attackable = Attackable_]()
		{ Attackable = true; }, AttackCooltime_ });
}

bool Character::Move(Position diff)
{
	if (!Moveable_) return false;
	auto ret = DynamicObj::Move(diff);
	if (ret)
	{
		Moveable_ = false;
		EventManager::Get().AddEvent({ [&Moveable = Moveable_]()
			{ Moveable = true; }, MovementCooltime_ });
	}
	return ret;
}
