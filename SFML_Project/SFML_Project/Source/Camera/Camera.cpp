#include "Camera.h"

Camera * Camera::s_activeCamera = nullptr;


Camera * Camera::GetActiveCamera()
{
	return s_activeCamera;
}

Camera::Camera()
{
	if (s_activeCamera == nullptr)
		s_activeCamera = this;

	m_position.x = 0.0f;
	m_position.y = 0.0f;
	m_position.z = 1.0f;
}

Camera::Camera(const sf::Vector3f & position) : Camera()
{
	SetPosition(position);
}

Camera::Camera(float x, float y, float z)
{
	SetPosition(x, y, z);
}

Camera::~Camera()
{
	if (this == s_activeCamera)
		s_activeCamera = nullptr;
}

void Camera::SetAsActive()
{
	s_activeCamera = this;
}

void Camera::SetPosition(float x, float y, float z)
{
	SetPosition({ x, y, z });
}

void Camera::SetPosition(const sf::Vector3f & position)
{
	m_position = position;
	m_position.z = m_position.z > 0.01f ? m_position.z : 0.01f;
}

void Camera::Translate(float x, float y, float z)
{
	Translate({ x, y, z });
}

void Camera::Translate(const sf::Vector3f & translation)
{
	m_position = m_position + translation;
	m_position.z = m_position.z > 0.01f ? m_position.z : 0.01f;
}

const sf::Vector3f & Camera::GetPosition()
{
	return m_position;
}
