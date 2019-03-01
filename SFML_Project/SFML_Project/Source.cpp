#include "Source/Engine.h"
#include <vector>

int WindowProc(sf::RenderWindow::Window * wnd);

int main()
{
	sf::Vector2f gridStart(-15, -15);
	sf::Vector2f start(12, -3);
	sf::Vector2f tileSize(32, 32);

	sf::Vector2f startToSource = (start - gridStart);

	startToSource.x /= tileSize.x;
	startToSource.y /= tileSize.y;

	sf::RenderWindow window(sf::VideoMode(800, 600), "Test");

	Engine e(&window);

	std::thread lol(WindowProc, &window);

	e.Run();

	while (!lol.joinable());

	lol.join();

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
