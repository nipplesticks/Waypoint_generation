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

int Waypoint::GetCluster() const
{
	return m_cluster;
}

void Waypoint::SetCluster(int cluster)
{
	m_cluster = cluster;
}

bool Waypoint::HasConnectionWith(const Waypoint & wp) const
{
	return std::find(m_connections.begin(), m_connections.end(), &wp) != m_connections.end();
}

const sf::Vector2f & Waypoint::GetWorldCoord() const
{
	return m_worldCoord;
}

void Waypoint::AddConnection(const Connection & c)
{
	//if (std::find(m_connections.begin(), m_connections.end(), c) == m_connections.end())
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
