#include "Source/Engine.h"
#include "Source/Pathfinding/QuadTreeGrid.h"

#include "Utility/Timer.h"
#include <iostream>
#include <fstream>
#include <vector>

int WindowProc(sf::RenderWindow::Window * wnd);

int main()
{
	sf::Vector2f gridStart(-15, -15);
	sf::Vector2f start(12, -3);
	sf::Vector2f tileSize(32, 32);

	sf::Vector2f startToSource = (start - gridStart);

	QuadGrid qG;

	Timer timer;
	timer.Start();
	std::cout << "Building tree..." << std::endl;
	qG.BuildTree(3, 100, sf::Vector2f(-50.f, -50.f));

	std::cout << timer.Stop() << std::endl;

	Quadrant q;
	bool yeah = qG.GetQuadrant(sf::Vector2f(20.42f, -30.123f), 3u, &q);
	
	std::ofstream outlol;
	outlol.open("QuadTreeEXDE.txt");
	outlol << qG.ToString();
	outlol.close();

	startToSource.x /= tileSize.x;
	startToSource.y /= tileSize.y;

	sf::RenderWindow window(sf::VideoMode(800, 600), "Test");

	Engine e(&window);

	e.Run();

	WindowProc(&window);

	e.Terminate();

	return 0;
}

int WindowProc(sf::RenderWindow::Window * wnd)
{
	while (wnd->isOpen())
	{
		sf::Event event;
		while (wnd->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				wnd->close();
		}
	}

	return 0;
}
