-- build

workspace ("Vision")
	
	architecture ("x86_64")
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

	filter "system:windows"

		systemversion "latest"

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"VN_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "VN_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "VN_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "VN_DIST"
		runtime "Release"
		optimize "on"

outputdir = "%{cfg.system}-%{cfg.architecture}-%{cfg.bulidcfg}"

project ("Vision")
	
	location ("Vision")
	kind ("StaticLib")
	language ("C++")
	cppdialect ("C++17")
	staticruntime ("on")

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/src",
	}

	links
	{
	}

project ("Sandbox")

	location ("Sandbox")
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"Vision/src"
	}

	links
	{
		"Vision"
	}