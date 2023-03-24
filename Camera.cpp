#include "pch.h"

#include "Camera.h"
#include "Log.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <winrt/Microsoft.UI.Input.h>
//using namespace winrt::Butternut::implementation;

Camera::Camera(float verticalFOV, float nearClip, float farClip)
	: m_VerticalFOV(verticalFOV), m_NearClip(nearClip), m_FarClip(farClip)
{
	m_ForwardDirection = glm::vec3(0, 0, 1);
	m_Position = glm::vec3(0, 0, -6);
}

bool Camera::OnUpdate(float ts, winrt::Windows::System::VirtualKey key, winrt::Microsoft::UI::Input::PointerPoint point)
{
	bool moved = false;

	glm::vec2 mousePos;
	mousePos.x = point.Position().X;
	mousePos.y = point.Position().Y;

	glm::vec2 delta = (mousePos - m_LastMousePosition) * 0.002f;
	m_LastMousePosition = mousePos;

	const glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
	glm::vec3 rightDirection = glm::cross(m_ForwardDirection, upDirection);

	float speed = 1.0f;

 //Movement
	if (key == winrt::Windows::System::VirtualKey::W)
	{
		m_Position += m_ForwardDirection * speed * ts;
		ML_TRACE("Moving forwards.\n");
		moved = true;
	}
	else if (key == winrt::Windows::System::VirtualKey::S)
	{
		m_Position -= m_ForwardDirection * speed * ts;
		ML_TRACE("Moving backwards.\n");
		moved = true;
	}

	if (key == winrt::Windows::System::VirtualKey::A)
	{
		m_Position -= rightDirection * speed * ts;
		ML_TRACE("Moving left.\n");
		moved = true;
	}
	else if (key == winrt::Windows::System::VirtualKey::D)
	{
		m_Position += rightDirection * speed * ts;
		ML_TRACE("Moving right.\n");
		moved = true;
	}

	if (key == winrt::Windows::System::VirtualKey::Q)
	{
		m_Position -= upDirection * speed * ts;
		ML_TRACE("Moving up.\n");
		moved = true;
	}
	else if (key == winrt::Windows::System::VirtualKey::E)
	{
		m_Position += upDirection * speed * ts;
		ML_TRACE("Moving down.\n");
		moved = true;
	}

	//Rotation
	if (delta.x != 0.0f || delta.y != 0.0f)
	{
		float pitchDelta = delta.y * GetRotationSpeed();
		float yawDelta = delta.x * GetRotationSpeed();

		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
			glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));
		m_ForwardDirection = glm::rotate(q, m_ForwardDirection);

		moved = true;
	}

	if (moved)
	{
		RecalculateView();
		RecalculateRayDirections();
	}

	return moved;
}

void Camera::OnResize(uint32_t width, uint32_t height)
{
	if (width == m_ViewportWidth && height == m_ViewportHeight)
		return;

	m_ViewportWidth = width;
	m_ViewportHeight = height;

	RecalculateProjection();
	RecalculateRayDirections();
}

float Camera::GetRotationSpeed()
{
	return 0.3f;
}

void Camera::RecalculateProjection()
{
	m_Projection = glm::perspectiveFov(glm::radians(m_VerticalFOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearClip, m_FarClip);
	m_Projection[1][1] *= -1.0f;
	m_InverseProjection = glm::inverse(m_Projection);
}

void Camera::RecalculateView()
{
	m_View = glm::lookAt(m_Position, m_Position + m_ForwardDirection, glm::vec3(0, 1, 0));
	m_InverseView = glm::inverse(m_View);
}

void Camera::RecalculateRayDirections()
{
	m_RayDirections.resize(m_ViewportWidth * m_ViewportHeight);

	for (uint32_t y = 0; y < m_ViewportHeight; y++)
	{
		for (uint32_t x = 0; x < m_ViewportWidth; x++)
		{
			glm::vec2 coord = { (float)x / (float)m_ViewportWidth, (float)y / (float)m_ViewportHeight };
			coord = coord * 2.0f - 1.0f; // -1 -> 1

			glm::vec4 target = m_InverseProjection * glm::vec4(coord.x, coord.y, -1, 1);
			glm::vec3 rayDirection = glm::vec3(m_InverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
			m_RayDirections[x + y * m_ViewportWidth] = rayDirection;
		}
	}
}
