#include "Engine.h"
#include <iostream>
#include <fstream>
#include <DirectXMath.h>
#include <sstream>

const bool DRAW_LINES = false;
const bool DRAW_FIELDS = false;
const bool CREATE_PATH = true;


Engine::Engine(sf::RenderWindow * window)
{
	m_pWindow = window;

	sf::Vector2f bPos(0, 0);

	m_font.loadFromFile("../Assets/ARLRDBD.TTF");

	m_mousePosText.setFont(m_font);
	m_mousePosText.setCharacterSize(16);
	m_mousePosText.setFillColor(sf::Color::Red);
	m_mousePosText.setOutlineThickness(5);
	m_mousePosText.setOutlineColor(sf::Color::Black);

	std::string tmpCoord = std::to_string(0.0f) + ", " + std::to_string(0.0f);
	m_mousePosText.setString(tmpCoord);

	auto bb = m_mousePosText.getLocalBounds();
	sf::Vector2f origin(bb.width + 10.0f, 0);
	m_mousePosText.setOrigin(origin);
	m_mousePosText.setPosition(m_pWindow->getSize().x - 500, 0.0f);


	m_strings[0] = "Best_Grid_Path : T";
	m_strings[1] = "Grid_Heuristic : PD";
	m_strings[2] = "Use_Waypoint_Traversal : T";
	m_strings[3] = "Draw_Waypoint_Traversal : F";
	m_strings[4] = "Flag_Draw_Grid_Traversal : F";
	m_strings[5] = "ST_F_Wp_Traversal : 0";
	m_strings[6] = "ST_D_Wp_Traversal : 0";
	m_strings[7] = "ST_F_Grid_Traversal : 0";
	m_strings[8] = "ST_D_Grid_Traversal : 0";
	
	m_hArr[0] = Grid::Pure_Distance;
	m_hArr[1] = Grid::Manhattan_Distance;
	m_hArr[2] = Grid::Stanford_Distance;

	for (int i = 0; i < _countof(m_buttons); i++)
	{
		m_buttons[i].SetPosition(bPos + sf::Vector2f(240, 0));
		m_text[i].setFont(m_font);
		m_text[i].setCharacterSize(16);
		m_text[i].setFillColor(sf::Color::White);
		m_text[i].setOutlineColor(sf::Color::Black);
		m_text[i].setOutlineThickness(3.0f);
		m_text[i].setPosition(bPos + sf::Vector2f(0, 6));
		m_text[i].setString(m_strings[i]);
		bPos.y += 34;
	}

	m_camera.SetAsActive();
	sf::IntRect iRect(0, 0, 32, 32);

	m_texture.Load("../Assets/Test.bmp", iRect, {1, 8});
	m_grassTexture.Load("../Assets/Grass.bmp", iRect, {1, 1});
	m_brickTexture.Load("../Assets/Brick.bmp", iRect, {1, 1});

	m_background.SetTexture(&m_grassTexture);
	m_background.SetSize((sf::Vector2f)m_pWindow->getSize());

	m_background.SetPosition(0, 0);

	m_player.SetPosition(0, 0);
	m_player.SetColor(255, 255, 255);
	m_player.SetTexture(&m_texture, true);
	m_player.SetSpeed(128.0f);

	m_camera.SetPosition(0, 0);

	m_sourceTile.SetColor(sf::Color::Cyan);
	m_sourceTile.SetSize(MAP_TILE_SIZE, MAP_TILE_SIZE);
	m_sourceTile.SetOutlineThickness(-3);
	m_sourceTile.SetOutlineColor(sf::Color::Black);

	m_endTile.SetColor(sf::Color::Magenta);
	m_endTile.SetSize(MAP_TILE_SIZE, MAP_TILE_SIZE);
	m_endTile.SetOutlineThickness(-3);
	m_endTile.SetOutlineColor(sf::Color::Black);


	Draw();

	//_loadMap("SmallMap.txt");
	//_loadMap("bigGameProjectGrid.txt");
	_loadMap("UMAP2.txt");
	//_loadMap("bigGameProjectGridEdgy");
	//_loadMap("bigGameProjectGridEdgy2");
}

Engine::~Engine()
{
	m_pWindow = nullptr;

	delete[] m_grid;
}

