#pragma once
#include <vector>
#include <string>
#include <SFML/System/Vector2.hpp>
#include "../../Entity/Entity.h"

class Quadrant
{
public:
	Quadrant();
	Quadrant(const Quadrant & other);
	~Quadrant();

	//void Build(const sf::Vector2f & pos, const sf::Vector2f & size, unsigned int currentLevel, unsigned int maxLevel, std::vector<Quadrant> & quadTree, size_t index);

	void Create(const sf::Vector2f & pos, float size, unsigned int currentLevel, bool isLeaf);
	void Create(float startX, float startY, float size, unsigned int currentLevel, bool isLeaf);

	void AddChild(int index);

	void SetParent(int index);

	const unsigned int * GetChildren() const;

	int GetParent() const;

	unsigned int GetNrOfChildren() const;

	const sf::Vector2f & GetMin() const;

	const sf::Vector2f & GetMax() const;

	const unsigned int & GetLevel() const;

	const float & GetSize() const;

	bool IsLeaf() const;

	Quadrant & operator=(const Quadrant & other);

	bool ContainsObjects() const;

	std::string ToString() const;

	void SetObject(const Entity * object);

	void ClearObjects();

	const std::vector<const Entity*> & GetObjects() const;

private:
	sf::Vector2f m_min;
	sf::Vector2f m_max;

	long int m_parent = -1;
	unsigned int m_children[4] = { 0 };
	unsigned int m_nrOfChildren = 0;

	float m_size;

	unsigned int m_level;

	std::vector<const Entity*> m_objects;

	bool m_isLeaf;
private:
	void _copy(const Quadrant & other);

};