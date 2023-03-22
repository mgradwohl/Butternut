#include "pch.h"

#include "Renderer.h"

#include <winrt/Windows.Foundation.Numerics.h>
#include <WindowsNumerics.h>
#include <winrt/Microsoft.UI.h>
#include "winrt/Windows.UI.h"
#include <winrt/Microsoft.Graphics.Canvas.h>
#include <winrt/Microsoft.Graphics.Canvas.UI.Xaml.h>
#include <winrt/Microsoft.Graphics.DirectX.h>
#include <winrt/Windows.Storage.Streams.h>
#include <execution>
#include <vector>

#include<gsl/gsl>

#include "Random.h"
#include "Log.h"

namespace Utils {
	static winrt::Windows::UI::Color ConvertToColor(const glm::vec4& color)
	{
		//uint8_t r = (uint8_t)(color.r * 255.0f);
		//uint8_t g = (uint8_t)(color.g * 255.0f);
		//uint8_t b = (uint8_t)(color.b * 255.0f);
		//uint8_t a = (uint8_t)(color.a * 255.0f);

		return winrt::Microsoft::UI::ColorHelper::FromArgb((uint8_t)(color.a * 255.0f), (uint8_t)(color.r * 255.0f), (uint8_t)(color.g * 255.0f), (uint8_t)(color.b * 255.0f));
	}

	//static uint32_t ConvertToRGBA(const glm::vec4& color)
	//{
	//	uint8_t r = (uint8_t)(color.r * 255.0f);
	//	uint8_t g = (uint8_t)(color.g * 255.0f);
	//	uint8_t b = (uint8_t)(color.b * 255.0f);
	//	uint8_t a = (uint8_t)(color.a * 255.0f);

	//	uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
	//	return result;
	//}


	//static uint32_t ConvertToBGRA(const glm::vec4& color)
	//{
	//	uint8_t r = (uint8_t)(color.r * 255.0f);
	//	uint8_t g = (uint8_t)(color.g * 255.0f);
	//	uint8_t b = (uint8_t)(color.b * 255.0f);
	//	uint8_t a = (uint8_t)(color.a * 255.0f);

	//	uint32_t result = (b << 24) | (g << 16) | (r << 8) | a;
	//	return result;
	//}
}

winrt::Microsoft::Graphics::Canvas::CanvasRenderTarget& Renderer::GetImage()
{
	return m_FinalImage;
}

void Renderer::OnResize(winrt::Microsoft::Graphics::Canvas::CanvasDevice& device, uint32_t width, uint32_t height, float dpi)
{
	ML_METHOD;
	if (m_FinalImage)
	{
		// No resize necessary
		if (m_FinalImage.SizeInPixels().Width == width && m_FinalImage.SizeInPixels().Height == height)
			return;
	}
	else
	{
		float w = width;
		float h = height;

		m_FinalImage = winrt::Microsoft::Graphics::Canvas::CanvasRenderTarget(device, w, h, dpi);// , winrt::Microsoft::Graphics::DirectX::DirectXPixelFormat::R8G8B8A8UInt, winrt::Microsoft::Graphics::Canvas::CanvasAlphaMode::Straight );
		// TODO this does not make the image right side up
		// best to just draw it right side up
		//m_FinalImage.CreateDrawingSession().Transform(winrt::Windows::Foundation::Numerics::make_float3x2_scale(1.0f, -1.0f));
	}

	uint32_t w = m_FinalImage.SizeInPixels().Width;
	uint32_t h = m_FinalImage.SizeInPixels().Height;
	m_ImageData.resize(w * h);
	m_AccumulationData.resize(w* h);

	m_ImageHorizontalIter.resize(w);
	for (uint32_t i = 0; i < w; i++)
		m_ImageHorizontalIter[i] = i;

	m_ImageVerticalIter.resize(h);
	for (uint32_t i = 0; i < h; i++)
		m_ImageVerticalIter[i] = i;
}

