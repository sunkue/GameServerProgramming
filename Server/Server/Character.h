#pragma once

#include "Obj.h"
#include "Script.h"
#include "Astar.h"

/////////////////////////////////
// 
//			CharacterManager
// 
/////////////////////////////////

inline int RequireExp(int Level) { return static_cast<int>(50 * pow(2, Level)); }
inline int MaxHp(int Level) { return Level * 100; }
inline int DefaultAttack(int Level) { return 25 + 25 * Level; }

struct Character : public DynamicObj, public Scriptable
{
	friend class CharacterManager;
public:
	virtual void Regen();
	Character(ID id) :DynamicObj{ id } { CompileScript(); };
	virtual ~Character() = default;
	virtual void HpDecrease(ID agent, int amount) {};
	virtual void HpIncrease(ID agent, int amount) {};
	void Attack(const vector<ID>& target, int damage);
	virtual void AttackImpl(const vector<ID>& target, int damage);
	virtual bool Move(Position diff) override;
	GET_REF(Hp);
	GET_REF(Level);
protected:
	virtual bool MoveForce(Position diff);
	SET(Hp);
	SET(Level);
protected:
	Position StartPosition_{};
	atomic_int Hp_{};
	atomic_int Level_{};
	milliseconds MovementCooltime_{ 1s };
	milliseconds AttackCooltime_{ 1s };
	atomic_bool Moveable_{ true };
	atomic_bool Attackable_{ true };
	unique_ptr<AstarPathFinder> Astar_;
};

class CharacterManager
{
	SINGLE_TON(CharacterManager);
public:
	bool Move(ID Id_, eMoveOper oper);
	bool Move(ID Id_, Position to);
	bool MoveForce(ID Id_, Position to);
	bool InitialMove(ID Id_, Position to);
	Position GetPosition(ID Id_) { return Characters_[Id_]->GetPos(); }
	void SetPosition(ID Id_, Position pos) { Characters_[Id_]->SetPos(pos); }
	void Enable(ID Id_) { Characters_[Id_]->Enable(); }
	void Disable(ID Id_) { Characters_[Id_]->Disable(); }
	void InitFromDataBase(ID id, DbCharacterID DbId);
	void ActivateSkill(ID id, eSkill skill);
	GET_REF_UNSAFE(Characters);
protected:
private:
	// PLAYER		[ 0, MAX_PLAYER )
	// MONSTER	[ MAX_PLAYER, MAX_PLAYER + MAX_MONSTER )
	// NPC		[ MAX_PLAYER + MAX_MONSTER, MAX_CHARACTER )
	array<unique_ptr<Character>, MAX_CHARACTER> Characters_;
	array<unique_ptr<DynamicObj>, MAX_OBSTACLE> Obstacles_;
};