void Engine::Run()
{
	m_pWindow->setActive();
	double total = 0.0;
	int counter = 0;
	m_running = true;
	Timer deltaTime;
	Timer deltaTime2;
	deltaTime.Start();
	deltaTime2.Start();

	while (IsRunning())
	{

		Timer eventTimer;
		eventTimer.Start();
		sf::Event event;
		while (m_pWindow->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				m_pWindow->close();
		}
		
		counter++;

		Update(deltaTime.Stop());

		Draw();

		total += deltaTime2.Stop(Timer::MILLISECONDS);

		if (total > 1000)
		{
			m_pWindow->setTitle(std::to_string(total / counter) + " ms");
			counter = 0;
			total = 0.0;
		}
	}
}

std::string sfVecToString(const sf::Vector2f & vec)
{
	return "X: " + std::to_string(vec.x) + ", Y: " + std::to_string(vec.y);
}

void Engine::Update(double dt)
{
	static bool s_mouseLeftLastFrame = false;
	static bool s_isPrinted = false;
	static bool s_runTest = false;
	static bool timerStart = false;
	static Timer timer;

	float zoom = m_camera.GetPosition().z;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		m_camera.Translate(CAMERA_MOVE_SPEED * zoom * (float)dt, 0.0f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		m_camera.Translate(-CAMERA_MOVE_SPEED * zoom * (float)dt, 0.0f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		m_camera.Translate(0.0f, -CAMERA_MOVE_SPEED * zoom * (float)dt);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		m_camera.Translate(0.0f, CAMERA_MOVE_SPEED * zoom * (float)dt);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
		m_camera.Translate(0.0f, 0.0f, -CAMERA_ZOOM_SPEED * (float)dt);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		m_camera.Translate(0.0f, 0.0f, CAMERA_ZOOM_SPEED * (float)dt);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
	{
		m_player.SetPosition(m_playerSpawn);
		m_player.SetPath(std::vector<Tile>());
	}


	bool mouseRightThisFrame = sf::Mouse::isButtonPressed(sf::Mouse::Right);
	bool mouseLeftThisFrame = sf::Mouse::isButtonPressed(sf::Mouse::Left);

	double time = 0;

	Timer t;
	t.Start();
	sf::Vector2i mousePos = sf::Mouse::getPosition(*m_pWindow);
	if (mouseLeftThisFrame)
	{
		for (int i = 0; i < _countof(m_buttons); i++)
		{
			int pVal = 0;
			if (pVal = m_buttons[i].Pressed(mousePos))
			{
				switch (i)
				{
				case 0:
					if (pVal == 1)
					{
						Grid::Flag_Best_Grid_Path = false;
						m_strings[0] = "Best_Grid_Path : F";
					}
					else
					{
						Grid::Flag_Best_Grid_Path = true;
						m_strings[0] = "Best_Grid_Path : T";
					}
					break;
				case 1:
					if (!s_mouseLeftLastFrame)
					{
						if (pVal == 1)
							m_currentChoice--;
						else
							m_currentChoice++;
						m_currentChoice = std::clamp(m_currentChoice, 0, 2);
						Grid::Flag_Grid_Heuristic = m_hArr[m_currentChoice];
						
						if (Grid::Flag_Grid_Heuristic == Grid::Pure_Distance)
							m_strings[1] = "Grid_Heuristic : PD";
						else if (Grid::Flag_Grid_Heuristic == Grid::Manhattan_Distance)
							m_strings[1] = "Grid_Heuristic : MD";
						else
							m_strings[1] = "Grid_Heuristic : SD";
					}
					break;
				case 2:
					if (pVal == 1)
					{
						Grid::Flag_Use_Waypoint_Traversal = false;
						m_strings[2] = "Use_Waypoint_Traversal : F";

					}
					else
					{
						Grid::Flag_Use_Waypoint_Traversal = true;
						m_strings[2] = "Use_Waypoint_Traversal : T";

					}
					break;
				case 3:
					if (pVal == 1)
					{
						Grid::Flag_Draw_Waypoint_Traversal = false;
						m_strings[3] = "Draw_Waypoint_Traversal : F";

					}
					else
					{
						Grid::Flag_Draw_Waypoint_Traversal = true;
						m_strings[3] = "Draw_Waypoint_Traversal : T";

					}

					break;
				case 4:
					if (pVal == 1)
					{
						Grid::Flag_Draw_Grid_Traversal = false;
						m_strings[4] = "Flag_Draw_Grid_Traversal : F";

					}
					else
					{
						Grid::Flag_Draw_Grid_Traversal = true;
						m_strings[4] = "Flag_Draw_Grid_Traversal : T";

					}
					break;
				case 5:
					if (pVal == 1)
						Grid::Flag_Sleep_Time_Finnished_Waypoint_Traversal--;
					else
						Grid::Flag_Sleep_Time_Finnished_Waypoint_Traversal++;

					Grid::Flag_Sleep_Time_Finnished_Waypoint_Traversal = std::max(0, Grid::Flag_Sleep_Time_Finnished_Waypoint_Traversal);

					m_strings[5] = "ST_F_Wp_Traversal : " + std::to_string(Grid::Flag_Sleep_Time_Finnished_Waypoint_Traversal);
					break;
				case 6:
					if (pVal == 1)
						Grid::Flag_Sleep_Time_During_Waypoint_Traversal--;
					else
						Grid::Flag_Sleep_Time_During_Waypoint_Traversal++;

					Grid::Flag_Sleep_Time_During_Waypoint_Traversal = std::max(0, Grid::Flag_Sleep_Time_During_Waypoint_Traversal);
					m_strings[6] = "ST_D_Wp_Traversal : " + std::to_string(Grid::Flag_Sleep_Time_During_Waypoint_Traversal);

					break;
				case 7:
					if (pVal == 1)
						Grid::Flag_Sleep_Time_Finnished_Grid_Traversal--;
					else
						Grid::Flag_Sleep_Time_Finnished_Grid_Traversal++;

					Grid::Flag_Sleep_Time_Finnished_Grid_Traversal = std::max(0, Grid::Flag_Sleep_Time_Finnished_Grid_Traversal);

					m_strings[7] = "ST_F_Grid_Traversal : " + std::to_string(Grid::Flag_Sleep_Time_Finnished_Grid_Traversal);
					break;
				case 8:
					if (pVal == 1)
						Grid::Flag_Sleep_Time_During_Grid_Traversal--;
					else
						Grid::Flag_Sleep_Time_During_Grid_Traversal++;

					Grid::Flag_Sleep_Time_During_Grid_Traversal = std::max(0, Grid::Flag_Sleep_Time_During_Grid_Traversal);
					m_strings[8] = "ST_D_Grid_Traversal : " + std::to_string(Grid::Flag_Sleep_Time_During_Grid_Traversal);
					break;
				}

				for (int k = 0; k < _countof(m_strings); k++)
				{
					m_text[k].setString(m_strings[k]);
				}

				break;
			}
		}
	}

	sf::Vector2u windowSize = m_pWindow->getSize();
	sf::Vector2f clickWorld;
	clickWorld.x = (((float)mousePos.x - (float)windowSize.x * 0.5f) * zoom) + m_camera.GetPosition().x;
	clickWorld.y = (((float)mousePos.y - (float)windowSize.y * 0.5f) * zoom) + m_camera.GetPosition().y;

	std::string mpWorld = std::to_string(clickWorld.x) + ", " + std::to_string(clickWorld.y);

	bool spacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
		
	static int counter = 0;

	if (CREATE_PATH)
	{
		if (spacePressed && !s_isPrinted)
		{
			auto pPos = m_player.GetPosition();
			std::string outStart = std::to_string(pPos.x) + " " + std::to_string(pPos.y) + " ";
			std::string outDest = std::to_string(clickWorld.x) + " " + std::to_string(clickWorld.y) + "\n";
			m_newPaths << outStart;
			m_newPaths << outDest;
			counter++;
		}

	
		if (counter > 8 && m_newPaths.is_open())
			m_newPaths.close();
	}
	else
	{
		if (spacePressed && !s_isPrinted)
		{
			s_runTest = true;
			std::cout << "-- STARTING TEST -- \n";
			std::cout << "Using Best Grid Path: " << (Grid::Flag_Best_Grid_Path ? "true" : "false") << std::endl;
			std::cout << "Grid_Heuristic: ";

			switch (Grid::Flag_Grid_Heuristic)
			{
			case 0:
				std::cout << "Pure Distance\n";
				break;
			case 1:
				std::cout << "Manhattan Distance\n";
				break;
			case 2:
				std::cout << "Stanford Distance\n";
				break;
			}

			std::cout << "Traversing waypoints: " << (Grid::Flag_Use_Waypoint_Traversal ? "true" : "false");
			std::cout << "\n\n";
		}
	}

	if (s_runTest)
	{
		double d1, d2;

		m_sourceTile.SetPosition(m_testPath[counter].first);
		m_endTile.SetPosition(m_testPath[counter].second);

		m_grid->FindPath(m_testPath[counter].first, m_testPath[counter].second, d1, d2, m_pWindow, this);

		std::cout << "Path: " << counter + 1 << std::endl;
		std::cout << "WPPathTime: " << d1 << " ms\t GridPathTime: " << d2 << " ms\nTotal Time: " << d1 + d2 << " ms" << std::endl;
		std::cout << "Source: [" << m_testPath[counter].first.x << " , " << m_testPath[counter].first.y << "] Destination: [" << m_testPath[counter].second.x << " , "<< m_testPath[counter].second.y << "]" << std::endl << std::endl;
		
		if (++counter > 8)
		{
			s_runTest = false;
			counter = 0;
			std::cout << "-- Test complete --\n";
		}
	}
	s_isPrinted = spacePressed;

	m_mousePosText.setString(mpWorld);

	if (mouseRightThisFrame /* && !s_mouseRightLastFrame*/)
	{
		std::vector<Tile> playerPath = m_player.GetPath();
		std::vector<Tile> newPath;



		double dummy1, dummy2;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && !playerPath.empty())
		{
			
			sf::Vector2f source = playerPath.back().GetWorldCoord();
			newPath = m_grid->FindPath(source, clickWorld, dummy1, dummy2);
			newPath.insert(newPath.begin(), playerPath.begin(), playerPath.end());
		}
		else
		{
			
			newPath = m_grid->FindPath(m_player.GetPosition() + m_player.GetSize() * 0.5f, clickWorld, dummy1, dummy2, m_pWindow, this);

		}
		
		if (!newPath.empty())
		{
			//std::cout << "Path: [" << sfVecToString(m_player.GetPosition())  << "] --> [" <<sfVecToString(newPath.back().GetWorldCoord()) << "]\n";
			m_player.SetPath(newPath);
		}
	}
	
	time = t.Stop();
	//std::cout << "Time: " << std::to_string(time) << " ms\n\n";
	m_pathFindingTime = time;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
	{
		dt *= 4.0f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
	{
		dt *= 10.0f;
	}

	m_player.Update(dt);

	s_mouseLeftLastFrame = mouseLeftThisFrame;
}

void Engine::Draw(bool clearAndDisplay)
{
	if (clearAndDisplay)
		m_pWindow->clear();
	Camera * cam = Camera::GetActiveCamera();
	sf::Vector3f camPos = cam->GetPosition();
	sf::Vector2u wndSize = m_pWindow->getSize();

	m_background.Draw(m_pWindow);

	for (auto & b : m_blocked)
		b.Draw(m_pWindow);

	for (auto & f : m_field)
		f.Draw(m_pWindow);

	for (auto & w : m_waypoints)
		w.Draw(m_pWindow);
	for (auto & l : m_lines)
		l.Draw(m_pWindow);

	m_player.Draw(m_pWindow);

	m_sourceTile.Draw(m_pWindow);
	m_endTile.Draw(m_pWindow);

	for (int i = 0; i < _countof(m_buttons); i++)
	{
		m_buttons[i].Draw(m_pWindow);
		m_pWindow->draw(m_text[i]);
	}

	m_pWindow->draw(m_mousePosText);

	if (clearAndDisplay)
		m_pWindow->display();
}

bool Engine::IsRunning() const
{
	return m_pWindow && m_pWindow->isOpen() && m_running;
}

void Engine::Terminate()
{
	m_running = false;
}

void Engine::_loadTestPath(const std::string & pathName)
{
	std::ifstream mapPath;
	mapPath.open("../Assets/" + pathName);

	std::string line;

	for (int i = 0; i < 9; i++)
	{
		std::getline(mapPath, line);
		std::stringstream ss(line);
		
		std::pair<sf::Vector2f, sf::Vector2f> tmp;
		
		ss >> tmp.first.x;
		ss >> tmp.first.y;
		ss >> tmp.second.x;
		ss >> tmp.second.y;

		m_testPath[i] = tmp;
	}

	mapPath.close();
}

void Engine::_loadMap(const std::string & mapName)
{
	if (CREATE_PATH)
		m_newPaths.open("../Assets/PATHS.txt");
	else
		_loadTestPath(mapName + "Paths.txt");


	std::ifstream mapText;
	mapText.open("../Assets/" + mapName + ".txt");

	int yLevel = 0;
	int xLevel = 0;
	
	std::vector<bool> map;
	std::string row;


	while (std::getline(mapText, row))
	{
		xLevel = (int)row.size();

		for (int i = 0; i < xLevel; i++)
		{
			map.push_back(row[i] == '#');
		}

		yLevel++;
	}

	m_mapWidth = xLevel;
	m_mapHeight = yLevel;

	mapText.close();
	
	m_grid = new Grid(sf::Vector2i(xLevel, yLevel), { 0.0f, 0.0f }, { 32.0f, 32.0f });


	bool placedPlayer = false;

	m_background.SetSize(xLevel * MAP_TILE_SIZE, yLevel * MAP_TILE_SIZE);
	for (int y = 0; y < yLevel; y++)
	{
		for (int x = 0; x < xLevel; x++)
		{
			int index = x + y * xLevel;

			if (map[index])
			{
				m_grid->Block({ x, y });

				Entity e;
				e.SetPosition(m_grid->At(x, y).GetWorldCoord());
				e.SetSize(MAP_TILE_SIZE, MAP_TILE_SIZE);
				e.SetTexture(&m_brickTexture);
				m_blocked.push_back(e);
			}
			else if (!placedPlayer)
			{
				m_player.SetPosition(m_grid->At(x, y).GetWorldCoord());
				m_playerSpawn = m_player.GetPosition();
				m_camera.SetPosition(m_player.GetPosition().x, m_player.GetPosition().y);
				placedPlayer = true;
			}
		}
	}


	int size = std::max(yLevel, xLevel) * (int)MAP_TILE_SIZE;

	m_quadTree.BuildTree(5, size, m_background.GetPosition());
	m_quadTree.PlaceObjects(m_blocked);
	
	Timer t;
	t.Start();
	std::vector<Waypoint> waypoints;
	_createWaypoints(waypoints, map);
	std::cout << "Time to createWaypoints: " << t.Stop(Timer::MILLISECONDS) << std::endl;

	for (auto & w: waypoints)
	{
		Entity e;
		e.SetPosition(w.GetWorldCoord() - sf::Vector2f(4, 4));
		e.SetSize(8, 8);
		e.SetColor(sf::Color::Black);
		m_waypoints.push_back(e);

		if (DRAW_LINES)
		{
			for (auto & c : w.GetConnections())
			{
				Line l;
				l.SetLine(w.GetWorldCoord(), waypoints[c.Waypoint].GetWorldCoord());
				l.SetColor(sf::Color::Blue);
				m_lines.push_back(l);
			}
		}
	}

	m_grid->SetWaypoints(waypoints, &m_quadTree);


	if (DRAW_FIELDS)
	{
		std::map<Waypoint *, sf::Color> wpToColor;

		for (int y = 0; y < m_mapHeight; y++)
		{
			for (int x = 0; x < m_mapWidth; x++)
			{
				Tile t = m_grid->At(x, y);
				Waypoint * wp;

				if ((wp = t.GetFieldOwner()) != nullptr)
				{
					Entity e;
					e.SetPosition(t.GetWorldCoord());
					e.SetSize(t.GetTileSize());

					sf::Color c;

					auto it = wpToColor.find(wp);
					if (it == wpToColor.end())
					{
						c = sf::Color(rand() % 256, rand() % 256, rand() % 256);
						wpToColor.insert(wpToColor.end(), std::pair(wp, c));
					}
					else
					{
						c = wpToColor[wp];
					}
					e.SetColor(c);

					m_field.push_back(e);

				}
			}
		}
	}

	

}

void Engine::_createWaypoints(std::vector<Waypoint>& waypoints, const std::vector<bool> & map)
{
	std::cout << "Create Wp\n";
	struct BLOCK
	{
		sf::Vector2f topLeft, topRight, bottomRight, bottomLeft;
	};

	std::vector<BLOCK> blocks;
	std::vector<bool> used(m_mapHeight * m_mapWidth);

	Timer t;
	t.Start();
	for (unsigned int y = 0; y < m_mapHeight; y++)
	{
		for (unsigned int x = 0; x < m_mapWidth; x++)
		{
			int index = x + y * m_mapWidth;
			if (map[index] /* This tile is blocked */ && !used[index])
			{
				
				bool canContinue = true;
				unsigned int stopIndex = m_mapWidth;
				unsigned int x2;
				unsigned int y2;
				for (y2 = y; y2 < m_mapHeight; y2++)
				{
					int lol = x + y2 * m_mapWidth;
					if (!map[lol] || used[lol])
					{
						break;
					}

					for (x2 = x; x2 < stopIndex; x2++)
					{
						int subIndex = x2 + y2 * m_mapWidth;
						if (!map[subIndex] || used[subIndex])
						{
							stopIndex = x2;
							break;
						}
					}
				}

				for (unsigned int y3 = y; y3 < y2; y3++)
				{
					for (unsigned int x3 = x; x3 < x2; x3++)
					{
						unsigned int subIndex = x3 + y3 * m_mapWidth;
						used[subIndex] = true;
					}
				}

				BLOCK b;
				b.topLeft = sf::Vector2f(x * MAP_TILE_SIZE, y * MAP_TILE_SIZE);
				b.topRight = sf::Vector2f(x2 * MAP_TILE_SIZE, y * MAP_TILE_SIZE);
				b.bottomRight = sf::Vector2f(x2 * MAP_TILE_SIZE, y2 * MAP_TILE_SIZE);
				b.bottomLeft = sf::Vector2f(x * MAP_TILE_SIZE, y2 * MAP_TILE_SIZE);

				blocks.push_back(b);
			}
		}
	}

	double time = t.Stop(Timer::MILLISECONDS);

	std::cout << "Build blocks time: " << time << " ms\n";

	for (unsigned int y = 0; y < m_mapHeight; y++)
	{
		for (unsigned int x = 0; x < m_mapWidth; x++)
		{
			sf::Vector2f lol = sf::Vector2f(x * MAP_TILE_SIZE, y* MAP_TILE_SIZE);

			int inside = -1;

			int counter = int('A');
			for (auto & b : blocks)
			{
				if (lol.x >= b.topLeft.x && lol.x < b.bottomRight.x
					&&
					lol.y >= b.topLeft.y && lol.y < b.bottomRight.y)
				{
					inside = counter;
					break;
				}
				counter++;
			}

			if (inside > -1)
				std::cout << char(inside);
			else
				std::cout << " ";

		}
		std::cout << std::endl;
	}


	using namespace DirectX;

	for (auto & e : blocks)
	{
		sf::Vector2f points[4] = {
			{ e.topLeft },
			{ e.topRight },
			{ e.bottomRight },
			{ e.bottomLeft }
		};

		sf::Vector2f dirs[4] = {
			{ e.topLeft + sf::Vector2f(-MAP_TILE_SIZE * 0.5f, -MAP_TILE_SIZE * 0.5f) },
			{ e.topRight + sf::Vector2f(MAP_TILE_SIZE * 0.5f, -MAP_TILE_SIZE * 0.5f)},
			{ e.bottomRight + sf::Vector2f(MAP_TILE_SIZE * 0.5f, MAP_TILE_SIZE * 0.5f)},
			{ e.bottomLeft + sf::Vector2f(-MAP_TILE_SIZE * 0.5f, MAP_TILE_SIZE * 0.5f)}
		};

		sf::Vector2f eCenter = (e.topLeft + e.bottomRight) * 0.5f;
		
		float length = XMVectorGetX(XMVector2Length(XMVectorSet(MAP_TILE_SIZE, MAP_TILE_SIZE, 0.0f, 0.0f)));

		XMFLOAT2 xmCenter = { eCenter.x, eCenter.y };

		XMVECTOR xmVecCenter = XMLoadFloat2(&xmCenter);

		for (int i = 0; i < 4; i++)
		{
			XMFLOAT2 p = { points[i].x, points[i].y };
			XMFLOAT2 d = { dirs[i].x, dirs[i].y };
			XMVECTOR dir = XMVector2Normalize(XMVectorSubtract(XMLoadFloat2(&d), XMLoadFloat2(&p)));
			XMStoreFloat2(&d, dir);
			sf::Vector2f direction = { d.x, d.y };

			Waypoint wp(points[i] + direction * length * 0.5f);

			if (_isInsideMap(wp))
			{
				Entity * e = m_quadTree.PointInsideObject(wp.GetWorldCoord());

				if (e == nullptr)
				{
					size_t size = waypoints.size();

					bool canAdd = true;

					sf::Vector2f newPos = wp.GetWorldCoord();

					for (size_t w = 0; w < size; w++)
					{
						static const float MIN_LENGTH = 1.0f;

						sf::Vector2f comparePos = waypoints[w].GetWorldCoord();

						float l = XMVectorGetX(XMVector2Length(XMVectorSet(comparePos.x - newPos.x, comparePos.y - newPos.y, 0.0f, 0.0f)));


						if (l < MIN_LENGTH)
						{
							canAdd = false;
							break;
						}

					}

					if (canAdd)
					{
						wp.SetArrayIndex((int)waypoints.size());
						waypoints.push_back(wp);
					}
				}
			}
		}
	}



	_connectWaypoints(waypoints);
}

bool Engine::_isInsideMap(const Waypoint & waypoint)
{
	sf::Vector2f pos = m_background.GetPosition();
	sf::Vector2f size = m_background.GetSize();
	sf::Vector2f wPos = waypoint.GetWorldCoord();

	if (wPos.x > pos.x && wPos.x < pos.x + size.x &&
		wPos.y > pos.y && wPos.y < pos.y + size.y)
		return true;


	return false;
}

bool Engine::_lineIntersectionBB(const sf::Vector2f & lo, const sf::Vector2f & le, const sf::Vector2f points[4])
{
	for (int i = 0; i < 4; i++)
	{
		sf::Vector2f edgeStart = points[i];
		sf::Vector2f edgeEnd = points[(i + 1) % 4];

		sf::Vector2f intersection;

		if (_lineIntersectionLine(lo, le, edgeStart, edgeEnd, intersection))
			return true;
	}

	return false;
}

bool Engine::_lineIntersectionLine(const sf::Vector2f & l1o, const sf::Vector2f & l1e, const sf::Vector2f & l2o, const sf::Vector2f & l2e, sf::Vector2f & intersectionPoint)
{
	static const float EPSILON = 0.0001f;

	sf::Vector2f b = l1e - l1o;
	sf::Vector2f d = l2e - l2o;

	float bDotDPerp = b.x * d.y - b.y * d.x;

	if (fabs(bDotDPerp) < EPSILON)
		return false;

	sf::Vector2f c = l2o - l1o;
	
	float t = (c.x * d.y - c.y * d.x) / bDotDPerp;

	if (t < 0.0f || t > 1.0f)
		return false;

	float u = (c.x * b.y - c.y * b.x) / bDotDPerp;

	if (u < 0.0f || u > 1.0f)
		return false;

	intersectionPoint = l1o + t * b;

	return true;
}

void Engine::_connectWaypoints(std::vector<Waypoint>& waypoints)
{
	using namespace DirectX;

	int size = (int)waypoints.size();

	static const float CLUSTER_DIST = MAP_TILE_SIZE * 1.5f;

	int cluster = 0;

	std::cout << std::endl;
	for (int i = 0; i < size; i++)
	{
		std::cout << "\rConnection time: " << double(i) / size << "\t%";

		for (int j = 0; j < size; j++)
		{
			
			if (i != j && !waypoints[j].HasConnectionWith(waypoints[i]))
			{
				sf::Vector2f lineStart = waypoints[i].GetWorldCoord();
				sf::Vector2f lineEnd = waypoints[j].GetWorldCoord();

				bool intersection = false;

				Entity * e = m_quadTree.DispatchRay(lineStart, lineEnd);

				if (e == nullptr)
				{
					float length = XMVectorGetX(XMVector2LengthSq(XMVectorSubtract(XMVectorSet(lineEnd.x, lineEnd.y, 0.0f, 0.0f), XMVectorSet(lineStart.x, lineStart.y, 0.0f, 0.0f))));
				
					Waypoint::Connection c(j, length);
					Waypoint::Connection c1(i, length);

					waypoints[i].AddConnection(c);
					waypoints[j].AddConnection(c1);

				}
			}
		}
	}
	std::cout << std::endl;
}

