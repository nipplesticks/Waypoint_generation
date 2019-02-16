#include "Grid.h"

Grid::Grid(const sf::Vector2i & size, const sf::Vector2f & gridStartPosition, const sf::Vector2f & tileSize)
{
	Tile::SetTileSize(tileSize);
	m_grid = std::vector<Tile>(size.x * size.y);
	m_gridSize = size;
	
	for (int y = 0; y < m_gridSize.y; y++)
	{
		for (int x = 0; x < m_gridSize.x; x++)
		{
			int index = x + y * m_gridSize.x;
			m_grid[index].SetGridCoord(x, y);
			sf::Vector2f offset = sf::Vector2f(x, y);
			offset.x *= tileSize.x;
			offset.y *= tileSize.y;
			m_grid[index].SetWorldCoord(gridStartPosition + offset);
		}
	}

}

Grid::~Grid()
{
}

std::vector<Tile> Grid::FindPath(const sf::Vector2f & source, const sf::Vector2f & destination)
{
	return _findPath(m_grid, source, destination);
}

void Grid::_checkTile(const Tile & current,
	float addedGCost, int offsetX, int offsetY,
	const Tile & destination,
	std::vector<Tile>& openList,
	std::vector<Tile>& grid,
	std::vector<bool>& closedList)
{
	sf::Vector2i currentIndex = current.GetGridCoord();

	Tile nextTile = Tile(currentIndex + sf::Vector2i(offsetX, offsetY));
	int nextTileIndex = nextTile.GetGridCoord().x + nextTile.GetGridCoord().y * m_gridSize.x;


	if (_isValid(nextTile) && !closedList[nextTileIndex] && grid[nextTileIndex].IsPathable())
	{
		Tile tile = grid[nextTileIndex];
		Tile::PathFindingVars pfv;
		pfv.parentIndex = currentIndex.x + currentIndex.y * m_gridSize.x;
		pfv.gCost = current.GetPathfindingVars().gCost + addedGCost;
		pfv.hCost = _calcHValue(tile, destination);
		pfv.fCost = pfv.gCost + pfv.hCost;
		tile.SetPathfindingVars(pfv);
		grid[nextTileIndex].SetPathfindingVars(pfv);
		openList.push_back(tile);
	}
}


