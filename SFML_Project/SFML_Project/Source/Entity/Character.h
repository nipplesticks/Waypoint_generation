#pragma once
#include "Entity.h"
#include "../Pathfinding/Tile.h"


class Character : public Entity
{
public:
	Character();
	~Character();

	void Update(double dt);
	void SetPath(const std::vector<Tile> & path);
	const std::vector<Tile> & GetPath() const;
	void SetSpeed(float speed);
	void Draw(sf::RenderWindow * wnd) override;

private:
	std::vector<Tile> m_path;
	std::vector<Entity> m_pathDraw;
	float m_speed = 100.0f;
};