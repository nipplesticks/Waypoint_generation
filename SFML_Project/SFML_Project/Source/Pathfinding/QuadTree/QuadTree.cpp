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

void QuadTree::PlaceObjects(const std::vector<Entity> & objectVector)
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

void QuadTree::_traverseAndPlace(const Entity * e, int quadIndex)
{
	if (_inside(e->GetPosition(), e->GetSize(), m_quadTree[quadIndex]))
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

bool QuadTree::_inside(const sf::Vector2f & min, const sf::Vector2f & size, const Quadrant & quadrant)
{
	const sf::Vector2f & qMin = quadrant.GetMin();
	float qSize = quadrant.GetSize();


	sf::FloatRect a1, a2;

	a1.left		= min.x;
	a1.top		= min.y;
	a1.width	= size.x;
	a1.height	= size.y;

	a2.left		= qMin.x;
	a2.top		= qMin.y;
	a2.width	= qSize;
	a2.height	= qSize;

	/*if (
		(min.x > qMin.x && min.y > qMin.y && min.x < qMax.x && min.y < qMax.y)
		||
		(max.x > qMin.x && max.y > qMin.y && max.x < qMax.x && max.y < qMax.y)
		||
		(min.x > qMin.x && max.y > qMin.y && min.x < qMax.x && max.y < qMax.y)
		||
		(max.x > qMin.x && min.y > qMin.y && max.x < qMax.x && min.y < qMax.y)
		)
	{
		return true;
	}*/

	return a1.intersects(a2);
}

