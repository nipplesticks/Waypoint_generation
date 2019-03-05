#include "Quadrant.h"

Quadrant::Quadrant()
{
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

void Quadrant::Create(const sf::Vector2f & pos, float size, unsigned int currentLevel, bool isLeaf)
{
	m_min = pos;
	m_max = pos + sf::Vector2f(size, size);
	m_size = size;
	m_level = currentLevel;
	m_isLeaf = isLeaf;
}

void Quadrant::Create(float startX, float startY, float size, unsigned int currentLevel, bool isLeaf)
{

}

void Quadrant::AddChild(int index)
{
	if (m_nrOfChildren < 4)
		m_children[m_nrOfChildren++] = index;
}

void Quadrant::SetParent(int index)
{
	if (m_parent == -1)
		m_parent = index;
}

const unsigned int * Quadrant::GetChildren() const
{
	return m_children;
}

int Quadrant::GetParent() const
{
	return m_parent;
}

unsigned int Quadrant::GetNrOfChildren() const
{
	return m_nrOfChildren;
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

bool Quadrant::ContainsObjects() const
{
	return !m_objects.empty();
}
#include <sstream>
std::string Quadrant::ToString() const
{
	std::string str = "";

	str += "Min: " + std::to_string(m_min.x) + ", " + std::to_string(m_min.y) + "\n";
	str += "Max: " + std::to_string(m_max.x) + ", " + std::to_string(m_max.y) + "\n";
	str += "Size: " + std::to_string(m_size) + "\n";
	str += "Level: " + std::to_string(m_level) + "\n";
	str += "Leaf: ";
	str += m_isLeaf ? "True" : "False";
	str += "\n";
	str += "ParentIndex: " + std::to_string(m_parent) + "\n";
	str += "ChildrenIndices: ";
	for (unsigned int i = 0; i < m_nrOfChildren; i++)
		str += std::to_string(m_children[i]) + ", ";
	str += '\n';
	str += "NrOfObjects: " + std::to_string(m_objects.size()) + "\n";
	str += "Entity Addresses: ";
	int size = m_objects.size();


	for (int i = 0; i < size; i++)
	{
		std::stringstream ss;
		ss << m_objects[i];
		str += ss.str() + ", ";
	}
	str += "\n";

	return str;
}

void Quadrant::SetObject(const Entity * object)
{
	m_objects.push_back(object);
}

void Quadrant::ClearObjects()
{
	m_objects.clear();
}

const std::vector<const Entity*>& Quadrant::GetObjects() const
{
	return m_objects;
}

void Quadrant::_copy(const Quadrant & other)
{
	m_min = other.m_min;
	m_max = other.m_max;
	m_size = other.m_size;
	m_level = other.m_level;
	m_isLeaf = other.m_isLeaf;
	m_parent = other.m_parent;
	m_nrOfChildren = other.m_nrOfChildren;
	for (int i = 0; i < 4; i++)
	{
		m_children[i] = other.m_children[i];
	}
}
