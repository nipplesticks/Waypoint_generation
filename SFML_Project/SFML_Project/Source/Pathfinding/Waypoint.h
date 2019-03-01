#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>


class Waypoint
{
public:
	struct Connection
	{
		Connection(Waypoint * _wp = nullptr, float _cost = FLT_MAX)
		{
			Waypoint = _wp;
			Cost = _cost;
		}
		Connection(const Connection & other)
		{
			_copy(other);
		}
		bool operator<(const Connection & other) const
		{
			return Cost < other.Cost;
		}
		Connection & operator=(const Connection & other)
		{
			if (this != &other)
			{
				_copy(other);
			}
			return *this;
		}

		Waypoint * Waypoint = nullptr;
		float Cost = FLT_MAX;

	private:
		void _copy(const Connection & other)
		{
			Waypoint = other.Waypoint;
			Cost = other.Cost;
		}

	};

public:
	Waypoint(float x = 0, float y = 0);
	Waypoint(const sf::Vector2f & worldCoord);

	void AddConnection(const Connection & c);

	const std::vector<Connection> & GetConnections() const;

private:
	sf::Vector2f m_worldCoord;
	std::vector<Connection> m_connections;
};