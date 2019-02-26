#include "Quadrant.h"

Quadrant::Quadrant()
{
	m_min = sf::Vector2f(0.0f ,0.0f);
	m_max = sf::Vector2f(0.0f, 0.0f);
	m_level = 0;
	m_isLeaf = true;
}

Quadrant::Quadrant(const Quadrant & other)
{
	_copy(other);
}

Quadrant::~Quadrant()
{
	
}

void Quadrant::Build(const sf::Vector2f & pos, const sf::Vector2f & size, unsigned int currentLevel, unsigned int maxLevel, std::vector<Quadrant> & quadTree)
{
	m_min = pos;
	m_max = pos + size;
	m_size = m_max.x - m_min.x;
	m_level = currentLevel;
	quadTree.push_back(*this);
	if (maxLevel == currentLevel)
		m_isLeaf = true;
	else
	{
		sf::Vector2f subSize = size * 0.5f;
		currentLevel++;

		for (int y = 0; y < 2; y++)
		{
			for (int x = 0; x < 2; x++)
			{
				Quadrant q;
				sf::Vector2f subPos;
				subPos.x = pos.x + subSize.x * x;
				subPos.y = pos.y + subSize.y * y;
				q.Build(subPos, subSize, currentLevel, maxLevel, quadTree);
			}
		}
	}
}

const sf::Vector2f & Quadrant::GetMin() const
{
	return m_min;
}

const sf::Vector2f & Quadrant::GetMax() const
{
	return m_max;
}

const unsigned int & Quadrant::GetLevel() const
{
	return m_level;
}

const float & Quadrant::GetSize() const
{
	return m_size;
}

bool Quadrant::IsLeaf() const
{
	return m_isLeaf;
}

Quadrant & Quadrant::operator=(const Quadrant & other)
{
	if (this != &other)
	{
		_copy(other);
	}


	return *this;
}

std::string Quadrant::ToString() const
{
	std::string str = "";

	str += "Min: " + std::to_string(m_min.x) + " " + std::to_string(m_min.y) + "\n";
	str += "Max: " + std::to_string(m_max.x) + " " + std::to_string(m_max.y) + "\n";
	str += "Size: " + std::to_string(m_size) + "\n";
	str += "Level: " + std::to_string(m_level) + "\n";
	str += "Leaf: ";
	str += m_isLeaf ? "True" : "False";
	str += "\n";

	return str;
}

void Quadrant::_copy(const Quadrant & other)
{
	m_min = other.m_min;
	m_max = other.m_max;
	m_size = other.m_size;
	m_level = other.m_level;
	m_isLeaf = other.m_isLeaf;
}
