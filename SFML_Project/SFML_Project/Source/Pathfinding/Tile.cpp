#include "Tile.h"
#include <stdlib.h>
#include <cstring>

sf::Vector2f Tile::s_size = sf::Vector2f(32, 32);

Tile::Tile(sf::Vector2i gridCoord, sf::Vector2f worldCoord, bool pathable)
{
	m_gridCoord = gridCoord;
	m_worldCoord = worldCoord;
	m_pathable = pathable;
	m_subGrid = 0;
}

Tile::Tile(const Tile & other)
{
	_copy(other);
}

Tile::~Tile()
{
	
}

#pragma region GET & SET
const sf::Vector2i & Tile::GetGridCoord() const
{
	return m_gridCoord;
}

const Tile::PathFindingVars & Tile::GetPathfindingVars() const
{
	return m_pfv;
}

const sf::Vector2f & Tile::GetWorldCoord() const
{
	return m_worldCoord;
}

const sf::Vector2f & Tile::GetTileSize()
{
	return s_size;
}

const bool Tile::IsPathable() const
{
	return m_pathable;
}

const int Tile::GetSubGrid() const
{
	return m_subGrid;
}

void Tile::SetGridCoord(const sf::Vector2i & gridCoord)
{
	m_gridCoord = gridCoord;
}

void Tile::SetPathfindingVars(const PathFindingVars & pfv)
{
	m_pfv = pfv;
}

void Tile::SetGridCoord(const int x, const int y)
{
	SetGridCoord({ x, y });
}

void Tile::SetWorldCoord(const sf::Vector2f & worldCoord)
{
	m_worldCoord = worldCoord;
}

void Tile::SetWorldCoord(const float x, const float y)
{
	SetWorldCoord({ x, y });
}

void Tile::SetTileSize(const sf::Vector2f & tileSize)
{
	s_size = tileSize;
}

void Tile::SetPathable(const bool pathable)
{
	m_pathable = pathable;
}

void Tile::SetSubGrid(const int subGrid)
{
	m_subGrid = subGrid;
}

#pragma endregion

#pragma region Operators
Tile & Tile::operator=(const Tile & other)
{
	if (this != &other)
	{
		_copy(other);
	}
	return *this;
}

bool Tile::operator==(const Tile & other)
{
	return m_gridCoord == other.m_gridCoord;
}

bool Tile::operator==(const sf::Vector2i & gridCoord)
{
	return m_gridCoord == gridCoord;
}

bool Tile::operator<(const Tile & other)
{
	return m_pfv.fCost < other.m_pfv.fCost;
}

//bool Tile::operator>(const Tile & other)
//{
//	return !(*this < other);
//}

std::string Tile::ToString() const
{
	std::string str = "";
	str += "GridCoord: " + std::to_string(m_gridCoord.x) + ", " + std::to_string(m_gridCoord.y) + "\n";
	str += "WorldCoord: " + std::to_string(m_worldCoord.x) + ", " + std::to_string(m_worldCoord.y) + "\n";
	str += "Size: " + std::to_string(s_size.x) + ", " + std::to_string(s_size.y) + "\n";
	str += "Pathable: " + std::to_string(m_pathable) + "\n";
	str += "SubGrid: " + std::to_string(m_subGrid) + "\n";
	str += "PfvVars: \n" + m_pfv.ToString();
	str += "\n";
	return str;
}

void Tile::_copy(const Tile & other)
{
	m_gridCoord = other.m_gridCoord;
	m_pfv = other.m_pfv;
	m_worldCoord = other.m_worldCoord;
	m_pathable = other.m_pathable;
	m_subGrid = other.m_subGrid;
}
#pragma endregion