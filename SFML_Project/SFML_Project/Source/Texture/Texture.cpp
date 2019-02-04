#include "Texture.h"

Texture::Texture()
{
	m_startArea.top = 0;
	m_startArea.left = 0;
	m_startArea.width = 0;
	m_startArea.height = 0;
	m_nrOfFrames.x = 0;
	m_nrOfFrames.y = 0;
}

Texture::~Texture()
{
	
}

void Texture::Load(const sf::String & path, const sf::IntRect & startArea, const sf::Vector2u & nrOfFrames)
{
	Load(path);
	m_startArea = startArea;
	m_nrOfFrames = nrOfFrames;
}

void Texture::Load(const sf::String & path)
{
	m_texture.loadFromFile(path);
}

sf::Texture * Texture::GetTexture()
{
	return &m_texture;
}

const sf::IntRect & Texture::GetArea()
{
	return m_startArea;
}

const sf::Vector2u & Texture::GetNrOfFrames()
{
	return m_nrOfFrames;
}
