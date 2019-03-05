#include "QuadTree.h"

QuadTree::QuadTree()
{

}

QuadTree::~QuadTree()
{
}

void QuadTree::BuildTree(unsigned int maxLevel, unsigned int worldSize, const sf::Vector2f & startPos)
{
	m_quadTree.clear();
	m_leafs.clear();
	m_maximumLevel = maxLevel;
	m_worldSize = worldSize;

	size_t numberOfQuadrants = 0;

	for (unsigned int level = 0; level <= maxLevel; level++)
	{
		numberOfQuadrants += (unsigned int)std::pow(4, level);
	}
	m_quadTree = std::vector<Quadrant>(numberOfQuadrants);

	float size = worldSize;

	Quadrant q;
	q.Create(startPos, size, 0, false);
	size_t index = 0;
	m_quadTree[index++] = q;

	for (unsigned int level = 1; level <= maxLevel; level++)
	{
		bool isLeaf = level == maxLevel;
		float rowCol = (unsigned int)std::pow(2, level);
		size = size * 0.5f;

		for (unsigned int y = 0; y < rowCol; y++)
		{
			sf::Vector2f currentPos = startPos;
			currentPos.y += size * y;

			for (unsigned int x = 0; x < rowCol; x++)
			{
				currentPos.x = startPos.x + size * x;
				q.Create(currentPos, size, level, isLeaf);
				size_t parent = _GetQuadrantIndex(currentPos, level - 1);				
				m_quadTree[index] = q;
				if (isLeaf)
					m_leafs.push_back(&m_quadTree[index]);
				m_quadTree[index].SetParent(parent);
				m_quadTree[parent].AddChild(index++);
			}
		}
	}
}

void QuadTree::BuildTree(unsigned int maxLevel, unsigned int worldSize, float worldStartX, float worldStartY)
{
	BuildTree(maxLevel, worldSize, sf::Vector2f(worldStartX, worldStartY));
}

void QuadTree::PlaceObjects(std::vector<Entity> & objectVector)
{
	for (auto & q : m_leafs)
		q->ClearObjects();

	int size = objectVector.size();

	for (int i = 0; i < size; i++)
		_traverseAndPlace(&objectVector[i], 0);



}

Quadrant * QuadTree::GetQuadrant(const sf::Vector2f & worldPos, unsigned int level)
{
	if (m_quadTree.empty())
		return nullptr;

	size_t quadrantIndex = _GetQuadrantIndex(worldPos, level);

	return &m_quadTree[quadrantIndex];;
}

Quadrant * QuadTree::GetQuadrantFrom(Quadrant * refQuadrant, Direction dir)
{
	sf::Vector2f searchPos = refQuadrant->GetMin();

	float size = refQuadrant->GetSize() * 1.5f;

	switch (dir)
	{
	case North:
		searchPos.y -= size;
		break;
	case North_East:
		searchPos.y -= size;
		searchPos.x += size;
		break;
	case East:
		searchPos.x += size;
		break;
	case South_East:
		searchPos.y += size;
		searchPos.x += size;
		break;
	case South:
		searchPos.y += size;
		break;
	case South_West:
		searchPos.y += size;
		searchPos.x -= size;
		break;
	case West:
		searchPos.x -= size;
		break;
	case North_West:
		searchPos.y -= size;
		searchPos.x -= size;
		break;

	}

	return GetQuadrant(searchPos, refQuadrant->GetLevel());
}

unsigned int QuadTree::GetWorldSize() const
{
	return m_maximumLevel;
}

unsigned int QuadTree::GetMaxTreeLevel() const
{
	return m_maximumLevel;
}

const std::vector<Quadrant>& QuadTree::GetQuadrantVector() const
{
	return m_quadTree;
}

Entity * QuadTree::DispatchRay(const sf::Vector2f & rayStart, const sf::Vector2f & rayEnd) const
{
	Entity * e = nullptr;
	float t = FLT_MAX;
	_traverseWithRay(rayStart, rayEnd, 0, t, e);
	return e;
}

Entity * QuadTree::PointInsideObject(const sf::Vector2f & point) const
{
	Entity * e = nullptr;

	_pointTraverse(point, 0, e);

	return e;
}

