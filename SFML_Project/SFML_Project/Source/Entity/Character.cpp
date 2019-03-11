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
		moveDir.x = xmDir.x * (float)dt * m_speed;
		moveDir.y = xmDir.y * (float)dt * m_speed;

		if (m_path.size() > 1)
		{
			if (fabs(direction.x) < TILE_SIZE.x * 0.1f && fabs(direction.y) < TILE_SIZE.y * 0.1f)
			{
				m_path.erase(m_path.begin());
				m_pathDraw.erase(m_pathDraw.begin());
			}
			
		}
		else
		{
			if (fabs(direction.x) < 1.0f && fabs(direction.y) < 1.0f)
			{
				m_path.erase(m_path.begin());
				m_pathDraw.erase(m_pathDraw.begin());

			}
			
		}

		Translate(moveDir);

		if (p_pTexture)
		{
			/*p_time += dt;
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
			}*/

			sf::IntRect r = p_pTexture->GetArea();
			if (xmDir.x < -0.33 && xmDir.y < -0.33f)	// UPLEFT
				p_textureFrame.y = 7;
			else if (xmDir.x > 0.33 && xmDir.y < -0.33f)	// UPRIGHT
				p_textureFrame.y = 1;
			else if (xmDir.x < -0.33 && xmDir.y >= 0.33f)	// DOWNLEFT
				p_textureFrame.y = 5;
			else if (xmDir.x > 0.33 && xmDir.y >= 0.33f)	// DOWNRIGHT
				p_textureFrame.y = 3;
			else if (xmDir.y < -0.33f)		// UP
				p_textureFrame.y = 0;
			else if (xmDir.y > 0.33f)	// DOWN
				p_textureFrame.y = 4;
			else if (xmDir.x > 0.33)	// RIGHT
				p_textureFrame.y = 2;
			else if (xmDir.x < -0.33)	// LEFT
				p_textureFrame.y = 6;

			r.left = p_textureFrame.x * r.width;
			r.top = p_textureFrame.y * r.height;
			p_spr.setTextureRect(r);
		}

	}
}

void Character::SetPath(const std::vector<Tile>& path)
{
	m_path = path;

	m_pathDraw.clear();

	for (auto & p : m_path)
	{
		Entity e;
		e.SetPosition(p.GetWorldCoord());
		e.SetSize(p.GetTileSize());
		e.SetColor(sf::Color::Red);
		m_pathDraw.push_back(e);
	}

}

const std::vector<Tile>& Character::GetPath() const
{
	return m_path;
}

void Character::SetSpeed(float speed)
{
	m_speed = speed;
}

void Character::Draw(sf::RenderWindow * wnd)
{
	for (auto & d : m_pathDraw)
	{
		d.Draw(wnd);
	}
	Entity::Draw(wnd);
}
