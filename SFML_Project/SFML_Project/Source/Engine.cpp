#include "Engine.h"
#include <iostream>
#include <fstream>

Engine::Engine(sf::RenderWindow * window)
{
	m_pWindow = window;

	m_camera.SetAsActive();
	sf::IntRect iRect(0, 0, 32, 32);

	m_texture.Load("../Assets/Test.bmp", iRect, {1, 8});
	m_grassTexture.Load("../Assets/Grass.bmp", iRect, {1, 1});
	m_brickTexture.Load("../Assets/Brick.bmp", iRect, {1, 1});

	m_background.SetTexture(&m_grassTexture);

	m_background.SetPosition(0, 0);

	m_player.SetPosition(0, 0);
	m_player.SetColor(255, 255, 255);
	m_player.SetTexture(&m_texture, true);
	m_player.SetSpeed(128.0f);

	m_camera.SetPosition(0, 0);

	//m_grid = new Grid(sf::Vector2i(MAP_WIDTH, MAP_HEIGHT), { 0.0f, 0.0f }, { 32.0f, 32.0f });
	//m_background.SetSize(MAP_WIDTH * MAP_TILE_SIZE, MAP_HEIGHT * MAP_TILE_SIZE);
	_loadMap("UMAP.txt");
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

		Update(deltaTime.Stop() - eventTimer.Stop());

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
	static bool s_mouseRightLastFrame = false;
	static bool timerStart = false;
	static Timer timer;

	if (s_mouseRightLastFrame && !timerStart)
	{
		timer.Start();
		timerStart = true;
	}

	float zoom = m_camera.GetPosition().z;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		m_camera.Translate(CAMERA_MOVE_SPEED * zoom * dt, 0.0f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		m_camera.Translate(-CAMERA_MOVE_SPEED * zoom * dt, 0.0f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		m_camera.Translate(0.0f, -CAMERA_MOVE_SPEED * zoom * dt);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		m_camera.Translate(0.0f, CAMERA_MOVE_SPEED * zoom * dt);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
		m_camera.Translate(0.0f, 0.0f, -CAMERA_ZOOM_SPEED * dt);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		m_camera.Translate(0.0f, 0.0f, CAMERA_ZOOM_SPEED * dt);

	bool mouseRightThisFrame = sf::Mouse::isButtonPressed(sf::Mouse::Right);

	double time = 0;

	if (mouseRightThisFrame && !s_mouseRightLastFrame)
	{
		Timer t;
		t.Start();
		std::vector<Tile> playerPath = m_player.GetPath();
		std::vector<Tile> newPath;
		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_pWindow);
		sf::Vector2u windowSize = m_pWindow->getSize();

		sf::Vector2f clickWorld;
		clickWorld.x = (((float)mousePos.x - (float)windowSize.x * 0.5f) * zoom) + m_camera.GetPosition().x;
		clickWorld.y = (((float)mousePos.y - (float)windowSize.y * 0.5f) * zoom) + m_camera.GetPosition().y;

		Timer pathTime;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && !playerPath.empty())
		{
			
			sf::Vector2f source = playerPath.back().GetWorldCoord();
			newPath = m_grid->FindPath(source, clickWorld);
			newPath.insert(newPath.begin(), playerPath.begin(), playerPath.end());
		}
		else
		{
			
			newPath = m_grid->FindPath(m_player.GetPosition() + m_player.GetSize() * 0.5f, clickWorld, m_pWindow, this);

		}
		
		time = t.Stop(Timer::MILLISECONDS);
		if (!newPath.empty())
		{
			std::cout << "Path: [" << sfVecToString(m_player.GetPosition())  << "] --> [" <<sfVecToString(newPath.back().GetWorldCoord()) << "]\n";
			std::cout << "Time: " << std::to_string(time) << " ms\n\n";
		}

		m_player.SetPath(newPath);
	}
	
	m_player.Update(dt);

	s_mouseRightLastFrame = mouseRightThisFrame;


}

