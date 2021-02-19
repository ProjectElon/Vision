-- build script

workspace ("Vision")

	architecture ("x64")
	systemversion ("latest")
	startproject ("EagleEye")

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

	filter ("system:windows")

		defines
		{
			"VN_PLATFORM_DESKTOP"
		}

	filter ("system:linux")

		defines
		{
			"VN_PLATFORM_DESKTOP"
		}

	filter ("system:macos")

		defines
		{
			"VN_PLATFORM_DESKTOP"
		}

	filter ("configurations:Debug")

		defines ("VN_DEBUG")
		runtime ("Debug")
		symbols ("on")

		defines
		{
			"VN_EDITOR"
		}

	filter ("configurations:Release")

		defines ("VN_RELEASE")
		runtime ("Release")
		optimize ("on")

		defines
		{
			"VN_EDITOR"
		}

	filter ("configurations:Dist")

		defines ("VN_DIST")
		runtime ("Release")
		optimize ("on")

outputdir = "%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}"

includedir = {}

includedir["spdlog"]   = "ThirdParty/spdlog/include"
includedir["GLFW"]     = "ThirdParty/GLFW/include"
includedir["Glad"]     = "ThirdParty/Glad/include"
includedir["ImGui"]    = "ThirdParty/imgui"
includedir["glm"]      = "ThirdParty/glm"
includedir["stb"]	   = "ThirdParty/stb"
includedir["ImGuizmo"] = "ThirdParty/ImGuizmo"

group ("Dependencies")

	include ("ThirdParty/GLFW")
	include ("ThirdParty/Glad")
	include ("ThirdParty/imgui")

group ("")

project ("Vision")

	location ("Vision")
	kind ("StaticLib")
	language ("C++")
	cppdialect ("C++17")
	staticruntime ("on")

	pchheader ("pch.h")
	pchsource ("Vision/src/pch.cpp")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{includedir.ImGuizmo}/ImGuizmo.h",
		"%{includedir.ImGuizmo}/ImGuizmo.cpp",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{includedir.spdlog}",
		"%{includedir.GLFW}",
		"%{includedir.Glad}",
		"%{includedir.ImGui}",
		"%{includedir.glm}",
		"%{includedir.stb}",
		"%{includedir.ImGuizmo}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	filter "files:ThirdParty/ImGuizmo/**.cpp"
	flags { "NoPch" }

project ("PreBuild")

	location ("PreBuild")
	kind ("ConsoleApp")
	language ("C++")
	cppdialect ("C++17")
	staticruntime ("on")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"Vision/src",
		"%{includedir.spdlog}",
		"%{includedir.ImGui}",
		"%{includedir.glm}",
		"%{includedir.stb}",
	}

	links
	{
		"Vision"
	}

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

project ("EagleEye")

	location ("EagleEye")
	kind ("ConsoleApp")
	language ("C++")
	cppdialect ("C++17")
	staticruntime ("on")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"Vision/src",
		"%{includedir.spdlog}",
		"%{includedir.ImGui}",
		"%{includedir.glm}",
		"%{includedir.stb}",
		"%{includedir.ImGuizmo}"
	}

	links
	{
		"Vision"
	}

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")