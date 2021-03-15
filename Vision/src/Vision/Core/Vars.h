#pragma once

#include "Vision/Core/Common.h"
#include "Vision/Core/String.h"
#include "Vision/Platform/Window.h"

namespace Vision
{
    Inspect(category : "vars")
    struct Settings
    {
        int32 WindowX = 0;
        int32 WindowY = 0;

        uint32 WindowWidth = 1280;
        uint32 WindowHeight = 720;

        uint32 ViewportWidth = 1280;
        uint32 ViewportHeight = 720;

        uint32 FontSize = 72;
        uint32 DebugFontSize = 20;

        bool IsWindowMaximized = false;
        bool FullScreen = false;

        String WindowTitle = "untitled";
        String ScenePath = "none";
    };

    Inspect(category : "vars")
    struct Vars
    {
        Settings Settings;

        Ignore()

        void Load();
        void Save();
    };
}
