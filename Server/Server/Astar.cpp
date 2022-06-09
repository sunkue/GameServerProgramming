#include "stdafx.h"
#include "Astar.h"
#include "World.h"

// Astar ������ Ŭ����

AstarPathFinder::Node::Node(Position _point, Node* _pParent, Position _EndPoint) : point{ _point }, pParent{ _pParent }, end{ _EndPoint }
{
	if (pParent == nullptr) // �θ� ���� ���
	{
		G = 0;
	}
	else if ( // ���� ������ ���
		(pParent->point.x == point.x - 1 && pParent->point.y == point.y) || // �θ� '��'���⿡ �ְų�
		(pParent->point.x == point.x + 1 && pParent->point.y == point.y) || // �θ� '��'���⿡ �ְų�
		(pParent->point.x == point.x && pParent->point.y == point.y - 1) || // �θ� '��'���⿡ �ְų�
		(pParent->point.x == point.x && pParent->point.y == point.y + 1)) // �θ� '��'���⿡ ������		
	{
		G = pParent->G + 10;
	}
	else if ( // �밢�� ������ ���
		(pParent->point.x == point.x - 1 && pParent->point.y == point.y - 1) || // �θ� '�»�'���⿡ �ְų�
		(pParent->point.x == point.x - 1 && pParent->point.y == point.y + 1) || // �θ� '���'���⿡ �ְų�
		(pParent->point.x == point.x + 1 && pParent->point.y == point.y - 1) || // �θ� '����'���⿡ �ְų�
		(pParent->point.x == point.x + 1 && pParent->point.y == point.y + 1)) // �θ� '����'���⿡ ������
	{
		G = pParent->G + 14;
	}
	else {
		cout << " �θ� ���� ���� " << endl;
		F = -100000;  H = -100000; G = -100000;
	}

	H = (abs(end.x - point.x) + abs(end.y - point.y)) * 10;

	F = G + H;
}

// �� ���� false=������ �� �ִ� �� , true=��ֹ�
void AstarPathFinder::Map::SetMap(Position _Center)
{
	for (auto& mm : map)
	{
		for (auto& m : mm)
			m = false;
	}
	Center = _Center;
	auto c = Position{ AstarPathFinder::Map::Nx / 2 , AstarPathFinder::Map::Ny / 2 };
	auto ns = World::Get().GetNearSectors4(Center);
	for (auto& s : ns)
	{
		for (auto o : s->GetObstacles())
		{
			if (!o->GetEnable()) continue;
			if (abs(o->GetPos().x - _Center.x) < c.x && abs(o->GetPos().y - _Center.y) < c.y)
			{
				map[o->GetPos().x - _Center.x + c.x][o->GetPos().y - _Center.y + c.y] = true;
			}
		}

		for (auto o : s->GetPlayers())
		{
			if (!o->GetEnable()) continue;
			if (abs(o->GetPos().x - _Center.x) < c.x && abs(o->GetPos().y - _Center.y) < c.y)
			{
				map[o->GetPos().x - _Center.x + c.x][o->GetPos().y - _Center.y + c.y] = true;
			}
		}

		for (auto o : s->GetMonsters())
		{
			if (!o->GetEnable()) continue;
			if (abs(o->GetPos().x - _Center.x) < c.x && abs(o->GetPos().y - _Center.y) < c.y)
			{
				map[o->GetPos().x - _Center.x + c.x][o->GetPos().y - _Center.y + c.y] = true;
			}
		}

		for (auto o : s->GetNpcs())
		{
			if (!o->GetEnable()) continue;
			if (abs(o->GetPos().x - _Center.x) < c.x && abs(o->GetPos().y - _Center.y) < c.y)
			{
				map[o->GetPos().x - _Center.x + c.x][o->GetPos().y - _Center.y + c.y] = true;
			}
		}
	}

	map[AstarPathFinder::Map::Nx / 2][AstarPathFinder::Map::Ny / 2] = false;

	for (int x = 0; x < AstarPathFinder::Map::Nx; x++)
	{
		for (int y = 0; y < AstarPathFinder::Map::Ny; y++)
		{
			if (map[x][y])
			{
				// cout << x + _Center.x - 6 << "  " << y + _Center.y - 6 << endl;
			}
		}
	}
}


