#include "Line.h"
#include "../Camera/Camera.h"
Line::Line()
{
}

Line::Line(const sf::Vector2f & start, const sf::Vector2f & end)
{
	SetLine(start, end);
}

Line::Line(const Line & other)
{
	_copy(other);
}

Line::~Line()
{
}

void Line::SetLine(const sf::Vector2f & start, const sf::Vector2f & end)
{
	m_position[0] = start;
	m_position[1] = end;
}

void Line::SetColor(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a)
{
	SetColor({ r, g, b, a });
}

void Line::SetColor(const sf::Color & color)
{
	m_line[0].color = color;
	m_line[1].color = color;
}

void Line::SetColor(const sf::Color color[2])
{
	m_line[0].color = color[0];
	m_line[1].color = color[1];
}

void Line::Draw(sf::RenderWindow * window)
{
	if (window)
	{
		sf::Vector2u ws = window->getSize();
		sf::FloatRect screenBox;
		screenBox.left = 0.0f;
		screenBox.top = 0.0f;
		screenBox.height = ws.y;
		screenBox.width = ws.x;

		_calcScreenPosition(screenBox);
		if (m_insideScreen)
			window->draw(m_line, 2, sf::Lines);
	}


}

Line & Line::operator=(const Line & other)
{
	if (this != &other)
		_copy(other);

	return *this;
}

void Line::_calcScreenPosition(const sf::FloatRect & screenBox)
{
	Camera * cam = Camera::GetActiveCamera();
	m_insideScreen = false;

	if (cam)
	{
		sf::Vector3f camPos = cam->GetPosition();

		sf::Vector2f dist = m_position[1] - m_position[0];
		sf::Vector2f middlePoint = (m_position[1] + m_position[0]) * 0.5f;
		sf::Vector2f screenDist = dist * (1.0f / camPos.z);

		sf::Vector2f screenPos = middlePoint - sf::Vector2f(camPos.x, camPos.y);
		screenPos *= (1.0f / camPos.z);
		screenPos.x += screenBox.width * 0.5f;
		screenPos.y += screenBox.height * 0.5f;

		m_line[0].position = screenPos - (screenDist * 0.5f);
		m_line[1].position = screenPos + (screenDist * 0.5f);

		//if (screenBox.contains(m_line[0].position) || screenBox.contains(m_line[1].position))
		m_insideScreen = true;
	}
}

void Line::_copy(const Line & other)
{
	m_line[0] = other.m_line[0];
	m_line[1] = other.m_line[1];
	m_position[0] = other.m_position[0];
	m_position[1] = other.m_position[1];
	m_insideScreen = other.m_insideScreen;
}
