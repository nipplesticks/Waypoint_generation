#include "Grid.h"
#include "../Engine.h"
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#define NUMBER_OF_THREADS 8

bool Grid::Flag_Best_Grid_Path = true;

Grid::Grid_Heuristic Grid::Flag_Grid_Heuristic = Grid::Pure_Distance;

bool Grid::Flag_Use_Waypoint_Traversal = true;

bool Grid::Flag_Draw_Waypoint_Traversal = false;
bool Grid::Flag_Draw_Grid_Traversal = false;

int Grid::Flag_Sleep_Time_Finnished_Waypoint_Traversal = 0;
int Grid::Flag_Sleep_Time_During_Waypoint_Traversal = 0;
int Grid::Flag_Sleep_Time_Finnished_Grid_Traversal = 0;
int Grid::Flag_Sleep_Time_During_Grid_Traversal = 0;

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
			sf::Vector2f offset = sf::Vector2f((float)x, (float)y);
			offset.x *= tileSize.x;
			offset.y *= tileSize.y;
			m_grid[index].SetWorldCoord(gridStartPosition + offset);
		}
	}

}

Grid::~Grid()
{
}

std::vector<Tile> Grid::FindPath(const sf::Vector2f & source, const sf::Vector2f & destination, double & _outTimeWp, double & _outTimeGrid, sf::RenderWindow * wnd, Engine * eng)
{
	_outTimeWp = 0.0;
	_outTimeGrid = 0.0;


	if (source.x < m_grid.front().GetWorldCoord().x || source.y < m_grid.front().GetWorldCoord().y ||
		destination.x < m_grid.front().GetWorldCoord().x || destination.y < m_grid.front().GetWorldCoord().y ||
		source.x > m_grid.back().GetWorldCoord().x + Tile::GetTileSize().x || source.y > m_grid.back().GetWorldCoord().y + Tile::GetTileSize().y ||
		destination.x > m_grid.back().GetWorldCoord().x + Tile::GetTileSize().x || destination.y > m_grid.back().GetWorldCoord().y + Tile::GetTileSize().y)
		return std::vector<Tile>();


	std::vector<Tile> tileChain;
	std::vector<Tile> path;
	
	if (Flag_Use_Waypoint_Traversal)
		_outTimeWp = _createTileChain(tileChain, source, destination, wnd, eng);

	if (tileChain.size() > 1)
	{
		int pathIterations = (int)tileChain.size() - 1;

		for (int i = 0; i < pathIterations; i++)
		{
			double time;
			std::vector<Tile> partOfPath = _findPath(tileChain[i].GetWorldCoord(), tileChain[i + 1].GetWorldCoord(), time, wnd, eng);
			path.insert(path.end(), partOfPath.begin(), partOfPath.end());
			_outTimeGrid += time;
		}
	}
	// Not sure if needed
	// Might be an edge case if source and destination belong to the same waypoint
	else
	{
		path = _findPath(source, destination, _outTimeGrid, wnd, eng);
	}

	return path;
}

void Grid::Block(const sf::Vector2i & coord)
{
	m_grid[coord.x + coord.y * m_gridSize.x].SetPathable(false);
}

void Grid::SetWaypoints(const std::vector<Waypoint>& waypoints, QuadTree * q)
{
	m_waypoints = waypoints;
	m_wpNodes = std::vector<WpNode>(m_waypoints.size());

	for (size_t i = 0; i < m_waypoints.size(); i++)
	{
		m_wpNodes[i] = WpNode(-1, &m_waypoints[i], 0.0f, 0.0f);
	}

	int totalGridSize = m_grid.size();

	int stepSize = totalGridSize / NUMBER_OF_THREADS;
	std::thread threads[NUMBER_OF_THREADS];

	Timer time;
	time.Start();
	for (int i = 0; i < NUMBER_OF_THREADS; i++)
	{
		int start = stepSize * i;
		int end = start + stepSize;
		if (i == NUMBER_OF_THREADS - 1)
			end = totalGridSize;

		threads[i] = std::thread(&Grid::_createFields, this, start, end, q);
	}

	for (int i = 0; i < NUMBER_OF_THREADS; i++)
		threads[i].join();

	std::cout << "Time to create fields: " << time.Stop() << " sec\n";

}