std::vector<Tile> Grid::_findPath(std::vector<Tile> grid, const sf::Vector2f & source, const sf::Vector2f & destination)
{

	sf::Vector2f tileSize = Tile::GetTileSize();
	sf::Vector2f gridStart = m_grid[0].GetWorldCoord();

	sf::Vector2f startToSource = source - gridStart;
	sf::Vector2f startToEnd = destination - gridStart;

	sf::Vector2i gridSourceIndex = sf::Vector2i(startToSource.x / tileSize.x, startToSource.y / tileSize.y);
	sf::Vector2i gridDestIndex = sf::Vector2i(startToEnd.x / tileSize.x, startToEnd.y / tileSize.y);


	if (gridSourceIndex.x < 0 || gridSourceIndex.x >= m_gridSize.x || gridDestIndex.y < 0 || gridSourceIndex.y >= m_gridSize.y)
		return std::vector<Tile>();

	Tile tileSource = grid[gridSourceIndex.x + gridSourceIndex.y * m_gridSize.x];
	Tile tileDestination = grid[gridDestIndex.x + gridDestIndex.y * m_gridSize.x];;

	if (tileSource.GetSubGrid() != tileDestination.GetSubGrid() || !tileDestination.IsPathable() || !tileSource.IsPathable())
		return std::vector<Tile>();

	std::vector<bool>	closedList(m_gridSize.x * m_gridSize.y);
	std::vector<Tile>	openList;
	std::vector<Tile>	earlyExploration;
	int					earlyExplorationTileIndex = -1;
	Tile 				currentTile = tileSource;

	Tile::PathFindingVars pfv;
	pfv.parentIndex = -1;
	pfv.gCost = 0.0f;
	pfv.hCost = _calcHValue(tileSource, tileDestination);
	pfv.fCost = pfv.gCost + pfv.hCost;
	currentTile.SetPathfindingVars(pfv);

	grid[gridSourceIndex.x + gridSourceIndex.y * m_gridSize.x].SetPathfindingVars(pfv);


	openList.push_back(currentTile);

	while (!openList.empty() || earlyExplorationTileIndex != -1)
	{
		if (earlyExplorationTileIndex != -1)
		{
			currentTile = grid[earlyExplorationTileIndex];
			earlyExplorationTileIndex = -1;
		}
		else
		{
			std::sort(openList.begin(), openList.end());
			currentTile = openList.front();
			openList.erase(openList.begin());
		}

		if (currentTile == tileDestination)
		{
			std::vector<Tile> path;

			while (currentTile.GetPathfindingVars().parentIndex != -1)
			{
				Tile t = grid[currentTile.GetPathfindingVars().parentIndex];
				path.push_back(t);
				currentTile = t;
			}
			std::reverse(path.begin(), path.end());
			path.push_back(tileDestination);
			return path;
		}

		sf::Vector2i currentIndex = currentTile.GetGridCoord();
		closedList[currentIndex.x + currentIndex.y * m_gridSize.x] = true;
		/*
			Generate all the eight successors of the cell
			  N.W	N	N.E
				\	|	/
				 \	|  /
			W --- Node --- E
				 /	|  \
				/	|	\
			  S.W	S	S.E
			Node--> Current Node	= (0, 0)
			N	--> North			= (0, -1)
			S	--> South			= (0, 1)
			W	--> West			= (-1, 0)
			E	--> East			= (1, 0)
			N.W	--> Northwest		= (-1, -1)
			N.E	--> Northeast		= (1, -1)
			S.W	-->	Southwest		= (-1, 1)
			S.E	-->	Southeast		= (1, 1)
		*/

		// AddedGCost based on the distance to the node, 1.0 for direct paths and 1.414 for diagonal paths.
		// Offset defined by the new tiles direction standing at the source tile.
		// Check all possible node directions to see if they are valid or not for further exploration.
		/*---------- North ----------*/
		_checkTile(currentTile, 1.0f, 0, -1, tileDestination, earlyExploration, grid, closedList);
		/*---------- South ----------*/
		_checkTile(currentTile, 1.0f, 0, 1, tileDestination, earlyExploration, grid, closedList);
		/*---------- West ----------*/
		_checkTile(currentTile, 1.0f, -1, 0, tileDestination, earlyExploration, grid, closedList);
		/*---------- East ----------*/
		_checkTile(currentTile, 1.0f, 1, 0, tileDestination, earlyExploration, grid, closedList);
		/*---------- Northwest ----------*/
		_checkTile(currentTile, 1.414f, -1, -1, tileDestination, earlyExploration, grid, closedList);
		/*---------- Northeast ----------*/
		_checkTile(currentTile, 1.414f, 1, -1, tileDestination, earlyExploration, grid, closedList);
		/*---------- Southwest ----------*/
		_checkTile(currentTile, 1.414f, -1, 1, tileDestination, earlyExploration, grid, closedList);
		/*---------- Southeast ----------*/
		_checkTile(currentTile, 1.414f, 1, 1, tileDestination, earlyExploration, grid, closedList);

		std::sort(earlyExploration.begin(), earlyExploration.end());

		if (earlyExploration.size() > 0 && earlyExploration.front().GetPathfindingVars().fCost <= currentTile.GetPathfindingVars().fCost)
		{
			sf::Vector2i eIndex = earlyExploration.front().GetGridCoord();
			earlyExplorationTileIndex = eIndex.x + eIndex.y * m_gridSize.x;
			grid[earlyExplorationTileIndex].SetPathfindingVars(earlyExploration.front().GetPathfindingVars());
			earlyExploration.erase(earlyExploration.begin());
		}
		openList.insert(openList.end(), earlyExploration.begin(), earlyExploration.end());
		earlyExploration.clear();
	}

	return std::vector<Tile>();
}

bool Grid::_isValid(const Tile & tile)
{
	sf::Vector2i index = tile.GetGridCoord();

	if (index.x < 0 || index.x >= m_gridSize.x || index.y < 0 || index.y >= m_gridSize.y)
		return false;

	return true;
}

float Grid::_calcHValue(const Tile & s, const Tile & d)
{
	int deltaX = abs(s.GetGridCoord().x - d.GetGridCoord().x);
	int deltaY = abs(s.GetGridCoord().y - d.GetGridCoord().y);

	// 1.0f = Direct costs
	// 1.414f = Diagonal costs
	// 1.0f * (x + y) + (1.414f - 2 * 1.0f) * min(x, y)
	return (deltaX + deltaY) + (-0.414f) * std::min(deltaX, deltaY);
}
