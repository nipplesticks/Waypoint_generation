#include "Waypoint.h"

Waypoint::Waypoint(float x, float y)
{
	m_worldCoord = { x, y };
}

Waypoint::Waypoint(const sf::Vector2f & worldCoord)
{
	m_worldCoord = worldCoord;
}

void Waypoint::AddConnection(const Connection & c)
{
	m_connections.push_back(c);
}

const std::vector<Waypoint::Connection>& Waypoint::GetConnections() const
{
	return m_connections;
}
