#include "pch.h"

#include "Renderer.h"

//#include <winrt/Windows.Foundation.Numerics.h>
//#include <WindowsNumerics.h>
//#include <winrt/Microsoft.UI.h>
//#include "winrt/Windows.UI.h"
#include <winrt/Microsoft.Graphics.Canvas.h>
//#include <winrt/Microsoft.Graphics.Canvas.UI.Xaml.h>
//#include <winrt/Microsoft.Graphics.DirectX.h>
//#include <winrt/Windows.Storage.Streams.h>
#include <execution>
#include <vector>
//#include <numeric>

//#include<gsl/gsl>

// PERF TEST
//#include "Random.h"
//#include "RandomFloat.h"
#include "RandomVecWithinCone.h"
#include "Log.h"
#include <spdlog/stopwatch.h>
#include <chrono>
// #include <spdlog/fmt/chrono.h>
//..
namespace Utils {
	[[nodiscard]] static winrt::Windows::UI::Color ConvertToColor(const glm::vec4& color) noexcept
	{
		return winrt::Windows::UI::Color{ (uint8_t)(color.a * 255.0f), (uint8_t)(color.r * 255.0f), (uint8_t)(color.g * 255.0f), (uint8_t)(color.b * 255.0f) };
	}

	[[nodiscard]] static float ConvertPixelsToDips(size_t pixels, float dpi)
	{
		constexpr float  dipsPerInch = 96.0f;
		return pixels * dipsPerInch / dpi;
	}
}

winrt::Microsoft::Graphics::Canvas::CanvasRenderTarget& Renderer::GetImage() noexcept
{
	return m_FinalImage;
}

void Renderer::OnResize(const winrt::Microsoft::Graphics::Canvas::CanvasDevice& device, uint32_t width, uint32_t height, float dpi) noexcept
{
	ML_METHOD;
	if (width == _width && height == _height && m_FinalImage)
	{
		// nothing to do here
		return;
	}

	_width = width;
	_height = height;

	if (m_FinalImage)
	{
		if (m_FinalImage.SizeInPixels().Width == _width && m_FinalImage.SizeInPixels().Height == _height)
		{
			// No resize necessary
			return;
		}
	}

	// HACK TODO
	//m_FinalImage = winrt::Microsoft::Graphics::Canvas::CanvasRenderTarget(device, 10, 10, dpi); 
	const float w = Utils::ConvertPixelsToDips(_width, dpi);
	const float h = Utils::ConvertPixelsToDips(_height, dpi);

	m_FinalImage = winrt::Microsoft::Graphics::Canvas::CanvasRenderTarget(device, w, h, dpi);

	m_ImageData.resize(_width * _height);
	m_AccumulationData.resize(_width * _height);

	m_ImageHorizontalIter.resize(_width);
	std::iota(m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(), 0);

	m_ImageVerticalIter.resize(_height);
	std::iota(m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(), 0);

	m_FrameIndex = 1;
}

void Renderer::UpdateThread(uint32_t start, uint32_t end)
{
	// Note this does NOT look as good and some of the accumulated pixels appear to be in the wrong position
	for (uint32_t y = start; y < end; y++)
	{
		for (uint32_t x = 0; x < _width; x++)
		{
			glm::vec4 color(PerPixel(x, y));
			m_AccumulationData[x + y * _width] += color;

			color = m_AccumulationData[x + y * _width];
			color /= (float)m_FrameIndex;

			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * _width] = Utils::ConvertToColor(color);
		}
	}
}