const Quadrant & QuadTree::operator[](unsigned int index)
{
	return m_quadTree[index];
}

std::string QuadTree::ToString() const
{
	std::string str = "";

	str += "World size: " + std::to_string(m_worldSize) + "\n";
	str += "Maximum level: " + std::to_string(m_maximumLevel) + "\n\n";
	str += "############### QUAD TREE BEGIN ###############\n\n";

	for (int i = 0; i < m_quadTree.size(); i++)
	{
		str += "Index: " + std::to_string(i) + "\n";
		str += m_quadTree[i].ToString() + "\n";
	}

	return str;
}

size_t QuadTree::_GetQuadrantIndex(const sf::Vector2f & worldPos, unsigned int level)
{
	level = level > m_maximumLevel ? m_maximumLevel : level;

	size_t levelStartIndex = 0;
	size_t levelEndIndex = 0;

	for (unsigned int i = 0; i < level; i++)
	{
		levelStartIndex += (unsigned int)std::pow(4, i);
	}

	levelEndIndex = levelStartIndex + (unsigned int)std::pow(4, level);

	float size = m_quadTree[levelStartIndex].GetSize();
	sf::Vector2f searchPos = worldPos;

	sf::Vector2f worldStart = m_quadTree[0].GetMin();
	sf::Vector2f worldEnd = m_quadTree[0].GetMax();

	searchPos.x = searchPos.x < worldStart.x ? worldStart.x : searchPos.x;
	searchPos.y = searchPos.y < worldStart.y ? worldStart.y : searchPos.y;
	searchPos.x = searchPos.x > worldEnd.x ? worldEnd.x : searchPos.x;
	searchPos.y = searchPos.y > worldEnd.y ? worldEnd.y : searchPos.y;

	searchPos -= worldStart;

	sf::Vector2f step = searchPos / size;

	return levelStartIndex + (int)step.x + (int)step.y * (int)std::pow(2, level);
}

void QuadTree::_traverseAndPlace(Entity * e, int quadIndex)
{
	if (_insideAABB(e->GetPosition(), e->GetSize(), m_quadTree[quadIndex]))
	{
		int nrOfChildren = m_quadTree[quadIndex].GetNrOfChildren();

		if (nrOfChildren > 0)
		{
			const unsigned int * children = m_quadTree[quadIndex].GetChildren();
			for (int i = 0; i < nrOfChildren; i++)
				_traverseAndPlace(e, children[i]);
		}
		else
		{
			m_quadTree[quadIndex].SetObject(e);
		}
	}
}

void QuadTree::_pointTraverse(const sf::Vector2f & point, int quadIndex, Entity *& ePtr) const
{
	if (ePtr == nullptr)
	{
		if (_insideAABB(point, m_quadTree[quadIndex]))
		{
			int nrOfChildren = m_quadTree[quadIndex].GetNrOfChildren();

			if (nrOfChildren > 0)
			{
				const unsigned int * children = m_quadTree[quadIndex].GetChildren();
				for (int i = 0; i < nrOfChildren; i++)
					_pointTraverse(point, children[i], ePtr);
			}
			else
			{
				// Trace Objects
				const std::vector<Entity*> & objects = m_quadTree[quadIndex].GetObjects();

				size_t size = objects.size();
				float tTemp = FLT_MAX;

				for (int i = 0; i < size; i++)
				{
					sf::FloatRect fr;
					fr.left = objects[i]->GetPosition().x;
					fr.top = objects[i]->GetPosition().y;
					fr.width = objects[i]->GetSize().x;
					fr.height = objects[i]->GetSize().y;

					if (fr.contains(point))
					{
						ePtr = objects[i];
					}
				}
			}
		}
	}
}

