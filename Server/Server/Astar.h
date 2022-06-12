#pragma once

class AstarPathFinder
{
	THREAD_LOCAL_SINGLE_TON(AstarPathFinder) = default;
public:
	class Node // ��� Ŭ����
	{
	public:
		Position point;
		int F, G, H; // F = ���, G = ���� �Ÿ�, H = ���� �Ÿ�
		Position end;
		Node* pParent{};

	public:
		Node(Position _point, Node* _pParent, Position _EndPoint);
	};

	class Map // �� Ŭ����
	{
	public:
		static constexpr int Nx = 13; //odd
		static constexpr int Ny = 13; //odd
		static_assert(Nx % 2 == 1 && Ny % 2 == 1);
		void SetMap(Position Center);
	public:
		array<array<bool, Ny>, Nx> map{ false };
		Position Center{};
	};

private: // ���� �Լ�
	list<Position*> FindPath(Map* Navi, Position StartPoint, Position EndPoint);
	list<Node*>::iterator FindNextNode(list<Node*>* pOpenNode); // ���³�� �� F���� ���� ���� ��� ã�Ƽ� ��ȯ
	list<Node*>::iterator FindCoordNode(Position _point, list<Node*>* NodeList); // ��帮��Ʈ���� x,y ��ǥ�� ��带 ã�Ƽ� �ּҸ� ��ȯ. ������ end()��ȯ.
	void ExploreNode(Map* Navi, Node* SNode, list<Node*>* OpenNode, list<Node*>* CloseNode, Position EndPoint); // 8���� ��带 Ž���ϰ� ���� ��忡 �߰� �� �θ� ������ ������

public:
	void FindPath();
	Position GetWorldPos(int order); // order��° ����� ��ǥ�� �޾ƿ�
	Position GetMapPos(int order); // order��° ����� ��ǥ�� �޾ƿ�
	list<Position*> GetPath() { return path; } // ��θ� Coordinate* ����Ʈ�� ��°�� �޾ƿ�
	void SetFree(int _x, int _y); // �ش� ��ǥ�� ��ֹ��� ����
	void SetObstacle(int _x, int _y); // �ش� ��ǥ�� ��ֹ��� ��ġ

private:
	Map Navi; // �� ����

private:
	Position StartPoint; // �������
	Position EndPoint; // ��ǥ����
	list<Position*> path; // ���

public:
	void InitAstar(Position _StartPoint, Position _EndPoint)
	{
		clear();
		Navi.SetMap(_StartPoint);
		StartPoint = { Navi.Nx / 2, Navi.Ny / 2 };
		EndPoint = Position{ Navi.Nx / 2, Navi.Ny / 2 } + _EndPoint - _StartPoint;
		FindPath();
	}

	void clear();

	~AstarPathFinder()
	{
		clear();
	}
};

