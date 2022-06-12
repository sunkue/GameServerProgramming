#pragma once

class AstarPathFinder
{
	THREAD_LOCAL_SINGLE_TON(AstarPathFinder) = default;
public:
	class Node // 노드 클래스
	{
	public:
		Position point;
		int F, G, H; // F = 비용, G = 지난 거리, H = 남은 거리
		Position end;
		Node* pParent{};

	public:
		Node(Position _point, Node* _pParent, Position _EndPoint);
	};

	class Map // 맵 클래스
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

private: // 내부 함수
	list<Position*> FindPath(Map* Navi, Position StartPoint, Position EndPoint);
	list<Node*>::iterator FindNextNode(list<Node*>* pOpenNode); // 오픈노드 중 F값이 제일 작은 노드 찾아서 반환
	list<Node*>::iterator FindCoordNode(Position _point, list<Node*>* NodeList); // 노드리스트에서 x,y 좌표의 노드를 찾아서 주소를 반환. 없으면 end()반환.
	void ExploreNode(Map* Navi, Node* SNode, list<Node*>* OpenNode, list<Node*>* CloseNode, Position EndPoint); // 8방향 노드를 탐색하고 열린 노드에 추가 및 부모 변경을 실행함

public:
	void FindPath();
	Position GetWorldPos(int order); // order번째 경로의 좌표를 받아옴
	Position GetMapPos(int order); // order번째 경로의 좌표를 받아옴
	list<Position*> GetPath() { return path; } // 경로를 Coordinate* 리스트로 통째로 받아옴
	void SetFree(int _x, int _y); // 해당 좌표의 장애물을 없앰
	void SetObstacle(int _x, int _y); // 해당 좌표에 장애물을 설치

private:
	Map Navi; // 맵 생성

private:
	Position StartPoint; // 출발지점
	Position EndPoint; // 목표지점
	list<Position*> path; // 경로

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