void QuadTree::_traverseWithRay(const sf::Vector2f & rayStart, const sf::Vector2f & rayEnd, int quadIndex, float & t, Entity *& ePtr) const
{
	if (t > 0.0f)
	{
		if (_insideRay(rayStart, rayEnd, m_quadTree[quadIndex]))
		{
			int nrOfChildren = m_quadTree[quadIndex].GetNrOfChildren();

			if (nrOfChildren > 0)
			{
				const unsigned int * children = m_quadTree[quadIndex].GetChildren();
				for (int i = 0; i < nrOfChildren; i++)
					_traverseWithRay(rayStart, rayEnd, children[i], t, ePtr);
			}
			else
			{
				// Trace Objects
				const std::vector<Entity*> & objects = m_quadTree[quadIndex].GetObjects();

				size_t size = objects.size();
				float tTemp = FLT_MAX;

				for (int i = 0; i < size; i++)
				{
					if (_insideRay(rayStart, rayEnd, objects[i], tTemp) && tTemp < t)
					{
						ePtr = objects[i];
						t = tTemp;
					}
				}
			}
		}
	}
}

bool QuadTree::_lineWithLineIntersection(const sf::Vector2f & lineOrigin1, const sf::Vector2f & lineEnd1, const sf::Vector2f & lineOrigin2, const sf::Vector2f & lineEnd2, float & t) const
{
	static const float EPSILON = 0.0001f;

	sf::Vector2f b = lineEnd1 - lineOrigin1;
	sf::Vector2f d = lineEnd2 - lineOrigin2;

	float bDotDPerp = b.x * d.y - b.y * d.x;

	if (fabs(bDotDPerp) < EPSILON)
		return false;

	sf::Vector2f c = lineOrigin2 - lineOrigin1;

	float tTemp = (c.x * d.y - c.y * d.x) / bDotDPerp;

	if (tTemp < 0.0f || tTemp > 1.0f)
		return false;

	float u = (c.x * b.y - c.y * b.x) / bDotDPerp;

	if (u < 0.0f || u > 1.0f)
		return false;


	t = tTemp;

	return true;
}

bool QuadTree::_insideRay(const sf::Vector2f & rayStart, const sf::Vector2f & rayEnd, const Quadrant & quadrant) const
{
	if (_insideAABB(rayStart, rayEnd - rayStart, quadrant))
		return true;

	return false;
}

bool QuadTree::_insideRay(const sf::Vector2f & rayStart, const sf::Vector2f & rayEnd, const Entity * e, float & t) const
{

	sf::Vector2f points[4];
	points[0] = e->GetPosition();
	points[1] = e->GetPosition() + sf::Vector2f(e->GetSize().x, 0);
	points[2] = e->GetPosition() + e->GetSize();
	points[3] = e->GetPosition() + sf::Vector2f(0, e->GetSize().y);
	
	if (rayStart.x > points[0].x && rayStart.x < points[2].x
		&&
		rayStart.y > points[0].y && rayStart.y < points[2].y)
	{
		t = 0.0f;
		return true;
	}

	float tTemp = 0.0f;
	bool returnVal = false;
	for (int i = 0; i < 4; i++)
	{
		sf::Vector2f edgeStart = points[i];
		sf::Vector2f edgeEnd = points[(i + 1) % 4];

		if (_lineWithLineIntersection(rayStart, rayEnd, edgeStart, edgeEnd, tTemp) && tTemp < t)
		{
			t = tTemp;
			returnVal = true;
		}
	}

	return returnVal;
}

bool QuadTree::_insideAABB(const sf::Vector2f & min, const sf::Vector2f & size, const Quadrant & quadrant) const
{
	const sf::Vector2f & qMin = quadrant.GetMin();
	float qSize = quadrant.GetSize();

	sf::FloatRect a1, a2;

	a1.left		= min.x;
	a1.top		= min.y;
	a1.width	= size.x;
	a1.height	= size.y;

	if (a1.width == 0.0f)
		a1.width = 0.001f;
	if (a1.height == 0.0f)
		a1.height = 0.001f;

	a2.left		= qMin.x;
	a2.top		= qMin.y;
	a2.width	= qSize;
	a2.height	= qSize;

	return a2.intersects(a1) || a2.contains(min);
}

bool QuadTree::_insideAABB(const sf::Vector2f & min, const Quadrant & quadrant) const
{
	sf::FloatRect a1;
	const sf::Vector2f & qMin = quadrant.GetMin();
	float qSize = quadrant.GetSize();
	
	a1.left = qMin.x;
	a1.top = qMin.y;
	a1.width = qSize;
	a1.height = qSize;

	return a1.contains(min);
}