Tile Grid::TileFromWorldCoords(const sf::Vector2f & worldCoord) const
{
	sf::Vector2f tileSize = Tile::GetTileSize();
	sf::Vector2f gridStart = m_grid[0].GetWorldCoord();
	sf::Vector2f startToSource = worldCoord - gridStart;
	sf::Vector2i gridSourceIndex = sf::Vector2i((int)(startToSource.x / tileSize.x), (int)(startToSource.y / tileSize.y));

	if (gridSourceIndex.x < 0 || gridSourceIndex.x >= m_gridSize.x * m_gridSize.y)
	{
		return Tile();
	}

	return m_grid[gridSourceIndex.x + gridSourceIndex.y * m_gridSize.x];
}

const Tile & Grid::At(int x, int y)
{
	return m_grid[x + y * m_gridSize.x];
}

void Grid::_createFields(int start, int end, QuadTree * q)
{
	using namespace DirectX;

	int size = m_waypoints.size();

	for (int i = start; i < end; i++)
	{
		sf::Vector2f tWorld = m_grid[i].GetWorldCoord() + m_grid[i].GetTileSize() * 0.5f;

		if (m_grid[i].IsPathable())
		{
			if (m_grid[i].GetFieldOwner() == nullptr)
			{
				float distance = FLT_MAX;
				Waypoint * wp = nullptr;

				for (int i = 0; i < size; i++)
				{
					sf::Vector2f wpWorld = m_waypoints[i].GetWorldCoord();
					Entity * e = q->DispatchRay(tWorld, wpWorld);
					if (e == nullptr)
					{
						float tTemp = XMVectorGetX(XMVector2LengthSq(XMVectorSubtract(XMVectorSet(wpWorld.x, wpWorld.y, 0.0f, 0.0f), XMVectorSet(tWorld.x, tWorld.y, 0.0f, 0.0f))));

						if (tTemp < distance)
						{
							distance = tTemp;
							wp = &m_waypoints[i];
						}

					}
				}
				m_grid[i].SetFieldOwner(wp);
			}
		}
	}

}

void Grid::_checkNode(const Node & current,
	float addedGCost, int offsetX, int offsetY,
	const Tile & destination,
	std::vector<Node>& openList,
	int parentIndex,
	std::vector<bool>& closedList)
{
	sf::Vector2i currentIndex = m_grid[current.gridTileIndex].GetGridCoord();

	Tile nextTile = Tile(currentIndex + sf::Vector2i(offsetX, offsetY));
	int nextTileIndex = nextTile.GetGridCoord().x + nextTile.GetGridCoord().y * m_gridSize.x;


	if (_isValid(nextTile) && !closedList[nextTileIndex] && m_grid[nextTileIndex].IsPathable())
	{
		Node newNode(parentIndex, nextTile.Get1DGridCoord(m_gridSize.x), current.gCost + addedGCost, _calcHValue(nextTile, destination));
		openList.push_back(newNode);

		if (!Flag_Best_Grid_Path)
			closedList[nextTileIndex] = true;
	}
}

