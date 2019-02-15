#pragma once
#include <SFML/System/Vector2.hpp>
#include <algorithm>


class Tile
{
public:
	struct PathFindingVars
	{
		int parentIndex = -1;
		float fCost = FLT_MAX,
			gCost = FLT_MAX,
			hCost = FLT_MAX;
	};

public:
	Tile(sf::Vector2i gridCoord = sf::Vector2i(-1, -1), sf::Vector2f worldCoord = sf::Vector2f(0, 0), bool pathable = true);
	Tile(const Tile& other);
	~Tile();

	const sf::Vector2i& GetGridCoord() const;
	const PathFindingVars & GetPathfindingVars() const;
	const sf::Vector2f& GetWorldCoord() const;
	static const sf::Vector2f& GetTileSize();
	const bool IsPathable() const;
	const int GetSubGrid() const;

	void SetGridCoord(const sf::Vector2i& gridCoord);
	void SetPathfindingVars(const PathFindingVars & pfv);
	void SetGridCoord(const int x, const int y);
	void SetWorldCoord(const sf::Vector2f& worldCoord);
	void SetWorldCoord(const float x, const float y);
	static void SetTileSize(const sf::Vector2f& tileSize);
	void SetPathable(const bool pathable);
	void SetSubGrid(const int subGrid);

	Tile& operator=(const Tile& other);
	bool operator==(const Tile& other);
	bool operator==(const sf::Vector2i& gridCoord);
	bool operator<(const Tile& other);
	bool operator>(const Tile& other);



private:
	sf::Vector2i m_gridCoord;
	PathFindingVars m_pfv;
	sf::Vector2f m_worldCoord;
	// Tile size in world
	static sf::Vector2f s_size;
	bool m_pathable;
	int m_subGrid;
	int m_cost;

private:
	void _copy(const Tile& other);

};