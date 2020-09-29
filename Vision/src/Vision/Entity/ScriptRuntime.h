#pragma once

#include "Vision/Core/Base.h"

#include <functional>

using OnCreateFn  = std::function<void()>;
using OnDestroyFn = std::function<void()>;
using OnUpdateFn  = std::function<void(float)>;

struct ScriptRuntime
{
	OnCreateFn  OnCreate;
	OnDestroyFn OnDestroy;
	OnUpdateFn  OnUpdate;
};

using GetScriptRuntimeFn   = std::function<ScriptRuntime(const void*)>;
using ScriptRuntimeStorage = std::vector<std::pair<uint32, GetScriptRuntimeFn>>;

template<typename Script>
ScriptRuntime GetScriptRuntime(const void* scriptMemory)
{
	Script* script = (Script*)(scriptMemory);

	ScriptRuntime result;

	result.OnCreate  = std::bind(&Script::OnCreate,  script);
	result.OnDestroy = std::bind(&Script::OnDestroy, script);
	result.OnUpdate  = std::bind(&Script::OnUpdate,  script, std::placeholders::_1);

	return result;
}