// Astar ������ �Լ�
list<Position*> AstarPathFinder::FindPath(Map* Navi, Position StartPoint, Position EndPoint) // Map �� �����Ҵ��߱� ������ ���� ���� �� ����
{
	// (��,��,��,��) 4���� �ð���� Ž�� �� ����� ���� (���,����,����,�»�) Ž��.	
	list<Node*> OpenNode; // �������
	list<Node*> CloseNode; // �������
	Node* SNode; // ���õ� ���
	list<Position*> path;

	OpenNode.push_back(new Node(StartPoint, nullptr, EndPoint)); // ���������� ������忡 �߰�

	list<Node*>::iterator iter;

	// ���� ��尡 ��ų�(��������� ����==��) �������� ����(������忡�� ���� �߰�)�� ��� ��������
	// �� ������ �ݴ�� '���� ��忡 ������ �ְų� �������� �� ã�� ���' �ݺ�
	while ((OpenNode.end() != OpenNode.begin()) && (OpenNode.end() == FindCoordNode(EndPoint, &OpenNode)))
	{
		iter = FindNextNode(&OpenNode); // ������� �� F���� ���� ���� ����� �ּҸ� ã�Ƽ� iter �� ����
		SNode = *iter; // ������� �� F���� ���� ���� ��带 SNode�� ����

		// ���õ� SNode �ֺ��� 8���� ��� Ž��, ���� ������ �� �ִ� ���� ���� ��� ���̹Ƿ� ���� ���� �ּҸ� ����.
		ExploreNode(Navi, SNode, &OpenNode, &CloseNode, EndPoint);

		CloseNode.push_back(SNode); // ���� Ž���� ��带 ���� ��忡 �߰�
		OpenNode.erase(iter); // ���� ��忡 �߰��� ��带 ���� ��忡�� ����
	}

	if ((OpenNode.end() != OpenNode.begin())) // ���� ã�� ���
	{
		iter = FindCoordNode(EndPoint, &OpenNode); // �������� ��带 ã�Ƽ� iter�� ����
		for (SNode = *iter; SNode->pParent != nullptr; SNode = SNode->pParent)  // �θ� NULL�� ������ path�� ��� ����
		{
			path.push_back(new Position(SNode->point.x, SNode->point.y));
		}	path.push_back(new Position(SNode->point.x, SNode->point.y)); // �θ� NULL�� ����� path���� ����(��� ����)

		path.reverse(); // �������������� �������� �Է������Ƿ� �ٽ� �������� ������ ��������� ù ��°�� �ǵ��� ��.

		// ���� ã�� ��� �����Ҵ� ����
		iter = OpenNode.begin();
		for (; iter != OpenNode.end(); iter++)
		{
			delete* iter; // ���� ��� �����Ҵ� ����
		}
		iter = CloseNode.begin();
		for (; iter != CloseNode.end(); iter++)
		{
			delete* iter; // ���� ��� �����Ҵ� ����
		}

		return path; // ���� ã�� ��� ����
	}

	// ���� ã�� ���� ��� �����Ҵ� ����
	iter = CloseNode.begin();
	for (; iter != CloseNode.end(); iter++)
	{
		delete* iter; // ���� ��� �����Ҵ� ����
	}

	return path; // ���� ã�� ���� ��� ����

}

