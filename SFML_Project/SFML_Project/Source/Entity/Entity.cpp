#include "Entity.h"

Entity::Entity()
{
	m_position.x = 0.0f;
	m_position.y = 0.0f;
	m_textureFrame.x = 0;
	m_textureFrame.y = 0;

	m_size.x = 32.0f;
	m_size.y = 32.0f;
	m_spr.setFillColor(sf::Color::White);
}

Entity::Entity(const sf::Vector2f & position, const sf::Vector2f & size) : Entity()
{
	m_position = position;
	m_size = size;
}

Entity::~Entity()
{
}

void Entity::SetColor(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a)
{
	SetColor({ r, g, b, a });
}

void Entity::SetColor(const sf::Color & color)
{
	m_spr.setFillColor(color);
}

void Entity::SetOutlineThickness(float val)
{
	m_spr.setOutlineThickness(val);
}

void Entity::SetOutlineColor(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a)
{
	SetOutlineColor({ r, g, b, a });
}

void Entity::SetOutlineColor(const sf::Color & color)
{
	m_spr.setOutlineColor(color);
}

void Entity::SetPosition(float x, float y)
{
	SetPosition({ x, y });
}

void Entity::SetPosition(const sf::Vector2f & position)
{
	m_position = position;
}

void Entity::Translate(float x, float y)
{
	Translate({ x, y });
}

void Entity::Translate(const sf::Vector2f & translation)
{
	m_position = m_position + translation;
}

void Entity::SetSize(float x, float y)
{
	SetSize({ x, y });
}

void Entity::SetSize(const sf::Vector2f & size)
{
	m_size = size;
}

const sf::Vector2f & Entity::GetPosition() const
{
	return m_position;
}

const sf::Vector2f & Entity::GetSize() const
{
	return m_size;
}

void Entity::SetTexture(Texture * texture, bool useRect)
{
	m_pTexture = texture;
	m_spr.setTexture(m_pTexture->GetTexture());
	
	if (useRect)
	{
		m_spr.setTextureRect(m_pTexture->GetArea());
	}
}

void Entity::Update(double dt)
{
	if (m_pTexture)
	{
		m_time += dt;
		if (m_time > 1.0f)
		{
			m_time = 0.0;
			m_textureFrame.x++;
			if (m_textureFrame.x == m_pTexture->GetNrOfFrames().x)
			{
				m_textureFrame.x = 0;
				m_textureFrame.y++;
				
				if (m_textureFrame.y == m_pTexture->GetNrOfFrames().y)
				{
					m_textureFrame.y = 0;
				}
			}

			sf::IntRect r = m_pTexture->GetArea();
			r.left = m_textureFrame.x * r.width;
			r.top = m_textureFrame.y * r.height;
			m_spr.setTextureRect(r);
		}
	}


}

void Entity::Draw(sf::RenderWindow * wnd)
{
	if (wnd)
	{
		sf::Vector2u ws = wnd->getSize();
		sf::FloatRect screenBox;
		screenBox.left = 0.0f;
		screenBox.top = 0.0f;
		screenBox.height = ws.y;
		screenBox.width = ws.x;

		_calcScreenPosition(screenBox);

		if (m_insideScreen)
			wnd->draw(m_spr);
	}
}

void Entity::_calcScreenPosition(const sf::FloatRect & screenBox)
{
	Camera * cam = Camera::GetActiveCamera();
	m_insideScreen = false;

	if (cam)
	{
		sf::Vector3f camPos = cam->GetPosition();
		sf::Vector2f screenSize = m_size * (1.0f / camPos.z);
		sf::Vector2f screenPos = m_position - sf::Vector2f(camPos.x , camPos.y);
		screenPos *= (1.0f / camPos.z);
		screenPos.x += screenBox.width * 0.5f;
		screenPos.y += screenBox.height * 0.5f;

		m_spr.setSize(screenSize);
		m_spr.setPosition(screenPos);

		sf::FloatRect boundingBox = m_spr.getGlobalBounds();
		if (boundingBox.intersects(screenBox))
			m_insideScreen = true;
	}
}
