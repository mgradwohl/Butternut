#pragma once

#include <memory>

#include <winrt/Microsoft.Graphics.Canvas.h>
#include <glm/glm.hpp>

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

class Renderer
{
public:
	struct Settings
	{
		bool Accumulate = true;
	};

public:
	Renderer() = default;

	winrt::Microsoft::Graphics::Canvas::CanvasRenderTarget& GetImage();

	void OnResize(winrt::Microsoft::Graphics::Canvas::CanvasDevice& device, uint32_t width, uint32_t height, float dpi);
	void Render(Scene& scene);

	winrt::Microsoft::Graphics::Canvas::CanvasRenderTarget& GetFinalImage() { return m_FinalImage; }

	void ResetFrameIndex() { m_FrameIndex = 1; }
	Settings& GetSettings() { return m_Settings; }

private:
	struct HitPayload
	{
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;

		int ObjectIndex;
	};

	void DrawOffScreen();
	glm::vec4 PerPixel(uint32_t x, uint32_t y); // RayGen

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
	HitPayload Miss(const Ray& ray);

private:
	winrt::Microsoft::Graphics::Canvas::CanvasRenderTarget m_FinalImage{nullptr};
	std::vector<winrt::Windows::UI::Color> m_ImageData;
	std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;
	std::vector<glm::vec4> m_AccumulationData;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

	Settings m_Settings;
	uint32_t m_FrameIndex = 1;
};