void Renderer::Render(Scene& scene)
{
	ML_METHOD;

	spdlog::stopwatch sw;

	m_ActiveScene = &scene;
	m_ActiveCamera = &(scene.GetCamera());
	const glm::vec4 skyColor = glm::vec4(1.0f,0.6f, 0.7f, 0.9f);

	if (m_FrameIndex == 1)
	{
		m_AccumulationData.assign(m_AccumulationData.size(), glm::vec4(0.0f));
		ML_TRACE("Resetting FrameIndex\n");
	}

	// super slow code ahead
	// Matt's "I like control of how many threads get created" version
	// Note this does NOT look as good and some of the accumulated pixels appear to be in the wrong position
	//int _threadcount = 8000;// gsl::narrow_cast<int>(std::thread::hardware_concurrency() - 1);
	//uint32_t rowStart = 0;
	//const uint32_t rowsPerThread = gsl::narrow_cast<int>(_height / _threadcount);
	//const uint32_t remainingRows = gsl::narrow_cast<int>(_height % _threadcount);

	//std::vector<std::jthread> threads;
	//for (int t = 0; t < _threadcount - 1; t++)
	//{
	//	threads.emplace_back(std::jthread{ &Renderer::UpdateThread, this, rowStart, rowStart + rowsPerThread});
	//	rowStart += rowsPerThread;
	//}
	//threads.emplace_back(std::jthread{ &Renderer::UpdateThread, this, rowStart, rowStart + rowsPerThread + remainingRows});
	// end super slow code
	
	//Original Cherno Code with some modifications
	std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
		[this](uint32_t y)
		{
			std::for_each(std::execution::seq, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
				[this, y](uint32_t x)
				{
					glm::vec4 color(PerPixel(x, y));
					m_AccumulationData[x + y * _width] += color;

					color = m_AccumulationData[x + y * _width];
					color /= (float)m_FrameIndex;

					color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
					m_ImageData[x + y * _width] = Utils::ConvertToColor(color);
				});
		});

	//winrt::array_view<winrt::Windows::UI::Color> view{ m_ImageData };
	m_FinalImage.SetPixelColors(m_ImageData);
	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;

	ML_TRACE("FrameIndex {}", m_FrameIndex);
	//const std::chrono::duration elapsed = sw.elapsed();
	ML_TRACE("Render StopWatch {:3}", std::chrono::duration_cast<std::chrono::milliseconds>(sw.elapsed()));
}
/*
	spdlog::stopwatch sw;
	const std::chrono::duration elapsed = sw.elapsed();
	ML_TRACE("Render StopWatch {:3}", std::chrono::duration_cast<std::chrono::milliseconds>(elapsed));
*/

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	//Ray ray;
	//ray.Origin = m_ActiveCamera->GetPosition();
	//ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * _width];
	
	Ray ray(m_ActiveCamera->GetPosition(), m_ActiveCamera->GetRayDirections()[x + y * _width]);

	glm::vec3 color(0.0f);
	const glm::vec3 skyColor(0.6f, 0.7f, 0.9f);
	float multiplier = 1.0f;
	const glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, 1));

	constexpr int bounces = 6;
	for (int i = 0; i < bounces; i++)
	{
		const Renderer::HitPayload payload(TraceRay(ray));
		// TODO this hits perf testing because the CPU can't predict the branch
		if (payload.HitDistance < 0.0f)
		{
			color += skyColor * multiplier;
			break;
		}

		const float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f); // == cos(angle)
		const Sphere& sphere(m_ActiveScene->Spheres[payload.ObjectIndex]);
		const Material& material(m_ActiveScene->Materials[sphere.MaterialIndex]);

		glm::vec3 sphereColor(material.Albedo);
		sphereColor *= lightIntensity;
		color += sphereColor * multiplier;

		multiplier *= 0.5f;

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;

		// PERF TEST
		//ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal + material.Roughness * winrt::Butternut::implementation::Random::Vec3(-0.5f, 0.5f));
		//ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal + material.Roughness * ::Util::RandomFloat::Vec3WithinCone());
		ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal + material.Roughness * ::Util::RandomVecWithinCone::Vec3WithinCone());
	}
	return glm::vec4(color, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
	// where
	// a = ray origin
	// b = ray direction
	// r = radius
	// t = hit distance

	int closestSphere = -1;
	float hitDistance = std::numeric_limits<float>::max();
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
	{
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		const glm::vec3 origin(ray.Origin - sphere.Position);

		const float a = glm::dot(ray.Direction, ray.Direction);
		const float b = 2.0f * glm::dot(origin, ray.Direction);
		const float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

		// Quadratic forumula discriminant:
		// b^2 - 4ac

		const float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0f)
			continue;

		// Quadratic formula:
		// (-b +- sqrt(discriminant)) / 2a

		// float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a); // Second hit distance (currently unused)
		const float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
		if (closestT > 0.0f && closestT < hitDistance)
		{
			hitDistance = closestT;
			closestSphere = (int)i;
		}
	}

	if (closestSphere < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance, closestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	Renderer::HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Sphere& closestSphere(m_ActiveScene->Spheres[objectIndex]);

	const glm::vec3 origin(ray.Origin - closestSphere.Position);
	payload.WorldPosition = origin + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);

	payload.WorldPosition += closestSphere.Position;

	return payload;
}

Renderer::HitPayload Renderer::Miss([[maybe_unused]] const Ray& ray) noexcept
{
	Renderer::HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
}
