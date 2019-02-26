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

	for (unsigned int y = 0; y < MAP_HEIGHT; y++)
	{
		for (unsigned int x = 0; x < MAP_WIDTH; x++)
		{
			Entity e;
			e.SetPosition(x * MAP_TILE_SIZE, y * MAP_TILE_SIZE);
			e.SetSize(MAP_TILE_SIZE, MAP_TILE_SIZE);
			e.SetColor(sf::Color::White);
			e.SetTexture(&m_grassTexture);
			m_map.push_back(e);
		}
	}

	m_quadGrid.BuildTree(7, 100, sf::Vector2f(0, 0));

	//m_player.SetPosition(MAP_WIDTH * MAP_TILE_SIZE * 0.5f, MAP_HEIGHT * MAP_TILE_SIZE * 0.5f);
	m_player.SetPosition(0, 0);
	m_player.SetColor(255, 255, 255);
	m_player.SetTexture(&m_texture, true);
	m_player.SetSpeed(128.0f);

	m_camera.SetPosition(0, 0);

	m_grid = new Grid(sf::Vector2i(MAP_WIDTH, MAP_HEIGHT), { 0.0f, 0.0f }, {32.0f, 32.0f});

	for (unsigned int y = MAP_HEIGHT / 2 - 5; y < MAP_HEIGHT / 2 + 5; y++)
	{
		for (unsigned int x = MAP_WIDTH / 2 - 5; x < MAP_WIDTH / 2 + 5; x++)
		{
			m_map[x + y * MAP_WIDTH].SetTexture(&m_brickTexture);
			m_grid->Block(sf::Vector2i(x, y));
		}
	}

}

Engine::~Engine()
{
	m_pWindow = nullptr;

	while (!m_logicThread.joinable());
	m_logicThread.join();

	while (!m_drawThread.joinable());
	m_drawThread.join();

	delete[] m_grid;

}

void Engine::Run()
{
	m_pWindow->setActive(false);
	m_running = true;

	m_drawThread = std::thread(_draw, m_pWindow, this);
	m_logicThread = std::thread(_logic, this);
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

	if (mouseRightThisFrame && !s_mouseRightLastFrame)
	{
		std::vector<Tile> playerPath = m_player.GetPath();
		std::vector<Tile> newPath;
		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_pWindow);
		sf::Vector2u windowSize = m_pWindow->getSize();

		sf::Vector2f clickWorld;
		clickWorld.x = (((float)mousePos.x - (float)windowSize.x * 0.5f) * zoom) + m_camera.GetPosition().x;
		clickWorld.y = (((float)mousePos.y - (float)windowSize.y * 0.5f) * zoom) + m_camera.GetPosition().y;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && !playerPath.empty())
		{
			sf::Vector2f source = playerPath.back().GetWorldCoord();
			newPath = m_grid->FindPath(source, clickWorld);
			newPath.insert(newPath.begin(), playerPath.begin(), playerPath.end());
		}
		else
		{
			for (auto & t : playerPath)
			{
				int index = t.GetGridCoord().x + t.GetGridCoord().y * MAP_WIDTH;
				m_map[index].SetColor(sf::Color::White);
			}

			newPath = m_grid->FindPath(m_player.GetPosition() + m_player.GetSize() * 0.5f, clickWorld);

		}
		for (auto & t : newPath)
		{
			int index = t.GetGridCoord().x + t.GetGridCoord().y * MAP_WIDTH;
			m_map[index].SetColor(sf::Color::Red);
		}

		m_player.SetPath(newPath);
	}

	Tile t = m_grid->TileFromWorldCoords(m_player.GetPosition() + m_player.GetSize() * 0.5f);

	int index = t.GetGridCoord().x + t.GetGridCoord().y * MAP_WIDTH;

	m_map[index].SetColor(sf::Color::White);

	m_player.Update(dt);

	/*if (s_mouseRightLastFrame && timer.GetTime() > 0.1f)
	{
		timerStart = false;
		s_mouseRightLastFrame = false;
	}
	else*/

	s_mouseRightLastFrame = mouseRightThisFrame;


}

void Engine::Draw()
{
	int startX, endX, startY, endY;
	Camera * cam = Camera::GetActiveCamera();
	sf::Vector3f camPos = cam->GetPosition();
	sf::Vector2u wndSize = m_pWindow->getSize();

	startX = ((camPos.x - wndSize.x * 0.5f  * (camPos.z)) / (MAP_TILE_SIZE));
	startX = startX > 0 ? startX : 0;
	
	startY = ((camPos.y - wndSize.y * 0.5f * (camPos.z)) / (MAP_TILE_SIZE));
	startY = startY > 0 ? startY : 0;

	endX = startX + (wndSize.x * (camPos.z) / (MAP_TILE_SIZE) + 0.5f) + 2;
	endX = endX < MAP_WIDTH ? endX : MAP_WIDTH;

	endY = startY + (wndSize.y * (camPos.z) / (MAP_TILE_SIZE) + 0.5f) + 2;
	endY = endY < MAP_HEIGHT ? endY : MAP_HEIGHT;

	for (int y = startY; y < endY; y++)
	{
		for (int x = startX; x < endX; x++)
		{
			m_map[x + y * MAP_WIDTH].Draw(m_pWindow);
		}
	}

	m_player.Draw(m_pWindow);
}

bool Engine::IsRunning() const
{
	return m_pWindow && m_pWindow->isOpen() && m_running;
}

void Engine::Terminate()
{
	m_running = false;
}

void Engine::_logic(Engine * e)
{
	Timer deltaTime;
	deltaTime.Start();
	while (e->IsRunning())
	{
		e->Update(deltaTime.Stop());
	}
}



void Engine::_draw(sf::RenderWindow * wnd, Engine * e)
{
	double total = 0.0;
	int counter = 0;

	wnd->setActive();
	Timer t;
	t.Start();
	while (e->IsRunning())
	{
		counter++;
		total += t.Stop(Timer::MILLISECONDS);
		
		if (total > 1000)
		{
			std::cout << "\r" << total / counter << " ms";
			counter = 0;
			total = 0.0;
		}
		
		
		wnd->clear();

		e->Draw();

		wnd->display();
	}
}
