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
	GET(pos);
	SET(pos);
	GET(sector);
	GET(sight);
	SET(sector);
	GET_REF(enable);
protected:
	atomic_bool enable_{};
private:
	Position pos_{};
	int sight_{ SIGHT_SIZE };
	Position sector_{};
};

class DynamicObj : public StaticObj
{
public:
	bool Move(Position diff);
	virtual ~DynamicObj() = default;
private:

public:

protected:
	shared_mutex positionLock;
private:
};
