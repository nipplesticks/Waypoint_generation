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
	_loadMap("bigGameProjectGrid.txt");
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

	m_background.SetSize(xLevel * MAP_TILE_SIZE, yLevel * MAP_TILE_SIZE);
}

