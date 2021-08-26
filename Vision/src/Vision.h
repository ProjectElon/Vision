#pragma once

/*----------    Core    ----------*/
#include "Vision/Core/Defines.h"
#include "Vision/Core/Logger.h"
#include "Vision/Core/Application.h"
#include "Vision/Core/Layer.h"
#include "Vision/Core/LayerStack.h"
#include "Vision/Core/Vars.h"
#include "Vision/Core/String.h"
/*--------------------------------*/

/*----------   Events   ----------*/
#include "Vision/Events/Event.h"
#include "Vision/Events/ApplicationEvent.h"
#include "Vision/Events/KeyEvent.h"
#include "Vision/Events/MouseEvent.h"
/*--------------------------------*/

/*----------  Platform  ----------*/
#include "Vision/Platform/EntryPoint.h"
#include "Vision/Platform/Window.h"
#include "Vision/Platform/Input.h"
#include "Vision/Platform/KeyCodes.h"
#include "Vision/Platform/MouseCodes.h"
#include "Vision/Platform/Timer.h"
#include "Vision/Platform/PlatformUtils.h"
#include "Vision/Platform/Memory.h"
#include "Vision/Platform/File.h"
#include "Vision/Platform/FileSystem.h"
#include "Vision/Platform/DirectoryWatcher.h"
/*--------------------------------*/


/*---------- Assets ----------*/
#include "Vision/Assets/Assets.h"
/*--------------------------------*/

/*----------  Renderer  ----------*/
#include "Vision/Renderer/Renderer.h"
#include "Vision/Renderer/Renderer2D.h"
#include "Vision/Renderer/TextureAtlas.h"
#include "Vision/Renderer/OrthographicCamera.h"
#include "Vision/Renderer/PerspectiveCamera.h"
#include "Vision/Renderer/Font.h"
/*--------------------------------*/

/*----------  UI  ----------*/
#include "Vision/ImGui/ImGuiWidgets.h"
/*--------------------------------*/

/*----------  ECS  ----------*/
#include "Vision/Entity/Entity.h"
#include "Vision/Entity/Components.h"
#include "Vision/Entity/Scene.h"
/*--------------------------------*/