#pragma once
#include <SFML/Graphics.hpp>

class Line
{
public:
	Line();
	Line(const sf::Vector2f & start, const sf::Vector2f & end);
	Line(const Line & other);

	virtual ~Line();

	void SetLine(const sf::Vector2f & start, const sf::Vector2f & end);
	void SetColor(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a = 255.0f);
	void SetColor(const sf::Color & color);
	void SetColor(const sf::Color color[2]);

	void Draw(sf::RenderWindow * window);

	Line & operator=(const Line & other);

private:
	sf::Vertex m_line[2] = {};
	sf::Vector2f m_position[2] = {};
	bool m_insideScreen = false;

	void _calcScreenPosition(const sf::FloatRect & screenBox);

	void _copy(const Line & other);
};