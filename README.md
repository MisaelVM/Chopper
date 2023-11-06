# Chopper Engine

## Overview
Chopper Engine is a dynamic C++ game engine currently in the early stages of development. With a vision to evolve into a versatile and high-performance framework, Chopper Engine aims to harness the capabilities of Vulkan as its initial backend renderer, with plans to incorporate support for additional renderers in the future. Although not yet feature-complete, the project is dedicated to establishing a robust foundation for game development, emphasizing flexibility and performance.

## Requirements
- Microsoft Visual Studio 2022
- CMake 3.18+
- Vulkan SDK/Vulkan 1.3 support
- [vcpkg](https://github.com/microsoft/vcpkg)
- Ninja Build

## Getting Started

### Windows
1. Clone the repository: `git clone https://github.com/MisaelVM/Chopper.git`
2. Navigate to the project directory: `cd Chopper`
3. Build the project using Visual Studio by opening the project root directory with the IDE or use the command `cmake --preset <preset>`. **NOTE:** Configured presets assume that the environment variable `VCPKG_ROOT` exists with the root direction of `vcpkg` as its value.

### Linux
No support at the moment.

## Configuration
You can edit the `CMakePresets.json` file to configure your own presets and customize the build.

## Future Roadmap
- **Linux Support:** Adding support for the Linux operating system.
- **Graphics APIs:** Expanding rendering options with support for OpenGL and DirectX.


## License
Chopper Engine is licensed under the [Apache License 2.0](LICENSE).
