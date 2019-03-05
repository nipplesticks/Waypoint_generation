#pragma once
#include "Quadrant.h"

enum Direction
{
	North		= 0,
	North_East	= 1,
	East		= 2,
	South_East	= 3,
	South		= 4,
	South_West	= 5,
	West		= 6,
	North_West	= 7
};

class QuadTree
{
public:
	QuadTree();
	~QuadTree();

	void BuildTree(unsigned int maxLevel, unsigned int worldSize, const sf::Vector2f & worldStart);
	void BuildTree(unsigned int maxLevel, unsigned int worldSize, float worldStartX, float worldStartY);

	void PlaceObjects(std::vector<Entity> & objectVector);

	Quadrant * GetQuadrant(const sf::Vector2f & worldPos, unsigned int level);
	
	Quadrant * GetQuadrantFrom(Quadrant * refQuadrant, Direction dir);

	unsigned int GetWorldSize() const;
	unsigned int GetMaxTreeLevel() const;

	const std::vector<Quadrant> & GetQuadrantVector() const;

	Entity * DispatchRay(const sf::Vector2f & rayStart, const sf::Vector2f & rayEnd) const;
	Entity * PointInsideObject(const sf::Vector2f & point) const;


	const Quadrant & operator[](unsigned int index);

	std::string ToString() const;

private:
	std::vector<Quadrant> m_quadTree;
	std::vector<Quadrant*> m_leafs;
	unsigned int m_worldSize;
	unsigned int m_maximumLevel;

private:
	size_t _GetQuadrantIndex(const sf::Vector2f & worldPos, unsigned int level);
	void _traverseAndPlace(Entity * e, int quadIndex);
	void _pointTraverse(const sf::Vector2f & point, int quadIndex, Entity *& ePtr) const;
	void _traverseWithRay(const sf::Vector2f & rayStart, const sf::Vector2f & rayEnd, int quadIndex, float & t, Entity *& ePtr) const;
	bool _lineWithLineIntersection(const sf::Vector2f & lineOrigin1, const sf::Vector2f & lineEnd1, const sf::Vector2f & lineOrigin2, const sf::Vector2f & lineEnd2, float & t) const;
	bool _insideRay(const sf::Vector2f & rayStart, const sf::Vector2f & rayEnd, const Quadrant & quadrant) const;
	bool _insideRay(const sf::Vector2f & rayStart, const sf::Vector2f & rayEnd, const Entity * e, float & t) const;
	bool _insideAABB(const sf::Vector2f & min, const sf::Vector2f & size, const Quadrant & quadrant) const;
	bool _insideAABB(const sf::Vector2f & min, const Quadrant & quadrant) const;
};

