#pragma once
#include "AppWindow.h"

namespace  ST
{
    class DirLight;
    class PointLight;

    class ImguiPanel
    {
    public:
        static void Init(GLFWwindow* window);
        static void Close();
        static void Render();
        static void NewFrame();
        static void CreatePointLightPanel(const char* name,ST_REF<PointLight> light);
        static void CreateDirLightPanel(const char* name,ST_REF<DirLight> light);
        static void ShowDemoPanel();
    };
}

