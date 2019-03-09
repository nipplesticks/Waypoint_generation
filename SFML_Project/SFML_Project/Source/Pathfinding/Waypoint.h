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
		bool operator==(const Connection & other) const
		{
			return Waypoint == other.Waypoint;
		}
		bool operator==(const Waypoint * other) const
		{
			return Waypoint == other;
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
	Waypoint(const Waypoint & other);

	void SetWorldCoord(const sf::Vector2f & worldCoord);
	void SetWorldCoord(float x, float y);

	int GetCluster() const;
	void SetCluster(int cluster);

	bool GetVisited() const;
	void SetVisited(bool visited);

	bool HasConnectionWith(const Waypoint & wp) const;

	const sf::Vector2f & GetWorldCoord() const;

	void AddConnection(const Connection & c);

	const std::vector<Connection> & GetConnections() const;

	Waypoint& operator=(const Waypoint & other);

private:
	void _copy(const Waypoint & other);

private:
	sf::Vector2f m_worldCoord;
	int m_cluster = -1;
	std::vector<Connection> m_connections;
	bool m_visited = false;
};