#pragma once

#include <glm/glm.hpp>

#include <vector>

#include "Camera.h"
#include "Timer.h"

struct Material
{
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 1.0f;
	float Metallic = 0.0f;
};

struct Sphere
{
	glm::vec3 Position{0.0f};
	float Radius = 0.5f;

	int MaterialIndex = 0;
};

class Scene
{
public:
	Scene();

	void Init(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		_ViewportWidth = viewportWidth;
		_ViewportHeight = viewportHeight;
		_camera.OnResize(viewportWidth, viewportHeight);
	}

	void OnUpdate(float ts, winrt::Windows::System::VirtualKey key, glm::vec2 point)
	{
		_camera.OnUpdate(ts, key, point);
//			_Renderer.ResetFrameIndex();
	}

	const Camera& GetCamera() { return _camera; }
public:
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;

private:
	uint32_t _ViewportWidth = 0, _ViewportHeight = 0;
	float _LastRenderTime = 0.0f;
	Camera _camera;
};