void AstarPathFinder::ExploreNode(Map* Navi, Node* SNode, list<Node*>* OpenNode, list<Node*>* CloseNode, Position EndPoint)
{
	bool up = true, right = true, down = true, left = true; // �� ����� ���� �밢�� ���� Ž�� ���θ� ����. true == ��ֹ� ����, false == ����
	list<Node*>::iterator iter;
	Position point;

	// '��' ���� Ž��
	point.x = SNode->point.x - 1;	point.y = SNode->point.y;
	if (SNode->point.x > 0 && Navi->map[point.x][point.y] == false) // '��' ���⿡ ���� �����ϰ� ��ֹ��� ���� ���
	{
		// ��ֹ��� ���� ��쿡 �ش��ϹǷ� ��ֹ� false ����
		up = false;

		// �̹� ���� ��忡 �ִ� ���
		if (OpenNode->end() != FindCoordNode(point, OpenNode))
		{
			iter = FindCoordNode(point, OpenNode);
			if ((*iter)->G > (SNode->G + 10)) // ���� �θ� ���ؼ� ���� ���� ��뺸�� ���� ��带 ���ؼ� ���� �� ����� �� ������ ��� 
			{
				(*iter)->pParent = SNode; // ���� ��带 �θ�� �ٲ�
			}
		}

		// ���� ��忡 �ִ� ���
		else if (CloseNode->end() != FindCoordNode(point, CloseNode))
		{
		}

		// ����⿡ ��ֹ��� ���� ���� ��� �� ���� ��忡 �߰��Ǿ����� ���� ���
		// ����� ��带 ���� ��忡 �߰�, �θ�� ���� Ž�� ���� ����.
		else
		{
			OpenNode->push_back(new Node(point, SNode, EndPoint));
		}
	}
	// '��' ���� Ž��
	point.x = SNode->point.x;	point.y = SNode->point.y + 1;
	if (SNode->point.y < (Map::Ny - 1) && Navi->map[point.x][point.y] == false) // '��' ���⿡ ���� �����ϰ� ��ֹ��� ���� ���
	{
		// ��ֹ��� ���� ��쿡 �ش��ϹǷ� ��ֹ� false ����
		right = false;

		// �̹� ���� ��忡 �ִ� ���
		if (OpenNode->end() != FindCoordNode(point, OpenNode))
		{
			iter = FindCoordNode(point, OpenNode);
			if ((*iter)->G > (SNode->G + 10)) // ���� �θ� ���ؼ� ���� ���� ��뺸�� ���� ��带 ���ؼ� ���� �� ����� �� ������ ��� 
			{
				(*iter)->pParent = SNode; // ���� ��带 �θ�� �ٲ�
			}
		}

		// ���� ��忡 �ִ� ���
		else if (CloseNode->end() != FindCoordNode(point, CloseNode))
		{
		}

		// ����⿡ ��ֹ��� ���� ���� ��� �� ���� ��忡 �߰��Ǿ����� ���� ���
		// ����� ��带 ���� ��忡 �߰�, �θ�� ���� Ž�� ���� ����.
		else
		{
			OpenNode->push_back(new Node(point, SNode, EndPoint));
		}
	}
	// '��' ���� Ž��
	point.x = SNode->point.x + 1;	point.y = SNode->point.y;
	if (SNode->point.x < (Map::Nx - 1) && Navi->map[point.x][point.y] == 0) // '��' ���⿡ ���� �����ϰ� ��ֹ��� ���� ���
	{
		// ��ֹ��� ���� ��쿡 �ش��ϹǷ� ��ֹ� false ����
		down = false;

		// �̹� ���� ��忡 �ִ� ���
		if (OpenNode->end() != FindCoordNode(point, OpenNode))
		{
			iter = FindCoordNode(point, OpenNode);
			if ((*iter)->G > (SNode->G + 10)) // ���� �θ� ���ؼ� ���� ���� ��뺸�� ���� ��带 ���ؼ� ���� �� ����� �� ������ ��� 
			{
				(*iter)->pParent = SNode; // ���� ��带 �θ�� �ٲ�
			}
		}

		// ���� ��忡 �ִ� ���
		else if (CloseNode->end() != FindCoordNode(point, CloseNode))
		{
		}

		// ����⿡ ��ֹ��� ���� ���� ��� �� ���� ��忡 �߰��Ǿ����� ���� ���
		// ����� ��带 ���� ��忡 �߰�, �θ�� ���� Ž�� ���� ����.
		else
		{
			OpenNode->push_back(new Node(point, SNode, EndPoint));
		}
	}
	// '��' ���� Ž��
	point.x = SNode->point.x;	point.y = SNode->point.y - 1;
	if (SNode->point.y > 0 && Navi->map[point.x][point.y] == 0) // '��' ���⿡ ���� �����ϰ� ��ֹ��� ���� ���
	{
		// ��ֹ��� ���� ��쿡 �ش��ϹǷ� ��ֹ� false ����
		left = false;

		// �̹� ���� ��忡 �ִ� ���
		if (OpenNode->end() != FindCoordNode(point, OpenNode))
		{
			iter = FindCoordNode(point, OpenNode);
			if ((*iter)->G > (SNode->G + 10)) // ���� �θ� ���ؼ� ���� ���� ��뺸�� ���� ��带 ���ؼ� ���� �� ����� �� ������ ��� 
			{
				(*iter)->pParent = SNode; // ���� ��带 �θ�� �ٲ�
			}
		}

		// ���� ��忡 �ִ� ���
		else if (CloseNode->end() != FindCoordNode(point, CloseNode))
		{
		}

		// ����⿡ ��ֹ��� ���� ���� ��� �� ���� ��忡 �߰��Ǿ����� ���� ���
		// ����� ��带 ���� ��忡 �߰�, �θ�� ���� Ž�� ���� ����.
		else
		{
			OpenNode->push_back(new Node(point, SNode, EndPoint));
		}
	}

	// '���' ���� Ž��
	point.x = SNode->point.x - 1;	point.y = SNode->point.y + 1;
	if (SNode->point.x > 0 && SNode->point.y < (Map::Ny - 1) && Navi->map[point.x][point.y] == 0 &&
		up == false && right == false) // '���' ���⿡ ���� �����ϰ� ��ֹ��� ������, ������ ����⿡�� ��ֹ��� ���� ���
	{
		// �̹� ���� ��忡 �ִ� ���
		if (OpenNode->end() != FindCoordNode(point, OpenNode))
		{
			iter = FindCoordNode(point, OpenNode);
			if ((*iter)->G > (SNode->G + 14)) // ���� �θ� ���ؼ� ���� ���� ��뺸�� ���� ��带 ���ؼ� ���� �� ����� �� ������ ��� 
			{
				(*iter)->pParent = SNode; // ���� ��带 �θ�� �ٲ�
			}
		}

		// ���� ��忡 �ִ� ���
		else if (CloseNode->end() != FindCoordNode(point, CloseNode))
		{
		}

		// ����⿡ ��ֹ��� ���� ���� ��� �� ���� ��忡 �߰��Ǿ����� ���� ���
		// ����� ��带 ���� ��忡 �߰�, �θ�� ���� Ž�� ���� ����.
		else
		{
			OpenNode->push_back(new Node(point, SNode, EndPoint));
		}
	}
	// '����' ���� Ž��
	point.x = SNode->point.x + 1;	point.y = SNode->point.y + 1;
	if (SNode->point.x < (Map::Nx - 1) && SNode->point.y < (Map::Ny - 1) &&
		Navi->map[point.x][point.y] == 0 && right == false && down == false)
		// '����' ���⿡ ���� �����ϰ� ��ֹ��� ������, ������ �Ϲ��⿡�� ��ֹ��� ���� ���
	{
		// �̹� ���� ��忡 �ִ� ���
		if (OpenNode->end() != FindCoordNode(point, OpenNode))
		{
			iter = FindCoordNode(point, OpenNode);
			if ((*iter)->G > (SNode->G + 14)) // ���� �θ� ���ؼ� ���� ���� ��뺸�� ���� ��带 ���ؼ� ���� �� ����� �� ������ ��� 
			{
				(*iter)->pParent = SNode; // ���� ��带 �θ�� �ٲ�
			}
		}

		// ���� ��忡 �ִ� ���
		else if (CloseNode->end() != FindCoordNode(point, CloseNode))
		{
		}

		// ����⿡ ��ֹ��� ���� ���� ��� �� ���� ��忡 �߰��Ǿ����� ���� ���
		// ����� ��带 ���� ��忡 �߰�, �θ�� ���� Ž�� ���� ����.
		else
		{
			OpenNode->push_back(new Node(point, SNode, EndPoint));
		}
	}
	// '����' ���� Ž��
	point.x = SNode->point.x + 1;	point.y = SNode->point.y - 1;
	if (SNode->point.x < (Map::Nx - 1) && SNode->point.y > 0 && Navi->map[point.x][point.y] == 0 &&
		left == false && down == false) // '����' ���⿡ ���� �����ϰ� ��ֹ��� ������, �¹���� �Ϲ��⿡�� ��ֹ��� ���� ���
	{
		// �̹� ���� ��忡 �ִ� ���
		if (OpenNode->end() != FindCoordNode(point, OpenNode))
		{
			iter = FindCoordNode(point, OpenNode);
			if ((*iter)->G > (SNode->G + 14)) // ���� �θ� ���ؼ� ���� ���� ��뺸�� ���� ��带 ���ؼ� ���� �� ����� �� ������ ��� 
			{
				(*iter)->pParent = SNode; // ���� ��带 �θ�� �ٲ�
			}
		}

		// ���� ��忡 �ִ� ���
		else if (CloseNode->end() != FindCoordNode(point, CloseNode))
		{
		}

		// ����⿡ ��ֹ��� ���� ���� ��� �� ���� ��忡 �߰��Ǿ����� ���� ���
		// ����� ��带 ���� ��忡 �߰�, �θ�� ���� Ž�� ���� ����.
		else
		{
			OpenNode->push_back(new Node(point, SNode, EndPoint));
		}
	}
	// '�»�' ���� Ž��
	point.x = SNode->point.x - 1;	point.y = SNode->point.y - 1;
	if (SNode->point.x > 0 && SNode->point.y > 0 && Navi->map[point.x][point.y] == 0 &&
		left == false && up == false) // '�»�' ���⿡ ���� �����ϰ� ��ֹ��� ������, �¹���� ����⿡�� ��ֹ��� ���� ���
	{
		// �̹� ���� ��忡 �ִ� ���
		if (OpenNode->end() != FindCoordNode(point, OpenNode))
		{
			iter = FindCoordNode(point, OpenNode);
			if ((*iter)->G > (SNode->G + 14)) // ���� �θ� ���ؼ� ���� ���� ��뺸�� ���� ��带 ���ؼ� ���� �� ����� �� ������ ��� 
			{
				(*iter)->pParent = SNode; // ���� ��带 �θ�� �ٲ�
			}
		}

		// ���� ��忡 �ִ� ���
		else if (CloseNode->end() != FindCoordNode(point, CloseNode))
		{
		}

		// ����⿡ ��ֹ��� ���� ���� ��� �� ���� ��忡 �߰��Ǿ����� ���� ���
		// ����� ��带 ���� ��忡 �߰�, �θ�� ���� Ž�� ���� ����.
		else
		{
			OpenNode->push_back(new Node(point, SNode, EndPoint));
		}
	}
}

