#pragma once
#include "Tile.h"
#include <vector>
#include <SFML/Graphics.hpp>
#include "../Entity/Entity.h"
#include "Waypoint.h"
#include "QuadTree/QuadTree.h"

class Engine;

class Grid
{
public:
	Grid(const sf::Vector2i & size, const sf::Vector2f & gridStartPosition, const sf::Vector2f & tileSize);
	~Grid();

	std::vector<Tile> FindPath(const sf::Vector2f & source, const sf::Vector2f & destination, sf::RenderWindow * wnd = nullptr, Engine * eng = nullptr);

	void Block(const sf::Vector2i & coord);

	void SetWaypoints(const std::vector<Waypoint> & waypoints, QuadTree * q);

	Tile TileFromWorldCoords(const sf::Vector2f & worldCoord) const;

	const Tile & At(int x, int y);

private:
	std::vector<Tile> m_grid;
	std::vector<Waypoint> m_waypoints;
	sf::Vector2i m_gridSize;

private:
	struct Node
	{
		Node()
		{
			parentIndex = -1;
			gridTileIndex = 0;
			fCost = FLT_MAX;
			gCost = FLT_MAX;
			hCost = FLT_MAX;
		}
		Node(int _parentIndex, int _gridTileTindex, float _gCost, float _hCost)
		{
			parentIndex = _parentIndex;
			gridTileIndex = _gridTileTindex;
			fCost = _gCost + _hCost;
			gCost = _gCost;
			hCost = _hCost;

		}

		bool operator<(const Node & other) const
		{
			return fCost < other.fCost;
		}

		int parentIndex = -1;
		int gridTileIndex = 0;



		float fCost = FLT_MAX, gCost = FLT_MAX, hCost = FLT_MAX;
	};

	struct WpNode
	{
		WpNode()
		{
			parentIndex = -1;
			current = nullptr;
			fCost = FLT_MAX;
			gCost = FLT_MAX;
			hCost = FLT_MAX;
		}
		WpNode(int _parentIndex, Waypoint * _current, float _gCost, float _hCost)
		{
			parentIndex = _parentIndex;
			current = _current;
			gCost = _gCost;
			hCost = _hCost;
			fCost = gCost + hCost;
			visitedConnections = std::vector<bool>(current->GetConnections().size());
		}

		int parentIndex;
		Waypoint * current;
		std::vector<bool> visitedConnections;
		float fCost = FLT_MAX, gCost = FLT_MAX, hCost = FLT_MAX;
	};

	void _checkNode(const Node & current,
		float addedGCost, int offsetX, int offsetY,
		const Tile & destination,
		std::vector<Node> & openList,
		int parentIndex,
		std::vector<bool> & closedList
	);

	std::vector<Tile> _findPath(const sf::Vector2f & source, const sf::Vector2f & destination, sf::RenderWindow * wnd, Engine * eng);
	bool _isValid(const Tile & tile);
	bool _isValidCoord(const sf::Vector2i & coord);

	float _calcHValue(const Tile & s, const Tile & d);

	// Waypoint tracing
	void _createTileChain(std::vector<Tile> & tileChain, const sf::Vector2f & source, const sf::Vector2f & destination, sf::RenderWindow * wnd, Engine * eng);
};