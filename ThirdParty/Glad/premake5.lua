project ("Glad")

    kind("StaticLib")
    language("C")
    
    systemversion ("latest")
    staticruntime ("On")
    pic ("On")
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/glad.c",
        "include/glad/glad.h",
        "KHR/khrplatform.h"
    }

    includedirs
    {
        "include"
    }
    
    
    filter ("configurations:Debug")
		runtime ("Debug")
		symbols ("on")

	filter ("configurations:Release")
		runtime ("Release")
        optimize ("on")
    
    filter ("configurations:Dist")
		runtime ("Release")
		optimize ("on")