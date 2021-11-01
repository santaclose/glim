workspace "glim"
	architecture "x64"
	startproject "glim"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Projects
group "Dependencies"
	include "vendor/GLFW"
	include "vendor/Glad"
group ""

project "glim"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"demo.cpp",
		"vendor/msdfgl/src/**.c",
		"vendor/msdfgl/src/**.h"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"src",
		"vendor/GLFW/include",
		"vendor/Glad/include",
		"vendor/stb_truetype/",
		"vendor/freetype/include",
		"vendor/glm",
		"vendor/msdfgl/include"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"opengl32.lib",
		"vendor/freetype/win64/freetype.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLIM_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "GLIM_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "GLIM_RELEASE"
		runtime "Release"
		optimize "on"
