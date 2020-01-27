-- build script

workspace ("Vision")
	
	architecture ("x64")
	systemversion ("latest")
	startproject ("Sandbox")
	
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

	filter ("configurations:Release")

		defines ("VN_RELEASE")
		runtime ("Release")
		optimize ("on")

	filter ("configurations:Dist")

		defines ("VN_DIST")
		runtime ("Release")
		optimize ("on")

outputdir = "%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}"

includedir = {}

includedir["spdlog"] = "ThirdParty/spdlog/include"
includedir["GLFW"]   = "ThirdParty/GLFW/include"
includedir["Glad"]   = "ThirdParty/Glad/include"
includedir["ImGui"]  = "ThirdParty/imgui"
includedir["glm"]    = "ThirdParty/glm"

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
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{includedir.spdlog}",
		"%{includedir.GLFW}",
		"%{includedir.Glad}",
		"%{includedir.ImGui}",
		"%{includedir.glm}"
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

project ("Sandbox")

	location ("Sandbox")
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
		"ThirdParty",
		"%{includedir.spdlog}",
		"%{includedir.glm}"
	}

	links
	{
		"Vision"
	}

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")