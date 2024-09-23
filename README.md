# VulkanNuts

VulkanNuts is an open-source, multi-platform 3D game engine software written in C++. It leverages the Vulkan API for high-performance graphics rendering and is built upon the architecture of [littleVulkanEngine](https://github.com/blurrypiano/littleVulkanEngine) by [Brendan Galea](https://github.com/blurrypiano).

## Features

-   **Cross-Platform** (currently Windows-only; other platforms coming soon)
-   **Vulkan Renderer** for efficient and cross-platform graphics rendering
-   **Lighting Support**: Simple point and directional lights
-   **Alpha Blending**: Basic level alpha blending is implemented
-   **.OBJ Model Loading**: Supports loading `.obj` files exported from any 3D modeling software (test files available in the `res\models` folder)

## Dependencies

VulkanNuts uses the following libraries and tools:

-   Vulkan SDK for the Vulkan API
-   [GLFW](https://www.glfw.org/) for windowing
-   [GLM](https://github.com/g-truc/glm) for mathematics

All necessary dependencies are included in the `vendor` folder.

## Build Instructions

### Prerequisites:

-   Visual Studio 2022 (required for building)
-   Windows (support for other platforms is in progress)

### Steps:

1.  Clone the repository:
    ```
    git clone https://github.com/freezin-fire/vulkannuts.git
    ```
    
2.  Open the provided Visual Studio project file (`.vcproj`) in Visual Studio 2022.
3.  Build the solution within Visual Studio.

### Controls:
-   `W`, `A`, `S`, `D`: Strafe movement
-   `E`, `Q`: Move up/down
-   Arrow Keys: Camera movement

## Platform Support

-   **Currently Supported**: Windows
-   **Planned Support**: Linux, macOS

## Contributing

Contributions are welcome! Feel free to fork the repository, make changes, and submit a pull request. Please ensure that your code follows the existing code style and includes documentation for any new features.

## License

This project is licensed under the MIT License.
