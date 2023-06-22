#include "pch.h"

#include "Scene.h"

Scene::Scene() noexcept
    : _camera(45.0f, 0.1f, 100.0f)
{
    Material& pinkSphere = Materials.emplace_back();
    pinkSphere.Albedo = { 0.0f, 0.6f, 0.6f };
    pinkSphere.Roughness = 0.0f;

    Material& blueSphere = Materials.emplace_back();
    blueSphere.Albedo = { 0.2f, 0.3f, 1.0f };
    blueSphere.Roughness = 0.1f;

    {
        Sphere sphere;
        sphere.Position = { 0.0f, 0.0f, 0.0f };
        sphere.Radius = 1.0f;
        sphere.MaterialIndex = 0;
        Spheres.push_back(sphere);
    }

    {
        Sphere sphere;
        sphere.Position = { 0.0f, -101.0f, 0.0f };
        sphere.Radius = 100.0f;
        sphere.MaterialIndex = 1;
        Spheres.push_back(sphere);
    }
}