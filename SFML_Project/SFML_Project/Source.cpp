#include "Source/Engine.h"
#include <vector>

int main()
{
	sf::Vector2f gridStart(-15, -15);
	sf::Vector2f start(12, -3);
	sf::Vector2f tileSize(32, 32);

	sf::Vector2f startToSource = (start - gridStart);

	startToSource.x /= tileSize.x;
	startToSource.y /= tileSize.y;

	sf::RenderWindow window(sf::VideoMode(1280, 720), "Test");

	Engine e(&window);

	e.Run();

	return 0;
}