void Engine::Draw(bool clearAndDisplay)
{
	if (clearAndDisplay)
		m_pWindow->clear();
	int startX, endX, startY, endY;
	Camera * cam = Camera::GetActiveCamera();
	sf::Vector3f camPos = cam->GetPosition();
	sf::Vector2u wndSize = m_pWindow->getSize();

	m_background.Draw(m_pWindow);

	for (auto & b : m_blocked)
		b.Draw(m_pWindow);

	m_player.Draw(m_pWindow);

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

#include <fstream>
void Engine::_loadMap(const std::string & mapName)
{
	std::ifstream mapText;
	mapText.open("../Assets/" + mapName);

	int yLevel = 0;
	int xLevel = 0;
	
	std::vector<bool> map;
	std::string row;


	while (std::getline(mapText, row))
	{
		xLevel = row.size();

		for (int i = 0; i < xLevel; i++)
		{
			map.push_back(row[i] == '#');
		}

		yLevel++;
	}

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
				m_camera.SetPosition(m_player.GetPosition().x, m_player.GetPosition().y);
				placedPlayer = true;
			}
		}
	}
	sf::RectangleShape s;

	std::vector<Waypoint> waypoints;
	_createWaypoints(waypoints);
	m_grid->SetWaypoints(waypoints);


}
#include <DirectXMath.h>
void Engine::_createWaypoints(std::vector<Waypoint>& waypoints)
{
	using namespace DirectX;

	for (auto & e : m_blocked)
	{
		sf::Vector2f ePos = e.GetPosition();
		sf::Vector2f eSize = e.GetSize();
		sf::Vector2f points[4] = {
			{ ePos },
			{ ePos + sf::Vector2f(eSize.x, 0.0f) },
			{ ePos + eSize },
			{ ePos + sf::Vector2f(0.0f, eSize.y) },
		};
		sf::Vector2f eCenter = ePos + eSize * 0.5f;
		
		float length = XMVectorGetX(XMVector2Length(XMVectorSet(eSize.x, eSize.y, 0.0f, 0.0f)));

		XMFLOAT2 xmCenter = { eCenter.x, eCenter.y };

		XMVECTOR xmVecCenter = XMLoadFloat2(&xmCenter);

		for (int i = 0; i < 4; i++)
		{
			XMFLOAT2 p = { points[i].x, points[i].y };
			XMVECTOR dir = XMVector2Normalize(XMVectorSubtract(XMLoadFloat2(&p), xmVecCenter));
			XMStoreFloat2(&p, dir);
			sf::Vector2f direction = { p.x, p.y };

			Waypoint wp(points[i] + direction * length * 0.5f);

			if (_isInsideMap(wp) && !_intersectsWithBlock(wp))
				waypoints.push_back(wp);
		}
	}

	_connectWaypoints(waypoints);

}

bool Engine::_intersectsWithBlock(const Waypoint & waypoint)
{
	for (auto & e : m_blocked)
	{
		sf::Vector2f pos = e.GetPosition();
		sf::Vector2f size = e.GetSize();
		sf::Vector2f wPos = waypoint.GetWorldCoord();
		
		if (wPos.x > pos.x && wPos.x < pos.x + size.x &&
			wPos.y > pos.y && wPos.y < pos.y + size.y)
			return true;
	}

	return false;
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
	// TODO:: Create lines from each waypoint to each waypoint, if they dont intersect with anything, this is a gogo!!!

	int size = waypoints.size();

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (i != j && !waypoints[j].HasConnectionWith(waypoints[i]))
			{
				sf::Vector2f lineStart = waypoints[i].GetWorldCoord();
				sf::Vector2f lineEnd = waypoints[j].GetWorldCoord();

				bool intersection = false;

				for (auto & e : m_blocked)
				{
					sf::Vector2f ePos = e.GetPosition();
					sf::Vector2f eSize = e.GetSize();
					sf::Vector2f points[4] = {
						{ ePos },
						{ ePos + sf::Vector2f(eSize.x, 0.0f) },
						{ ePos + eSize },
						{ ePos + sf::Vector2f(0.0f, eSize.y) },
					};

					if (intersection = _lineIntersectionBB(lineStart, lineEnd, points))
						break;
				}

				if (!intersection)
				{
					float length = XMVectorGetX(XMVector2Length(XMVectorSubtract(XMVectorSet(lineEnd.x, lineEnd.y, 0.0f, 0.0f), XMVectorSet(lineStart.x, lineStart.y, 0.0f, 0.0f))));
					Waypoint::Connection c(&waypoints[j], length);
					Waypoint::Connection c1(&waypoints[i], length);

					waypoints[i].AddConnection(c);
					waypoints[j].AddConnection(c1);
				}
			}
		}
	}
}

