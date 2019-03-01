#include "Waypoint.h"

Waypoint::Waypoint(float x, float y)
{
	m_worldCoord = { x, y };
}

Waypoint::Waypoint(const sf::Vector2f & worldCoord)
{
	m_worldCoord = worldCoord;
}

Waypoint::Waypoint(const Waypoint & other)
{
	_copy(other);
}

void Waypoint::SetWorldCoord(const sf::Vector2f & worldCoord)
{
	m_worldCoord = worldCoord;
}

void Waypoint::SetWorldCoord(float x, float y)
{
	SetWorldCoord({ x, y });
}

const sf::Vector2f & Waypoint::GetWorldCoord() const
{
	return m_worldCoord;
}

void Waypoint::AddConnection(const Connection & c)
{
	m_connections.push_back(c);
}

const std::vector<Waypoint::Connection>& Waypoint::GetConnections() const
{
	return m_connections;
}

Waypoint & Waypoint::operator=(const Waypoint & other)
{
	if (this != &other)
		_copy(other);

	return *this;
}

void Waypoint::_copy(const Waypoint & other)
{
	m_connections = other.m_connections;
	m_worldCoord = other.m_worldCoord;
}
