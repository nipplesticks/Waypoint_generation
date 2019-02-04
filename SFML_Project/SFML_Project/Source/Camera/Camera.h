#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

class Camera
{
private:
	static Camera * s_activeCamera;

public:
	static Camera * GetActiveCamera();



public:
	Camera();
	Camera(const sf::Vector3f & position);
	Camera(float x, float y, float z = 1.0f);
	~Camera();

	void SetAsActive();

	void SetPosition(float x, float y, float z = 1.0f);
	void SetPosition(const sf::Vector3f & position);

	void Translate(float x, float y, float z = 0.0f);
	void Translate(const sf::Vector3f & translation);

	const sf::Vector3f & GetPosition();

private:
	sf::Vector3f m_position;


};