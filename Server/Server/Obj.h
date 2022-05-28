#pragma once

inline bool IsLimitPosition(Position::value_type N)
{
	return N <= 0 || MAP_SIZE - 1 <= N;
}

inline bool IsOverFlowedPosition(Position::value_type N)
{
	return N < 0 || MAP_SIZE <= N;
}

inline bool IsLimitPosition(Position pos)
{
	return IsLimitPosition(pos.x) || IsLimitPosition(pos.y);
}

inline bool IsOverFlowedPosition(Position pos)
{
	return IsOverFlowedPosition(pos.x) || IsOverFlowedPosition(pos.y);
}

inline bool IsLimitSectorIdxMin(Position::value_type N)
{
	return N <= 0;
}

inline bool IsLimitSectorIdxMax(Position::value_type N)
{
	return SECTOR_NUM - 1 <= N;
}

inline bool IsOverFlowedSectorIdxMin(Position::value_type N)
{
	return N < 0;
}

inline bool IsOverFlowedSectorIdxMax(Position::value_type N)
{
	return SECTOR_NUM <= N;
}

inline bool IsLimitSectorIdx(Position::value_type N)
{
	return IsLimitSectorIdxMin(N) || IsLimitSectorIdxMax(N);
}

inline bool IsOverFlowedSectorIdx(Position::value_type N)
{
	return IsOverFlowedSectorIdxMin(N) || IsOverFlowedSectorIdxMax(N);
}

inline bool IsLimitSectorIdx(Position sectorIdx)
{
	return IsLimitPosition(sectorIdx.x) || IsLimitPosition(sectorIdx.y);
}

inline bool IsOverFlowedSectorIdx(Position sectorIdx)
{
	return IsOverFlowedPosition(sectorIdx.x) || IsOverFlowedPosition(sectorIdx.y);
}

class StaticObj
{
public:
	StaticObj() {}
	virtual ~StaticObj() = default;
	bool IsInSightAndEnable(Position target);
	bool IsInSight(Position target);
	GET(Pos);
	SET(Pos);
	GET(SightRange);
	GET(SectorIdx);
	SET(SectorIdx);
	GET_REF(Enable);
protected:
	atomic_bool Enable_{};
private:
	Position Pos_{};
	int SightRange_{ SIGHT_SIZE };
	Position SectorIdx_{};
};

inline int RequireExp(int HP) { return static_cast<int>(50 * pow(2, HP)); }
inline int MaxHP(int Level) { return Level * 100; }

class DynamicObj : public StaticObj
{
	friend class CharacterManager;
public:
	DynamicObj(ID id) : Id_{ id } {};
	virtual ~DynamicObj() = default;
	virtual bool Move(Position diff);
	virtual void Update();
	virtual bool Enable();
	virtual bool Disable();
private:

public:
	GET(Id);
	GET_REF(Hp);
	GET_REF(Level);
protected:
	atomic_int Hp_{};
	atomic_int Level_{};
	ID Id_{ -1 };
protected:
	shared_mutex PositionLock;
private:
};
