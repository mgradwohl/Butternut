#pragma once

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

	winrt::Microsoft::Graphics::Canvas::CanvasRenderTarget& GetImage() noexcept;

	//void Renderer::OnResize(const winrt::Microsoft::Graphics::Canvas::CanvasDevice& device, uint32_t width, uint32_t height, float dpi)

	void OnResize(const winrt::Microsoft::Graphics::Canvas::CanvasDevice& device, uint32_t width, uint32_t height, float dpi) noexcept;
	void Render(Scene& scene);

	winrt::Microsoft::Graphics::Canvas::CanvasRenderTarget& GetFinalImage() noexcept { return m_FinalImage; }

	void ResetFrameIndex() noexcept { m_FrameIndex = 1; }
	Settings& GetSettings() noexcept { return m_Settings; }

private:
	struct HitPayload
	{
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;

		int ObjectIndex;
	};

	void UpdateThread(uint32_t start, uint32_t end);
	glm::vec4 PerPixel(uint32_t x, uint32_t y); // RayGen

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
	HitPayload Miss(const Ray& ray) noexcept;
	[[nodiscard]] size_t Width() const noexcept { return _width; };
	[[nodiscard]] size_t Height() const noexcept { return _height; };

private:
	winrt::Microsoft::Graphics::Canvas::CanvasRenderTarget m_FinalImage{nullptr};
	std::vector<winrt::Windows::UI::Color> m_ImageData;
	std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;
	std::vector<glm::vec4> m_AccumulationData;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

	// TODO cache these so that methods don't need to call into m_FinalImage.SizeInPixels().Width/Height
	// make sure they are updated when the OnResize is called
	size_t _width = 0;
	size_t _height = 0;

	Settings m_Settings;
	uint32_t m_FrameIndex = 1;
};