void Renderer::UpdateThread(uint32_t start, uint32_t end)
{
	// Note this does NOT look as good and some of the accumulated pixels appear to be in the wrong position
	const int w = m_FinalImage.SizeInPixels().Width;
	for (uint32_t y = start; y < end; y++)
	{
		for (uint32_t x = 0; x < w; x++)
		{
			glm::vec4 color = PerPixel(x, y);
			m_AccumulationData[x + y * w] += color;

			color = m_AccumulationData[x + y * w];
			color /= (float)m_FrameIndex;

			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * w] = Utils::ConvertToColor(color);
		}
	}
}

void Renderer::Render(Scene& scene)
{
	ML_METHOD;
	m_ActiveScene = &scene;
	m_ActiveCamera = &(scene.GetCamera());
	const glm::vec4 skyColor = glm::vec4(1.0f,0.6f, 0.7f, 0.9f);

	if (m_FrameIndex == 1)
	{
		memset(m_AccumulationData.data(), 0, m_FinalImage.SizeInPixels().Width * m_FinalImage.SizeInPixels().Height * sizeof(glm::vec4));
		ML_TRACE("Resetting FrameIndex\n");
	}

	// super slow code ahead
	// Matt's "I like control of how many threads get created" version
	// Note this does NOT look as good and some of the accumulated pixels appear to be in the wrong position
	//int _threadcount = 200;// gsl::narrow_cast<int>(std::thread::hardware_concurrency() - 1);
	//int rowStart = 0;
	//const auto rowsPerThread = gsl::narrow_cast<uint16_t>(m_FinalImage.SizeInPixels().Height / _threadcount);
	//const auto remainingRows = gsl::narrow_cast<uint16_t>(m_FinalImage.SizeInPixels().Height % _threadcount);

	//std::vector<std::jthread> threads;
	//for (int t = 0; t < _threadcount - 1; t++)
	//{
	//	threads.emplace_back(std::jthread{ &Renderer::UpdateThread, this, rowStart, gsl::narrow_cast<uint16_t>(rowStart + rowsPerThread)});
	//	rowStart += rowsPerThread;
	//}
	//threads.emplace_back(std::jthread{ &Renderer::UpdateThread, this, rowStart, gsl::narrow_cast<uint16_t>(rowStart + rowsPerThread + remainingRows)});

	//Original Cherno Code with some modifications
	const int w = m_FinalImage.SizeInPixels().Width;
	std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
		[this,w](uint32_t y)
		{
			std::for_each(std::execution::seq, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
				[this, y,w](uint32_t x)
				{
					glm::vec4 color = PerPixel(x, y);
					m_AccumulationData[x + y * w] += color;

					color = m_AccumulationData[x + y * w];
					color /= (float)m_FrameIndex;

					color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
					m_ImageData[x + y * w] = Utils::ConvertToColor(color);
				});
		});

	winrt::array_view<winrt::Windows::UI::Color> view{ m_ImageData };
	m_FinalImage.SetPixelColors(view);
	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
	ML_TRACE("FrameIndex {}\n", m_FrameIndex);

}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage.SizeInPixels().Width];
	
	glm::vec3 color(0.0f);
	const glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
	float multiplier = 1.0f;
	const glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, 1));

	int bounces = 5;
	Renderer::HitPayload payload;// = TraceRay(ray);
	for (int i = 0; i < bounces; i++)
	{
		payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f)
		{
			color += skyColor * multiplier;
			break;
		}

		const float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f); // == cos(angle)
		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];

		glm::vec3 sphereColor = material.Albedo;
		sphereColor *= lightIntensity;
		color += sphereColor * multiplier;

		multiplier *= 0.5f;

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
		ray.Direction = glm::reflect(ray.Direction,
			payload.WorldNormal + material.Roughness * winrt::Butternut::implementation::Random::Vec3(-0.5f, 0.5f));
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
		glm::vec3 origin = ray.Origin - sphere.Position;

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

	const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];

	glm::vec3 origin = ray.Origin - closestSphere.Position;
	payload.WorldPosition = origin + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);

	payload.WorldPosition += closestSphere.Position;

	return payload;
}

Renderer::HitPayload Renderer::Miss([[maybe_unused]] const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
}