std::vector<Tile> Grid::_findPath(const sf::Vector2f & source, const sf::Vector2f & destination, double & _outTime, sf::RenderWindow * wnd, Engine * eng)
{
	_outTime = 0.0;
	double drawTime = 0.0;
	Timer totalTimer;
	totalTimer.Start();


	sf::Vector2f tileSize = Tile::GetTileSize();
	sf::Vector2f gridStart = m_grid[0].GetWorldCoord();

	sf::Vector2f startToSource = source - gridStart;
	sf::Vector2f startToEnd = destination - gridStart;

	sf::Vector2i gridSourceIndex = sf::Vector2i((int)(startToSource.x / tileSize.x), (int)(startToSource.y / tileSize.y));
	sf::Vector2i gridDestIndex = sf::Vector2i((int)(startToEnd.x / tileSize.x), (int)(startToEnd.y / tileSize.y));


	if (gridSourceIndex.x	< 0 || gridSourceIndex.x	>= m_gridSize.x ||
		gridSourceIndex.y	< 0 || gridSourceIndex.y	>= m_gridSize.y ||
		gridDestIndex.x		< 0 || gridDestIndex.x		>= m_gridSize.x || 
		gridDestIndex.y		< 0 || gridDestIndex.y		>= m_gridSize.y)
		return std::vector<Tile>();

	Tile tileSource = m_grid[gridSourceIndex.x + gridSourceIndex.y * m_gridSize.x];
	Tile tileDestination = m_grid[gridDestIndex.x + gridDestIndex.y * m_gridSize.x];

	Node nSource(-1, tileSource.Get1DGridCoord(m_gridSize.x), 0.f, _calcHValue(tileSource, tileDestination));

	if (tileSource.GetSubGrid() != tileDestination.GetSubGrid() || !tileDestination.IsPathable() || !tileSource.IsPathable())
		return std::vector<Tile>();

	std::vector<bool>	closedList(m_gridSize.x * m_gridSize.y);

	std::vector<Node>	openList;
	std::vector<Node>	earlyExploration;
	std::vector<Node> nodes;

	Node	currentNode = nSource;
	Node	earlyExplorationNode;

	openList.push_back(currentNode);

	while (!openList.empty() || earlyExplorationNode.parentIndex != -1)
	{
#pragma region DRAW PATH
		if (wnd && Flag_Draw_Grid_Traversal)
		{
			Timer drawTimer;
			drawTimer.Start();
			sf::Event event;
			while (wnd->pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					wnd->close();
			}

			wnd->clear();
			eng->Draw(false);


			int counter = 0;

			Node printMe = currentNode;
			
			Entity e;

			for (auto & n : nodes)
			{
				Tile t = m_grid[n.gridTileIndex];
				
				e.SetPosition(t.GetWorldCoord());
				e.SetSize(t.GetTileSize());
				e.SetColor(sf::Color::Red);
				e.SetOutlineColor(sf::Color::Black);
				e.SetOutlineThickness(-1.0f);

				e.Draw(wnd);
			}

			while (printMe.parentIndex != -1)
			{
				Tile t = m_grid[printMe.gridTileIndex];
				printMe = nodes[printMe.parentIndex];

				e.SetPosition(t.GetWorldCoord());
				e.SetSize(t.GetTileSize());
				e.SetColor(sf::Color::White);
				e.SetOutlineColor(sf::Color::Black);
				e.SetOutlineThickness(-1.0f);

				e.Draw(wnd);
			}

			wnd->setTitle(std::to_string(nodes.size()));

			wnd->display();

			if (Flag_Sleep_Time_During_Grid_Traversal)
				Sleep(Flag_Sleep_Time_During_Grid_Traversal);

			drawTime += drawTimer.Stop(Timer::MILLISECONDS);
		}

#pragma endregion

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			return std::vector<Tile>();

		if (earlyExplorationNode.parentIndex != -1)
		{
			currentNode = earlyExplorationNode;
			earlyExplorationNode.parentIndex = -1;
		}
		else
		{
			std::sort(openList.begin(), openList.end());
			currentNode = openList.front();
			openList.erase(openList.begin());
		}
		nodes.push_back(currentNode);

		if (currentNode.gridTileIndex == tileDestination.Get1DGridCoord(m_gridSize.x))
		{
			std::vector<Tile> path;

			while (currentNode.parentIndex != -1)
			{
				Tile t = m_grid[currentNode.gridTileIndex];
				path.push_back(t);
				currentNode = nodes[currentNode.parentIndex];
			}

			std::reverse(path.begin(), path.end());
			path.push_back(tileDestination);

			if (wnd && Flag_Draw_Grid_Traversal)
			{
				Timer drawTimer;
				drawTimer.Start();
				sf::Event event;
				while (wnd->pollEvent(event))
				{
					if (event.type == sf::Event::Closed)
						wnd->close();
				}

				wnd->clear();
				eng->Draw(false);

				Entity e;
				e.SetColor(sf::Color::Red);
				e.SetOutlineColor(sf::Color::Black);
				e.SetOutlineThickness(-1.0f);

				for (int i = 0; i < path.size(); i++)
				{
					e.SetPosition(path[i].GetWorldCoord());
					e.SetSize(path[i].GetTileSize());
					e.Draw(wnd);
				}

				wnd->display();

				if (Flag_Sleep_Time_Finnished_Grid_Traversal)
					Sleep(Flag_Sleep_Time_Finnished_Grid_Traversal);
				drawTime += drawTimer.Stop(Timer::MILLISECONDS);
			}

			_outTime = totalTimer.Stop(Timer::MILLISECONDS);
			_outTime -= drawTime;
			return path;
		}
		closedList[currentNode.gridTileIndex] = true;
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

		//float horVerCost = Tile::GetTileSize().x;
		//float diagCost = std::sqrt(Tile::GetTileSize().x * 2.0f);
		float horVerCost = 1.0f;
		float diagCost = 1.414f;

		int parentIndex = (int)nodes.size() - 1;

		_checkNode(currentNode, horVerCost, 0, -1, tileDestination, earlyExploration, parentIndex, closedList);
		/*---------- South ----------*/
		_checkNode(currentNode, horVerCost, 0, 1, tileDestination, earlyExploration, parentIndex, closedList);
		/*---------- West ----------*/
		_checkNode(currentNode, horVerCost, -1, 0, tileDestination, earlyExploration, parentIndex, closedList);
		/*---------- East ----------*/
		_checkNode(currentNode, horVerCost, 1, 0, tileDestination, earlyExploration, parentIndex, closedList);
		/*---------- Northwest ----------*/
		_checkNode(currentNode, diagCost, -1, -1, tileDestination, earlyExploration, parentIndex, closedList);
		/*---------- Northeast ----------*/
		_checkNode(currentNode, diagCost, 1, -1, tileDestination, earlyExploration, parentIndex, closedList);
		/*---------- Southwest ----------*/
		_checkNode(currentNode, diagCost, -1, 1, tileDestination, earlyExploration, parentIndex, closedList);
		/*---------- Southeast ----------*/
		_checkNode(currentNode, diagCost, 1, 1, tileDestination, earlyExploration, parentIndex, closedList);

		std::sort(earlyExploration.begin(), earlyExploration.end());

		if (!earlyExploration.empty() && earlyExploration.front().fCost <= currentNode.fCost)
		{
			earlyExplorationNode = earlyExploration.front();
			earlyExploration.erase(earlyExploration.begin());
		}
		openList.insert(openList.end(), earlyExploration.begin(), earlyExploration.end());
		earlyExploration.clear();
	}

	//pathFile.close();

	return std::vector<Tile>();
}

