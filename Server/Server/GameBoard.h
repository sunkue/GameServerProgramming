#pragma once

class GameBoard
{
	SINGLE_TON(GameBoard) = default;
public:
	GET(n);
private:
	int n_{ 8 };
};

