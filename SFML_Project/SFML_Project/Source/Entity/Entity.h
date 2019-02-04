#pragma once
#include "../Camera/Camera.h"
#include "../Texture/Texture.h"

class Entity
{
public:
	Entity();
	Entity(const sf::Vector2f & position, const sf::Vector2f & size);
	~Entity();

	void SetColor(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a = 255.0f);
	void SetColor(const sf::Color & color);

	void SetOutlineThickness(float val);
	void SetOutlineColor(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a = 255.0f);
	void SetOutlineColor(const sf::Color & color);

	void SetPosition(float x, float y);
	void SetPosition(const sf::Vector2f & position);

	void Translate(float x, float y);
	void Translate(const sf::Vector2f & translation);

	void SetSize(float x, float y);
	void SetSize(const sf::Vector2f & size);

	const sf::Vector2f & GetPosition() const;
	const sf::Vector2f & GetSize() const;

	void SetTexture(Texture * texture, bool useRect = false);
	

	void Update();

	void Draw(sf::RenderWindow * wnd);

private:
	sf::RectangleShape m_spr;
	sf::Vector2f m_position;
	sf::Vector2f m_size;
	Texture *	m_pTexture = nullptr;
	bool m_insideScreen = false;

private:
	void _calcScreenPosition(const sf::FloatRect & screenBox);


};