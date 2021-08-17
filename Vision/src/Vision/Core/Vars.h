#pragma once

#include "Vision/Core/Defines.h"
#include "Vision/Core/String.h"
#include "Vision/Platform/Window.h"

namespace Vision
{
    introspect_struct("vars", true)
    struct Settings
    {
        int32 WindowX = 0;
        int32 WindowY = 0;

        uint32 WindowWidth  = 1280;
        uint32 WindowHeight = 720;

        uint32 ViewportWidth  = 1280;
        uint32 ViewportHeight = 720;

        //@Robustness: Have a default api for each platform right now we are dealing with opengl
        String RendererAPI = MakeString("OpenGL");
        String WindowTitle = MakeString("untitled");
        String ScenePath   = MakeString("none");

        uint32 FontSize = 72;
        uint32 DebugFontSize    = 20;
        String SpriteShaderPath = MakeString("Assets/Shaders/Sprite.glsl");
        String MainFontPath     = MakeString("Assets/Fonts/FiraCode-Regular.ttf");
        String DebugFontPath    = MakeString("Assets/Fonts/TitilliumWeb-Regular.ttf");
    };

    introspect_struct("vars", true)
    struct Vars
    {
        Settings Settings;

        ignore_methods()

        void Load();
        void Save();
    };
}