bool Grid::_isValid(const Tile & tile)
{
	sf::Vector2i index = tile.GetGridCoord();

	if (index.x < 0 || index.x >= m_gridSize.x || index.y < 0 || index.y >= m_gridSize.y)
		return false;

	return true;
}

bool Grid::_isValidCoord(const sf::Vector2i & coord)
{
	if (coord.x < 0 || coord.x >= m_gridSize.x || coord.y < 0 || coord.y >= m_gridSize.y)
		return false;
	return true;
}

float Grid::_calcHValue(const Tile & s, const Tile & d)
{
	using namespace DirectX;

	auto p1 = s.GetGridCoord();
	auto p2 = d.GetGridCoord();

	auto deltaX = abs(p1.x - p2.x);
	auto deltaY = abs(p1.y - p2.y);

	switch (Flag_Grid_Heuristic)
	{
	case Grid::Pure_Distance:
		return XMVectorGetX(XMVector2LengthSq(XMVectorSubtract(XMVectorSet(p2.x, p2.y, 0.0f, 0.0f), XMVectorSet(p1.x, p1.y, 0.0f, 0.0f))));;
		break;
	case Grid::Manhattan_Distance:
		return float(deltaX + deltaY);
		break;
	case Grid::Stanford_Distance:
		return float(deltaX + deltaY) + (-0.414) * (float)std::min(deltaX, deltaY);
		break;
	default:
		return 2.0f;
		break;
	}
}

