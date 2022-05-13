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

inline bool IsLimitSectorIdx(Position::value_type N)
{
	return N <= 0 || SECTOR_NUM - 1 <= N;
}

inline bool IsOverFlowedSectorIdx(Position::value_type N)
{
	return N < 0 || SECTOR_NUM <= N;
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


class DynamicObj : public StaticObj
{
public:
	bool Move(Position diff);
	virtual ~DynamicObj() = default;
private:

public:
	GET_REF(Hp);
	GET_REF(Level);
protected:
	atomic_int Hp_{};
	atomic_int Level_{};
protected:
	shared_mutex PositionLock;
private:
};
