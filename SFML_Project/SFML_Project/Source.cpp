#include <SFML/Graphics.hpp>
#include "Utility/Timer.h"


int Run(sf::RenderWindow * wnd);


int main()
{
	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");


	window.setActive(false);

	sf::Thread renderThread(&Run, &window);
	renderThread.launch();


	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
	}


	renderThread.terminate();

	return 0;
}

int Run(sf::RenderWindow * wnd)
{
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	wnd->setActive();

	while (wnd->isOpen())
	{




		wnd->clear();
		wnd->draw(shape);
		wnd->display();
	}

	return 0;
}