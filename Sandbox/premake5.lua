project "Sandbox"
    kind "consoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir "bin/%{cfg.buildcfg}"
    objdir    "bin/obj/%{cfg.buildcfg}"

    pchheader "stdafx.h"
    pchsource "pch/stdafx.cpp"

    files {
        "**.h", 
        "**.hpp", 
        "**.cpp",
        "*.lua",
        "*.dll"
    }
 
    includedirs {
        "include",
        "../include",
        "pch",
    }
	
	libdirs {
        "../libs/sfml/extlibs"
    }
	
	links {
	    "opengl32",
        "Winmm",
        "openal32",
        "winmm",
	}
    
    defines { 
        "GLEW_STATIC",
        "SFML_STATIC",
    }

    filter "configurations:debug"
        libdirs {
            "../libs/sfml/debug",
        }
		
		links {
            "sfml-graphics-s-d",
            "sfml-window-s-d",
            "sfml-system-s-d",
            "sfml-audio-s-d",
            "sfml-network-s-d",
        }
		
        symbols "On"

    filter "configurations:release"
	    libdirs {
            "../libs/sfml/release",
        }
		
		links {
            "sfml-graphics-s",
            "sfml-window-s",
            "sfml-system-s",
            "sfml-audio-s",
            "sfml-network-s",
        }

        optimize "On"
