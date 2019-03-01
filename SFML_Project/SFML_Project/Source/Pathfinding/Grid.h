#pragma once
#include "Tile.h"
#include <vector>

class Grid
{
public:
	Grid(const sf::Vector2i & size, const sf::Vector2f & gridStartPosition, const sf::Vector2f & tileSize);
	~Grid();
	std::vector<Tile> FindPath(const sf::Vector2f & source, const sf::Vector2f & destination);
	void Block(const sf::Vector2i & coord);
	Tile TileFromWorldCoords(const sf::Vector2f & worldCoord) const;

	const Tile & At(int x, int y);

private:
	std::vector<Tile> m_grid;
	sf::Vector2i m_gridSize;

private:
	void _checkTile(const Tile & current,
		float addedGCost, int offsetX, int offsetY,
		const Tile & destination,
		std::vector<Tile> & openList,
		std::vector<Tile> & grid,
		std::vector<bool> & closedList
	);

	std::vector<Tile> _findPath(std::vector<Tile> grid, const sf::Vector2f & source, const sf::Vector2f & destination);
	bool _isValid(const Tile & tile);
	float _calcHValue(const Tile & s, const Tile & d);

};