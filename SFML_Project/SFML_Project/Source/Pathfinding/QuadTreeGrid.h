#pragma once
#include <SFML/System/Vector2.hpp>
#include "Quadrant.h"

enum Direction
{
	North = 0,
	North_East,
	East,
	South_East,
	South,
	South_West,
	West,
	North_West
};

class QuadGrid
{
public:
	QuadGrid();
	~QuadGrid();

	void BuildTree(unsigned int maxLevel, unsigned int worldSize, const sf::Vector2f & startPos);

	bool GetQuadrant(const sf::Vector2f & worldPos, unsigned int level, __out Quadrant * outQuadrant);
	
	bool GetQuadrantFrom(const Quadrant & refQuadrant, Direction dir, __out Quadrant * outQuadrant);


	std::string ToString() const;


private:
	std::vector<Quadrant> m_quadTree;
	unsigned int m_worldSize;
	unsigned int m_maximumLevel;
};

