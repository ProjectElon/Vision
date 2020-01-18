-- build script

workspace ("Vision")
	
	architecture ("x64")
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

		systemversion "latest"

	filter ("configurations:Debug")

		defines "VN_DEBUG"
		runtime "Debug"
		symbols "on"

	filter ("configurations:Release")

		defines "VN_RELEASE"
		runtime "Release"
		optimize "on"

	filter ("configurations:Dist")

		defines "VN_DIST"
		runtime "Release"
		optimize "on"

outputdir = "%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}"

includedir = {}
includedir["spdlog"] = "ThirdParty/spdlog/include"

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
		"%{includedir.spdlog}"
	}

	links
	{
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
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
		"%{includedir.spdlog}"
	}

	links
	{
		"Vision"
	}

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")