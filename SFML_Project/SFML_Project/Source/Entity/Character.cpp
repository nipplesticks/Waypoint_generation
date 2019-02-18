#include "Character.h"

Character::Character()
{
}

Character::~Character()
{
}
#include <DirectXMath.h>
void Character::Update(double dt)
{
	if (p_pTexture)
	{
		p_time += dt;
		if (p_time > 1.0f)
		{
			p_time = 0.0;
			p_textureFrame.x++;
			if (p_textureFrame.x == p_pTexture->GetNrOfFrames().x)
			{
				p_textureFrame.x = 0;
				p_textureFrame.y++;

				if (p_textureFrame.y == p_pTexture->GetNrOfFrames().y)
				{
					p_textureFrame.y = 0;
				}
			}

			sf::IntRect r = p_pTexture->GetArea();
			r.left = p_textureFrame.x * r.width;
			r.top = p_textureFrame.y * r.height;
			p_spr.setTextureRect(r);
		}
	}

	if (!m_path.empty())
	{
		const sf::Vector2f & TILE_SIZE = Tile::GetTileSize();
		sf::Vector2f targetPosition = m_path.front().GetWorldCoord();
		sf::Vector2f myPos = GetPosition();
		

		sf::Vector2f direction = targetPosition - myPos;
		DirectX::XMFLOAT2 xmDir;
		DirectX::XMVECTOR xmVDir = DirectX::XMVectorSet(direction.x, direction.y, 0.0f, 0.0f);
		DirectX::XMStoreFloat2(&xmDir, DirectX::XMVector2Normalize(xmVDir));
		sf::Vector2f moveDir;
		moveDir.x = xmDir.x * dt * m_speed;
		moveDir.y = xmDir.y * dt * m_speed;

		if (m_path.size() > 1)
		{
			if (fabs(direction.x) < TILE_SIZE.x * 0.1f && fabs(direction.y) < TILE_SIZE.y * 0.1f)
				m_path.erase(m_path.begin());
			
		}
		else
		{
			if (fabs(direction.x) < 1.0f && fabs(direction.y) < 1.0f)
				m_path.erase(m_path.begin());
			
		}

		Translate(moveDir);
	}
}

void Character::SetPath(const std::vector<Tile>& path)
{
	m_path = path;
}

const std::vector<Tile>& Character::GetPath() const
{
	return m_path;
}

void Character::SetSpeed(float speed)
{
	m_speed = speed;
}
