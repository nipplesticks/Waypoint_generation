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

	m_background.SetSize(MAP_WIDTH * MAP_TILE_SIZE, MAP_HEIGHT* MAP_TILE_SIZE);
	m_background.SetPosition(0, 0);

	m_player.SetPosition(0, 0);
	m_player.SetColor(255, 255, 255);
	m_player.SetTexture(&m_texture, true);
	m_player.SetSpeed(128.0f);

	m_camera.SetPosition(0, 0);

	m_grid = new Grid(sf::Vector2i(MAP_WIDTH, MAP_HEIGHT), { 0.0f, 0.0f }, {32.0f, 32.0f});

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
	double total = 0.0;
	int counter = 0;
	m_pWindow->setActive(false);
	m_running = true;
	Timer deltaTime;
	Timer deltaTime2;
	deltaTime.Start();
	deltaTime2.Start();
	while (m_running)
	{
		counter++;
		Update(deltaTime.Stop());
		
		Draw();
		
		
		total += deltaTime2.Stop(Timer::MILLISECONDS);

		if (total > 1000)
		{
			std::cout << "\r" << total / counter << " ms";
			counter = 0;
			total = 0.0;
		}	
	}
	m_pWindow->close();
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

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		Terminate();

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
			
			newPath = m_grid->FindPath(m_player.GetPosition() + m_player.GetSize() * 0.5f, clickWorld);

		}
		
		m_player.SetPath(newPath);
	}

	Tile t = m_grid->TileFromWorldCoords(m_player.GetPosition() + m_player.GetSize() * 0.5f);

	m_player.Update(dt);

	s_mouseRightLastFrame = mouseRightThisFrame;


}

void Engine::Draw()
{
	m_pWindow->setActive();
	m_pWindow->clear();
	int startX, endX, startY, endY;
	Camera * cam = Camera::GetActiveCamera();
	sf::Vector3f camPos = cam->GetPosition();
	sf::Vector2u wndSize = m_pWindow->getSize();

	m_background.Draw(m_pWindow);

	m_player.Draw(m_pWindow);

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

