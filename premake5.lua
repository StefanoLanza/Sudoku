-- Global settings
local workspacePath = path.join("build/", _ACTION)  -- e.g. build/vs2019

-- Filters
local vs = "action:vs*"
local xcode = "action:xcode*"
local x86 = "platforms:x86"
local x64 = "platforms:x86_64"
local debug =  "configurations:Debug*"
local release =  "configurations:Release*"

workspace ("Sudoku")
	configurations { "Debug", "Release" }
	platforms { "x86", "x86_64" }
	language "C++"
	location (workspacePath)
	characterset "MBCS"
	flags   { "MultiProcessorCompile", }
	startproject "UnitTest"
	exceptionhandling "Off"
	defines { "_HAS_EXCEPTIONS=0" }
	cppdialect "c++17"
	rtti "Off"

filter { vs }
	buildoptions
	{
		"/permissive-",
	}
	system "Windows"
	defines { "_ENABLE_EXTENDED_ALIGNED_STORAGE", }
	-- systemversion "10.0.17134.0"

filter { xcode }
	system "macosx"
	systemversion("10.12") -- MACOSX_DEPLOYMENT_TARGET

filter { x86 }
	architecture "x86"
	  
filter { x64 }
	architecture "x86_64"

filter { vs, x86, }
	defines { "WIN32", "_WIN32", }

filter { vs, x64, }
	defines { "WIN64", "_WIN64", }

filter { vs, debug, }
	defines { "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1",  }

filter { vs, release, }
	defines { "_ITERATOR_DEBUG_LEVEL=0", "_SECURE_SCL=0", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1",  }

filter { debug }
	defines { "_DEBUG", "DEBUG", }
	flags   { "NoManifest", }
	optimize("Off")
	inlining "Default"
	warnings "Extra"
	symbols "Full"
	runtime "Debug"

filter { release }
	defines { "NDEBUG", }
	flags   { "NoManifest", "LinkTimeOptimization", "NoBufferSecurityCheck", "NoRuntimeChecks", }
	optimize("Full")
	inlining "Auto"
	warnings "Extra"
	symbols "Off"
	runtime "Release"

configuration "Debug"
    debugdir "bin"

configuration "Release"
    debugdir "bin"

project("Sudoku")
	kind "StaticLib"
	files { "src/**.c", "src/**.h", "include/**.*" }
	includedirs { "src", "include", }

project("Inih")
	kind "StaticLib"
	files { "external/inih/ini.*", }

project("Utils")
	kind "StaticLib"
	files { "utils/**.c", "utils/**.h" }
	sysincludedirs { "./", "external", }
	links( { "Inih", } )

project("UnitTest")
	kind "ConsoleApp"
	links("Sudoku")
	files { "tests/**.cpp", "sudokus/*", }
	sysincludedirs { "./", "external", }

project("Example1")
	kind "ConsoleApp"
	files "examples/example1.cpp"
	files "examples/printSudoku.*"
	files "sudokus/*"
	sysincludedirs { "./", }
	links( { "Sudoku", "Utils" })

project("Solver")
	kind "ConsoleApp"
	files { "solver/**.*" }
	sysincludedirs { "./", "external", }
	links( { "Sudoku", "Utils" })
