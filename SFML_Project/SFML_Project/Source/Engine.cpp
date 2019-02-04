#include "Engine.h"
#include <iostream>
Engine::Engine(sf::RenderWindow * window)
{
	m_pWindow = window;

	m_camera.SetAsActive();


	for (unsigned int y = 0; y < MAP_HEIGHT; y++)
	{
		for (unsigned int x = 0; x < MAP_WIDTH; x++)
		{
			Entity e;
			e.SetPosition(x * MAP_TILE_SIZE, y * MAP_TILE_SIZE);
			e.SetSize(MAP_TILE_SIZE, MAP_TILE_SIZE);
			e.SetColor(255 * x / MAP_WIDTH, 255 * y / MAP_HEIGHT, 255 * (x * y) / (MAP_WIDTH * MAP_HEIGHT));
			e.SetOutlineThickness(-1.0f);
			e.SetOutlineColor(sf::Color::Black);
			m_map.push_back(e);
		}
	}

	sf::IntRect iRect(0, 0, 32, 32);

	m_texture.Load("../Assets/Test.bmp", iRect, {2, 2});
	
	m_player.SetPosition(MAP_WIDTH * MAP_TILE_SIZE * 0.5f, MAP_HEIGHT * MAP_TILE_SIZE * 0.5f);
	m_player.SetColor(255, 255, 255);
	m_player.SetTexture(&m_texture, true);

	m_camera.SetPosition(MAP_WIDTH * MAP_TILE_SIZE * 0.5f, MAP_HEIGHT * MAP_TILE_SIZE * 0.5f);
}

Engine::~Engine()
{
	m_pWindow = nullptr;

	while (!m_logicThread.joinable());
	m_logicThread.join();

	while (!m_drawThread.joinable());
	m_drawThread.join();

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
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		m_player.Translate(CAMERA_MOVE_SPEED * dt, 0.0f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		m_player.Translate(-CAMERA_MOVE_SPEED * dt, 0.0f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		m_player.Translate(0.0f, -CAMERA_MOVE_SPEED * dt);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		m_player.Translate(0.0f, CAMERA_MOVE_SPEED * dt);

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
