#include "QuadTreeGrid.h"

QuadGrid::QuadGrid()
{
}

QuadGrid::~QuadGrid()
{
}

//void QuadGrid::BuildTree(unsigned int maxLevel, unsigned int worldSize, const sf::Vector2f & startPos)
//{
//	m_quadTree.clear();
//	m_maximumLevel = maxLevel;
//	m_worldSize = worldSize;
//
//	size_t numberOfQuadrants = 0;
//
//	for (unsigned int level = 0; level <= maxLevel; level++)
//	{
//		numberOfQuadrants += (unsigned int)std::pow(4, level);
//	}
//
//	m_quadTree = std::vector<Quadrant>(numberOfQuadrants);
//
//	Quadrant q;
//	q.Build(startPos, sf::Vector2f(worldSize, worldSize), 0, maxLevel, m_quadTree, 0);
//}

void QuadGrid::BuildTree(unsigned int maxLevel, unsigned int worldSize, const sf::Vector2f & startPos)
{
	m_quadTree.clear();
	m_maximumLevel = maxLevel;
	m_worldSize = worldSize;

	size_t numberOfQuadrants = 0;

	for (unsigned int level = 0; level <= maxLevel; level++)
	{
		numberOfQuadrants += (unsigned int)std::pow(4, level);
	}
	m_quadTree = std::vector<Quadrant>(numberOfQuadrants);

	sf::Vector2f size(worldSize, worldSize);

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
			currentPos.y += size.y * y;

			for (unsigned int x = 0; x < rowCol; x++)
			{
				currentPos.x = startPos.x + size.x * x;
				q.Create(currentPos, size, level, isLeaf);
				m_quadTree[index++] = q;
			}
		}
	}
}

bool QuadGrid::GetQuadrant(const sf::Vector2f & worldPos, unsigned int level, Quadrant * outQuadrant)
{
	if (m_quadTree.empty())
		return false;

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
	sf::Vector2f worldEnd	= m_quadTree[0].GetMax();

	searchPos.x =	searchPos.x < worldStart.x	? worldStart.x	: searchPos.x;
	searchPos.y =	searchPos.y < worldStart.y	? worldStart.y	: searchPos.y;
	searchPos.x =	searchPos.x > worldEnd.x	? worldEnd.x	: searchPos.x;
	searchPos.y =	searchPos.y > worldEnd.y	? worldEnd.y	: searchPos.y;

	searchPos -= worldStart;
	
	sf::Vector2f step = searchPos / size;

	size_t quadrantIndex = levelStartIndex + (int)step.x + (int)step.y * (int)std::pow(2, level);

	outQuadrant = &m_quadTree[quadrantIndex];

	return true;
}

bool QuadGrid::GetQuadrantFrom(const Quadrant & refQuadrant, Direction dir, Quadrant * outQuadrant)
{
	bool valid = false;
	
	sf::Vector2f searchPos = refQuadrant.GetMin();

	float size = refQuadrant.GetSize() * 1.5f;

	switch (dir)
	{
	case North:
		searchPos.y - size;
		break;
	case North_East:
		searchPos.y - size;
		searchPos.x + size;
		break;
	case East:
		searchPos.x + size;
		break;
	case South_East:
		searchPos.y + size;
		searchPos.x + size;
		break;
	case South:
		searchPos.y + size;
		break;
	case South_West:
		searchPos.y + size;
		searchPos.x - size;
		break;
	case West:
		searchPos.x - size;
		break;
	case North_West:
		searchPos.y - size;
		searchPos.x - size;
		break;
	}

	valid = GetQuadrant(searchPos, refQuadrant.GetLevel(), outQuadrant);

	return valid;
}

std::string QuadGrid::ToString() const
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

