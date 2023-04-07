# Butternut
 
 Butternut is based off The Cherno's raytracing series. https://www.youtube.com/@TheCherno.
 
 It's a simple CPU raytracer that draws to a Win2D canvas, written in C++/WinRT on the WinAppSDK/WinUI.

 To clone, use git clone --recursive https://github.com/mgradwohl/Butternut.git and ensure you have the Vulkan SDK (or ast least GLM) installed, and make sure you check that the include and lib paths are correct.

 GLM is used for Maths. I'd like to try DirectXMath and check if it is faster https://learn.microsoft.com/en-us/windows/win32/dxmath/directxmath-portal.
 Xoshiro is used for Random https://github.com/Reputeless/Xoshiro-cpp.
 The code is structured to eventually move key functions to the GPU.
 
 On my test machines, the rendering is no longer CPU bound.
 Check the preprocessor defines.

 Configurations
	Language: C++ 20
	AMD: Optimized for AVX2
	Debug: MSVC default Debug with console logging (slow)
	Tidy: Debug with Clang Tidy code analysis
	Release: MSVC default Release config, with optimizations
	Release Test: Release with console logging 
	Distro: No debug info, no console logging
