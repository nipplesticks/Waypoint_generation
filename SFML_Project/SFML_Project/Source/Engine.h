#pragma once
#include "Entity/Character.h"
#include "../Utility/Timer.h"
#include <thread>
#include <vector>
#include "Texture/Texture.h"
#include "Pathfinding/Grid.h"
#include "Pathfinding/QuadTree/QuadTree.h"
#include "Entity/Line.h"
#include <fstream>


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
	struct YesOrNo
	{
		YesOrNo()
		{
			no.setSize(sf::Vector2f(32, 32));
			yes.setSize(no.getSize());
			SetPosition(sf::Vector2f(0, 0));

			no.setFillColor(sf::Color::Red);
			no.setOutlineColor(sf::Color::Black);
			no.setOutlineThickness(-2.0f);

			yes.setFillColor(sf::Color::Green);
			yes.setOutlineColor(sf::Color::Black);
			yes.setOutlineThickness(-2.0f);
		}

		void Draw(sf::RenderWindow * wnd)
		{
			wnd->draw(no);
			wnd->draw(yes);
		}


		void SetPosition(sf::Vector2f pos)
		{
			no.setPosition(pos);
			yes.setPosition(pos + sf::Vector2f(34.0f, 0.0f));
		}

		int Pressed(sf::Vector2i mp)
		{
			if (no.getGlobalBounds().contains((sf::Vector2f)mp))
				return 1;
			else if (yes.getGlobalBounds().contains((sf::Vector2f)mp))
				return 2;

			return 0;
		}

		sf::RectangleShape no, yes;
	};


	Camera m_camera;
	const float CAMERA_MOVE_SPEED = 1000.0f;
	const float CAMERA_ZOOM_SPEED = 1.0f;
	const float MAP_TILE_SIZE = 32.0f;
	unsigned int m_mapWidth = 0;
	unsigned int m_mapHeight = 0;
	
	sf::Vector2f m_playerSpawn;

	Grid * m_grid;

	Entity m_background;

	Character m_player;
	Texture m_texture;
	Texture m_grassTexture;
	Texture m_brickTexture;

	QuadTree m_quadTree;
	
	YesOrNo m_buttons[9];

	Grid::Grid_Heuristic m_hArr[3];
	int m_currentChoice = 0;

	sf::Font m_font;
	sf::Text m_text[9];

	sf::Text m_mousePosText;

	std::string m_strings[9];

	std::pair<sf::Vector2f, sf::Vector2f> m_testPath[9];

	std::ofstream m_newPaths;

	double m_pathFindingTime = 0.0;

	Entity m_sourceTile, m_endTile;

	
	std::vector<Entity> m_blocked;
	std::vector<Entity> m_field;
	std::vector<Entity> m_waypoints;
	std::vector<Line>	m_lines;

private:
	sf::RenderWindow *	m_pWindow = nullptr;
	bool				m_running = false;

private:

	void _loadTestPath(const std::string & pathName);
	void _loadMap(const std::string & mapName);
	void _createWaypoints(std::vector<Waypoint> & waypoints, const std::vector<bool> & map);
	bool _isInsideMap(const Waypoint & waypoint);
	bool _lineIntersectionBB(const sf::Vector2f & lo, const sf::Vector2f & le, const sf::Vector2f points[4]);
	bool _lineIntersectionLine(const sf::Vector2f & l1o, const sf::Vector2f & l1e, const sf::Vector2f & l2o, const sf::Vector2f & l2e, __out sf::Vector2f & intersectionPoint);

	void _connectWaypoints(std::vector<Waypoint> & waypoints);

};