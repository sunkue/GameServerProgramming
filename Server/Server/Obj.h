#pragma once

class StaticObj
{
public:
	GET(pos);
	SET(pos);
	SET(enable);
	GET_REF(enable);
protected:
	Position pos_{};
	atomic_bool enable_{};
	Position sector_{};
};

class DynamicObj : public StaticObj
{
public:
	bool Move(Position diff);
private:

public:

protected:
	shared_mutex positionLock;
private:
};
