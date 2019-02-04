#include "Source/Engine.h"

int WindowProc(sf::RenderWindow::Window * wnd);


int main()
{
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
