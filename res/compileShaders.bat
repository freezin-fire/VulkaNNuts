@echo off

C:\VulkanSDK\Bin\glslc.exe Shaders\BasicShader.vert -o Shaders\BasicShader.vert.spv
C:\VulkanSDK\Bin\glslc.exe Shaders\BasicShader.frag -o Shaders\BasicShader.frag.spv
pause