list<AstarPathFinder::Node*>::iterator AstarPathFinder::FindNextNode(list<AstarPathFinder::Node*>* pOpenNode) // ���³�� �� F���� ���� ���� ��� ã�Ƽ� ��ȯ
{
	list<Node*>::iterator iter = (*pOpenNode).begin();

	int minValue = 2000000000; // ���� ���� ���� ���� ����
	int order = 0; // ���� ���� ���� �� ��°����

	for (int i = 1; iter != (*pOpenNode).end(); i++, iter++)
	{
		if ((*iter)->F <= minValue) // F���� �۰ų� ���� ��츦 �߰��ϸ�(�̷��� ���� �� F���� ���� ��� �ֱٿ� �߰��� ���� �켱)
		{
			minValue = (*iter)->F;
			order = i;
		}
	}

	iter = (*pOpenNode).begin();
	for (int i = 1; i < order; i++)
	{
		iter++;
	}

	return iter;
}

list<AstarPathFinder::Node*>::iterator AstarPathFinder::FindCoordNode(Position _point, list<AstarPathFinder::Node*>* NodeList) // ��帮��Ʈ���� x,y ��ǥ�� ��带 ã�Ƽ� �ּҸ� ��ȯ. ������ end()��ȯ.
{
	list<AstarPathFinder::Node*>::iterator iter = NodeList->begin();

	for (int i = 1; iter != NodeList->end(); i++, iter++)
	{
		if ((*iter)->point.x == _point.x && (*iter)->point.y == _point.y)
		{
			return iter;
		}
	}

	return NodeList->end();
}

void AstarPathFinder::FindPath()
{
	path = FindPath(&Navi, StartPoint, EndPoint);
}

Position AstarPathFinder::GetWorldPos(int order)
{
	if (path.size() - 1 < order) return Position{ -1 };
	Position pos = GetMapPos(order);
	pos.x -= Navi.Nx / 2;
	pos.y -= Navi.Ny / 2;
	pos = Navi.Center + pos;
	return pos;
}

Position AstarPathFinder::GetMapPos(int order)
{
	if (path.size() - 1 < order) return Position{ -1 };
	Position pos;
	auto p = path.begin();
	for (int i = 0; i < order; i++)
		p++;
	pos = **p;
	return pos;
}

void AstarPathFinder::SetFree(int _x, int _y)
{
	Navi.map[_x][_y] = false;
}

void AstarPathFinder::SetObstacle(int _x, int _y)
{
	Navi.map[_x][_y] = true;
}

void AstarPathFinder::clear()
{
	for (auto& p : path)
	{
		delete p;
	}
	path.clear();
}