double Grid::_createTileChain(std::vector<Tile>& tileChain, const sf::Vector2f & source, const sf::Vector2f & destination, sf::RenderWindow * wnd, Engine * eng)
{
	double time;
	Tile sourceTile = TileFromWorldCoords(source);
	Tile destinationTile = TileFromWorldCoords(destination);

	Waypoint * sourceWaypoint = sourceTile.GetFieldOwner();
	Waypoint * destinationWaypoint = destinationTile.GetFieldOwner();

	if (sourceWaypoint == nullptr || destinationWaypoint == nullptr)
		return 0;

	std::vector<WpNode> waypointPath = _findWaypointPath(sourceWaypoint, destinationWaypoint, m_wpNodes, time, wnd, eng);
	
	tileChain.push_back(TileFromWorldCoords(source));

	if (!waypointPath.empty())
	{
		int nrOfWaypoints = (int)waypointPath.size() - 1;
		for (int i = 1; i < nrOfWaypoints; i++)
		{
			tileChain.push_back(TileFromWorldCoords(waypointPath[i].ptr->GetWorldCoord()));
		}
	}

	tileChain.push_back(TileFromWorldCoords(destination));

	return time;
}

std::vector<Grid::WpNode> Grid::_findWaypointPath(Waypoint * source, Waypoint * destination, std::vector<WpNode> nodes, double & _outTime, sf::RenderWindow * wnd, Engine * eng)
{
	_outTime = 0.0;

	double drawTime = 0.0;

	Timer totalTime;
	totalTime.Start();

	WpNode current = WpNode(-1, source, 0.f, _calcWaypointHeuristic(source, destination));
	std::vector<WpNode> openList;
	std::vector<WpNode> waypointNodes;
	std::vector<WpNode> earlyExploration;

	openList.push_back(current);

	while (!openList.empty())
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			return std::vector<WpNode>();
#pragma region Draw Path
		if (wnd && Flag_Draw_Waypoint_Traversal)
		{
			Timer drawTimer;
			drawTimer.Start();
			sf::Event event;
			while (wnd->pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					wnd->close();
			}

			wnd->clear();
			eng->Draw(false);

			int counter = 0;

			WpNode printMe = current;
			Line l;

			for (auto & w : waypointNodes)
			{
				Waypoint * wp = w.ptr;
				if (w.parentIndex != -1)
				{
					l.SetLine(waypointNodes[w.parentIndex].ptr->GetWorldCoord(), wp->GetWorldCoord());
					l.Draw(wnd);
				}
			}

			if (current.parentIndex != -1)
			{
				l.SetColor(sf::Color::Red);
				l.SetLine(waypointNodes[current.parentIndex].ptr->GetWorldCoord(), current.ptr->GetWorldCoord());

				l.Draw(wnd);

				printMe = waypointNodes[current.parentIndex];

				while (printMe.parentIndex != -1)
				{
					l.SetLine(waypointNodes[printMe.parentIndex].ptr->GetWorldCoord(), printMe.ptr->GetWorldCoord());
					l.Draw(wnd);
					printMe = waypointNodes[printMe.parentIndex];
				}
			}

			wnd->display();
			if (Flag_Sleep_Time_During_Waypoint_Traversal)
				Sleep(Flag_Sleep_Time_During_Waypoint_Traversal);


			drawTime += drawTimer.Stop(Timer::MILLISECONDS);
		}
#pragma endregion


		std::sort(openList.begin(), openList.end());
		current = openList.front();
		openList.erase(openList.begin());
		waypointNodes.push_back(current);

		if (current.ptr == destination)
		{
			std::vector<WpNode> waypointPath;

			waypointPath.push_back(current);
			while (current.parentIndex != -1)
			{
				waypointPath.push_back(waypointNodes[current.parentIndex]);
				current = waypointPath.back();
			}
			std::reverse(waypointPath.begin(), waypointPath.end());

			if (wnd && Flag_Draw_Waypoint_Traversal)
			{
				Timer drawTimer;
				drawTimer.Start();
				sf::Event event;
				while (wnd->pollEvent(event))
				{
					if (event.type == sf::Event::Closed)
						wnd->close();
				}

				wnd->clear();
				eng->Draw(false);

				WpNode printMe = current;
				Line l;
				l.SetColor(sf::Color::Red);

				for (int i = 0; i < waypointPath.size() - 1; i++)
				{
					l.SetLine(waypointPath[i].ptr->GetWorldCoord(), waypointPath[i + 1].ptr->GetWorldCoord());
					l.Draw(wnd);
				}
				wnd->display();

				if (Flag_Sleep_Time_Finnished_Waypoint_Traversal)
					Sleep(Flag_Sleep_Time_Finnished_Waypoint_Traversal);

				drawTime += drawTimer.Stop(Timer::MILLISECONDS);
			}

			_outTime = totalTime.Stop(Timer::MILLISECONDS);
			_outTime -= drawTime;

			return waypointPath;
		}

		float lowestConnectionCost = FLT_MAX;
		int goToWaypoint = -1;
		int waypointIndex = 0;
		const std::vector<Waypoint::Connection> & waypointConnections = current.ptr->GetConnections();

		if (!waypointConnections.empty())
		{
			int connectionsSize = (int)waypointConnections.size();

			for (int i = 0; i < connectionsSize; i++)
			{
				if (!nodes[current.ptr->GetArrayIndex()].visitedConnections[i])
				{
					nodes[current.ptr->GetArrayIndex()].visitedConnections[i] = true;
					if (&m_waypoints[waypointConnections[i].Waypoint] == destination)
					{
						earlyExploration.push_back(WpNode(waypointNodes.size() - 1, &m_waypoints[waypointConnections[i].Waypoint], 0,0));
						goToWaypoint = waypointConnections[i].Waypoint;
						waypointIndex = i;

					}
					else
					{
						if (nodes[m_waypoints[waypointConnections[i].Waypoint].GetArrayIndex()].parentIndex)
						{
							earlyExploration.push_back(WpNode(waypointNodes.size() - 1, &m_waypoints[waypointConnections[i].Waypoint],
								waypointConnections[i].Cost + current.gCost,
								_calcWaypointHeuristic(&m_waypoints[waypointConnections[i].Waypoint], destination)));

							nodes[earlyExploration.back().ptr->GetArrayIndex()].parentIndex = 0;

							float connectionTraversalCost = _calcWaypointHeuristic(&m_waypoints[waypointConnections[i].Waypoint], destination) + waypointConnections[i].Cost;
							if (connectionTraversalCost < lowestConnectionCost)
							{
								lowestConnectionCost = connectionTraversalCost;
								goToWaypoint = waypointConnections[i].Waypoint;
								waypointIndex = i;
							}
						}
					}
				}
			}

			if (goToWaypoint != -1)
			{
				//std::sort(earlyExploration.begin(), earlyExploration.end());

				/*for (int i = 0; i < earlyExploration.size(); i++)
				{
					Waypoint * currentPtr = earlyExploration[i].ptr;
					auto iterator = std::find(currentPtr->GetConnections().begin(), currentPtr->GetConnections().end(), current.ptr);
					int index = iterator - currentPtr->GetConnections().begin();
					nodes[currentPtr->GetArrayIndex()].visitedConnections[index] = true;
				}*/

				openList.insert(openList.end(), earlyExploration.begin(), earlyExploration.end());
				earlyExploration.clear();
			}
		}
	}

	return std::vector<WpNode>();
}

float Grid::_calcWaypointHeuristic(const Waypoint * source, const Waypoint * destination)
{
	using namespace DirectX;

	//auto p1 = TileFromWorldCoords(source->GetWorldCoord()).GetGridCoord();
	//auto p2 = TileFromWorldCoords(destination->GetWorldCoord()).GetGridCoord();

	auto p1 = source->GetWorldCoord();
	auto p2 = destination->GetWorldCoord();

	float l = XMVectorGetX(XMVector2LengthSq(XMVectorSubtract(XMVectorSet(p2.x, p2.y, 0.0f, 0.0f), XMVectorSet(p1.x, p1.y, 0.0f, 0.0f))));

	return l;
}
