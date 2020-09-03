-- build script

workspace ("Vision")
	
	architecture ("x64")
	systemversion ("latest")
	startproject ("Editor")
	
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

includedir["spdlog"]    = "ThirdParty/spdlog/include"
includedir["GLFW"]      = "ThirdParty/GLFW/include"
includedir["Glad"]      = "ThirdParty/Glad/include"
includedir["ImGui"]     = "ThirdParty/imgui"
includedir["glm"]       = "ThirdParty/glm"
includedir["rapidjson"] = "ThirdParty/rapidjson/include"
includedir["stb_image"] = "ThirdParty/stb"

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
		"%{includedir.glm}",
		"%{includedir.rapidjson}",
		"%{includedir.stb_image}"
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

project ("Editor")

	location ("Editor")
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
		"%{includedir.ImGui}",
		"%{includedir.spdlog}",
		"%{includedir.glm}",
		"%{includedir.rapidjson}"
	}

	links
	{
		"Vision"
	}

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")