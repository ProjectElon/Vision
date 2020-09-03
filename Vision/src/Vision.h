#pragma once

/*----------    Core    ----------*/
#include "Vision/Core/Base.h"
#include "Vision/Core/Log.h"
#include "Vision/Core/Application.h"
#include "Vision/Core/Layer.h"
#include "Vision/Core/LayerStack.h"
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
/*--------------------------------*/

/*----------  Renderer  ----------*/
#include "Vision/Renderer/GraphicsContext.h"
#include "Vision/Renderer/RenderCommand.h"
#include "Vision/Renderer/Renderer.h"
#include "Vision/Renderer/Renderer2D.h"
#include "Vision/Renderer/VertexBuffer.h"
#include "Vision/Renderer/IndexBuffer.h"
#include "Vision/Renderer/VertexLayout.h"
#include "Vision/Renderer/Texture2D.h"
#include "Vision/Renderer/Shader.h"
#include "Vision/Renderer/RendererAPI.h"
#include "Vision/Renderer/OrthographicCamera.h"
#include "Vision/Renderer/OrthographicCameraController.h"
#include "Vision/Renderer/Sprite.h"
#include "Vision/Renderer/SpriteAtlas.h"
#include "Vision/Renderer/FrameBuffer.h"
/*--------------------------------*/

/*----------  ECS  ----------*/
#include "Vision/Entity/Entity.h"
#include "Vision/Entity/Component.h"
#include "Vision/Entity/Scene.h"
/*--------------------------------*/