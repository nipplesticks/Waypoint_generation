#pragma once
#include <SFML/Graphics.hpp>

class Texture
{
public:
	Texture();
	~Texture();

	void Load(const sf::String & path, const sf::IntRect & startArea, const sf::Vector2u & nrOfFrames);
	void Load(const sf::String & path);

	sf::Texture *		 GetTexture();
	const sf::IntRect &  GetArea();
	const sf::Vector2u & GetNrOfFrames();

private:
	sf::Texture m_texture;
	sf::IntRect  m_startArea;
	sf::Vector2u m_nrOfFrames;
};