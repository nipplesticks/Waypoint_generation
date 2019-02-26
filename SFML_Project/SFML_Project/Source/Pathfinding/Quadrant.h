#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <string>

class Quadrant
{
public:
	Quadrant();
	Quadrant(const Quadrant & other);
	~Quadrant();

	//void Build(const sf::Vector2f & pos, const sf::Vector2f & size, unsigned int currentLevel, unsigned int maxLevel, std::vector<Quadrant> & quadTree, size_t index);
	void Create(const sf::Vector2f & pos, const sf::Vector2f & size, unsigned int currentLevel, bool isLeaf);

	const sf::Vector2f & GetMin() const;
	const sf::Vector2f & GetMax() const;
	const unsigned int & GetLevel() const;
	const float & GetSize() const;

	bool IsLeaf() const;

	Quadrant & operator=(const Quadrant & other);

	std::string ToString() const;

private:
	sf::Vector2f m_min;
	sf::Vector2f m_max;
	float m_size;

	unsigned int m_level;

	bool m_isLeaf;
private:
	void _copy(const Quadrant & other);

};