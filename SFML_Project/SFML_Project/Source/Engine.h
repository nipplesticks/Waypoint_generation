#pragma once
#include "Entity/Character.h"
#include "../Utility/Timer.h"
#include <thread>
#include <vector>
#include "Texture/Texture.h"
#include "Pathfinding/Grid.h"
#include "Pathfinding/QuadTree/QuadTree.h"
#include "Entity/Line.h"

class Engine
{
public:
	Engine(sf::RenderWindow * window);
	~Engine();

	void Run();

	void Update(double dt);

	void Draw(bool clearAndDisplay = true);

	bool IsRunning() const;

	void Terminate();

private:
	Camera m_camera;
	const float CAMERA_MOVE_SPEED = 1000.0f;
	const float CAMERA_ZOOM_SPEED = 1.0f;
	const float MAP_TILE_SIZE = 32.0f;
	unsigned int m_mapWidth = 0;
	unsigned int m_mapHeight = 0;
	
	Grid * m_grid;

	Entity m_background;

	Character m_player;
	Texture m_texture;
	Texture m_grassTexture;
	Texture m_brickTexture;

	QuadTree m_quadTree;

	

	std::vector<Entity> m_blocked;
	std::vector<Entity> m_field;
	std::vector<Entity> m_waypoints;
	std::vector<Line>	m_lines;

private:
	sf::RenderWindow *	m_pWindow = nullptr;
	bool				m_running = false;

private:

	void _loadMap(const std::string & mapName);
	void _createWaypoints(std::vector<Waypoint> & waypoints, const std::vector<bool> & map);
	bool _isInsideMap(const Waypoint & waypoint);
	bool _lineIntersectionBB(const sf::Vector2f & lo, const sf::Vector2f & le, const sf::Vector2f points[4]);
	bool _lineIntersectionLine(const sf::Vector2f & l1o, const sf::Vector2f & l1e, const sf::Vector2f & l2o, const sf::Vector2f & l2e, __out sf::Vector2f & intersectionPoint);

	void _connectWaypoints(std::vector<Waypoint> & waypoints);

};