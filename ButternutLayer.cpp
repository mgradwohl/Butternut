#include "pch.h"

#include "Timer.h"
#include "Renderer.h"
#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>

namespace winrt::Butternut::implementation
{
    class Layer
    {
    public:
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}

        virtual void OnUpdate(float ts) {}
        virtual void OnUIRender() {}
    };

}

class ExampleLayer : public winrt::Butternut::implementation::Layer
{
public:
    ExampleLayer()
        : m_Camera(45.0f, 0.1f, 100.0f)
    {
        Material& pinkSphere = m_Scene.Materials.emplace_back();
        pinkSphere.Albedo = { 1.0f, 0.0f, 1.0f };
        pinkSphere.Roughness = 0.0f;

        Material& blueSphere = m_Scene.Materials.emplace_back();
        blueSphere.Albedo = { 0.2f, 0.3f, 1.0f };
        blueSphere.Roughness = 0.1f;

        {
            Sphere sphere;
            sphere.Position = { 0.0f, 0.0f, 0.0f };
            sphere.Radius = 1.0f;
            sphere.MaterialIndex = 0;
            m_Scene.Spheres.push_back(sphere);
        }

        {
            Sphere sphere;
            sphere.Position = { 0.0f, -101.0f, 0.0f };
            sphere.Radius = 100.0f;
            sphere.MaterialIndex = 1;
            m_Scene.Spheres.push_back(sphere);
        }
    }

    virtual void OnUpdate(float ts) override
    {
        if (m_Camera.OnUpdate(ts))
            m_Renderer.ResetFrameIndex();
    }

    virtual void OnUIRender() override
    {
        //if (ImGui::Button("Render"))
        //{
        //    Render();
        //}

        //ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);

        //if (ImGui::Button("Reset"))
        //    m_Renderer.ResetFrameIndex();

        // editing the spheres
        //for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
        //    Sphere& sphere = m_Scene.Spheres[i];
        //    ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
        //    ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
        //    ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size() - 1);

        // editing materials
        //    Material& material = m_Scene.Materials[i];
        //    ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
        //    ImGui::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f);
        //    ImGui::DragFloat("Metallic", &material.Metallic, 0.05f, 0.0f, 1.0f);

        // size of the canvas
        //m_ViewportWidth = ImGui::GetContentRegionAvail().x;
        //m_ViewportHeight = ImGui::GetContentRegionAvail().y;

        // render the image in the viewport (on the canvas)
        //auto image = m_Renderer.GetFinalImage();
        //if (image)
        //    ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
        //        ImVec2(0, 1), ImVec2(1, 0));

        Render();
    }

    void Render()
    {
        Timer timer;

        m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
        m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
        m_Renderer.Render(m_Scene, m_Camera);

        m_LastRenderTime = timer.ElapsedMillis();
    }
private:
    Renderer m_Renderer;
    Camera m_Camera;
    Scene m_Scene;
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

    float m_LastRenderTime = 0.0f;
};
