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
	// NPCs
	cerr << "initiallize Obstracles..";
	for (ID _id = 0; _id < MAX_OBSTACLE; _id++)
	{
		auto id = _id;
		Obstacles_[id] = make_unique<DynamicObj>(id);
		Obstacles_[id]->SetPos(Position{ rand() % MAP_SIZE, rand() % MAP_SIZE });
		Obstacles_[id]->Enable();
	}
	cerr << "..done" << endl;


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
		Characters_[id]->SetLevel(rand() % 5 + 1);
		Characters_[id]->SetHp(MaxHp(Characters_[id]->GetLevel()));
		switch (auto x = rand() % 20)
		{
		case 0: case 1: case 2: case 3: case 4:
			Characters_[id]->GetInventory().CreateItem(ItemBase{ eItemType::hpPotion , x + 1 });
		break; case 5: case 6: case 7: case 8:
			Characters_[id]->GetInventory().CreateItem(ItemBase{ eItemType::expPotion , 3 });

		break; case 9:
			Characters_[id]->GetInventory().CreateItem(ItemBase{ eItemType::stoneSword , 1 });
		break; case 10:
			Characters_[id]->GetInventory().CreateItem(ItemBase{ eItemType::ironSword , 1 });
		break; case 11:
			Characters_[id]->GetInventory().CreateItem(ItemBase{ eItemType::goldSword , 1 });
		break; case 12:
			Characters_[id]->GetInventory().CreateItem(ItemBase{ eItemType::diamondSword , 1 });

		break; case 13:
			Characters_[id]->GetInventory().CreateItem(ItemBase{ eItemType::ironHelmet , 1 });
		break; case 14:
			Characters_[id]->GetInventory().CreateItem(ItemBase{ eItemType::steelHelmet , 1 });

		break; case 15:
			Characters_[id]->GetInventory().CreateItem(ItemBase{ eItemType::leatherArmor , 1 });
		break; case 16:
			Characters_[id]->GetInventory().CreateItem(ItemBase{ eItemType::steelArmor , 1 });
		break; case 17:
			Characters_[id]->GetInventory().CreateItem(ItemBase{ eItemType::goldArmor , 1 });

		break; case 18:
			Characters_[id]->GetInventory().CreateItem(ItemBase{ eItemType::robberBoots , 1 });
		break; case 19:
			Characters_[id]->GetInventory().CreateItem(ItemBase{ eItemType::wingShoes , 1 });

		break; default: break;
		}
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

bool CharacterManager::MoveForce(ID Id_, Position to)
{
	auto moved = Characters_[Id_]->MoveForce(to);
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

	q.Func = [this, id, dbId](const vector<any>& t)
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

		{
			QueryRequest q;
			q.Query = L"EXEC SelectItemDataById "s + to_wstring(dbId);
			q.Targets = make_shared<vector<any>>(); q.Targets->reserve(3);
			q.Targets->emplace_back(make_any<wstring>());	 // name
			q.Targets->emplace_back(make_any<SQLINTEGER>()); // num
			q.Targets->emplace_back(make_any<BOOL>()); // used
			q.Func = [id](const vector<any>& t)
			{
				auto itemName = any_cast<wstring>(t[0]);
				auto itemNum = any_cast<SQLINTEGER>(t[1]);
				auto itemUsed = any_cast<BOOL>(t[2]);
#pragma warning( disable : 4244 ) 
				auto stringitemName = string{ itemName.begin(), itemName.end() };
#pragma warning( default : 4244 )
				trim(stringitemName);
				auto itemType = ItemTypeDecoder::toType(stringitemName);
				auto& player = CharacterManager::Get().GetCharacters()[id];
				player->GetInventory().CreateItem(ItemBase{ itemType , itemNum });
				{
					sc_sum_item sumItem;
					sumItem.changed = itemNum;
					sumItem.type = itemType;
					Server::Get().GetClients()[player->GetId()].DoSend(&sumItem);
				}

				if (itemUsed)
					reinterpret_cast<Player*>(player.get())->Equip(itemType);
			};
			DataBase::Get().AddQueryRequest(q);
		}

		for (int failed = 1; !CharacterManager::Get().InitialMove(id, Position{ posx + rand() % failed, posy + rand() % failed }); failed++)
			;;;

		{
			sc_set_position set_pos;
			auto pos = player.GetPos();
			set_pos.id = NetID(id);
			set_pos.pos = pos;
			Server::Get().GetClients()[id].DoSend(&set_pos);
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
		CharacterManager::Get().MoveForce(id, Position{});
	};
	DataBase::Get().AddQueryRequest(q);
}

void CharacterManager::ActivateSkill(ID id, eSkill skill)
{
	if (auto p = dynamic_cast<Player*>(Characters_[id].get()))
	{
		p->ActivateSkill(skill);
	}
}

void Character::Regen()
{
	Hp_ = 1;
	Move(StartPosition_ - GetPos());
}

bool Character::Attack(const vector<ID>& target, int damage)
{
	if (!Attackable_) return false;
	Attackable_ = false;
	AttackImpl(target, damage);
	EventManager::Get().AddEvent({ [&Attackable = Attackable_]()
		{ Attackable = true; }, AttackCooltime });
	return true;
}

void Character::AttackImpl(const vector<ID>& target, int damage)
{
	auto& characters = CharacterManager::Get().GetCharacters();
	for (auto t : target)
	{
		characters[t]->HpDecrease(Id_, damage);
	}
}

bool Character::Move(Position diff)
{
	if (!Moveable_) return false;
	auto ret = DynamicObj::Move(diff);
	if (ret)
	{
		Moveable_ = false;
		EventManager::Get().AddEvent({ [&Moveable = Moveable_]()
			{ Moveable = true; }, MovementCooltime });
	}
	return ret;
}

int Character::UseItem(eItemType itemType, int num)
{
	return Inventory_.UseItem(Id_, itemType, num);
}

bool Character::MoveForce(Position diff)
{
	return DynamicObj::Move(diff);
}
