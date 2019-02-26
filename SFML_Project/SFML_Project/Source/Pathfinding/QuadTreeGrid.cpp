#include "QuadTreeGrid.h"

QuadGrid::QuadGrid()
{
}

QuadGrid::~QuadGrid()
{
}

void QuadGrid::BuildTree(unsigned int maxLevel, unsigned int worldSize, const sf::Vector2f & startPos)
{
	Quadrant q;
	q.Build(startPos, sf::Vector2f(worldSize, worldSize), 0, maxLevel, m_quadTree);
}

bool QuadGrid::GetQuadrant(const sf::Vector2f & worldPos, unsigned int level, Quadrant * outQuadrant)
{
	outQuadrant = nullptr;
	if (m_quadTree.empty())
		return false;

	sf::Vector2f searchPos = worldPos;
	
	sf::Vector2f worldStart = m_quadTree[0].GetMin();
	sf::Vector2f worldEnd = m_quadTree[0].GetMax();

	searchPos.x = searchPos.x >= worldStart.x ? searchPos.x : worldStart.x;
	searchPos.x = searchPos.x <= worldEnd.x ? searchPos.x : worldEnd.x;
	searchPos.y = searchPos.y >= worldStart.y ? searchPos.y : worldStart.y;
	searchPos.y = searchPos.y <= worldEnd.y ? searchPos.y : worldEnd.y;



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

