#pragma once
#include "Entity/Character.h"
#include "../Utility/Timer.h"
#include <thread>
#include <vector>
#include "Texture/Texture.h"
#include "Pathfinding/Grid.h"

class Engine
{
public:
	Engine(sf::RenderWindow * window);
	~Engine();

	void Run();

	void Update(double dt);

	void Draw();

	bool IsRunning() const;

	void Terminate();

private:
	Camera m_camera;
	const float CAMERA_MOVE_SPEED = 100.0f;
	const float CAMERA_ZOOM_SPEED = 1.0f;

	const float MAP_TILE_SIZE = 32.0f;
	const unsigned int MAP_WIDTH = 100;
	const unsigned int MAP_HEIGHT = 100;
	
	std::vector<Entity> m_map;

	Grid * m_grid;

	Character m_player;
	Texture m_texture;
	Texture m_grassTexture;
	Texture m_brickTexture;

private:
	sf::RenderWindow *	m_pWindow = nullptr;
	bool				m_running = false;
	std::thread			m_logicThread;
	std::thread			m_drawThread;

private:
	static void _logic(Engine * e);
	static void _draw(sf::RenderWindow * wnd, Engine * e);
};