# VulcaoTemplate

A minimal, cross-platform starting point for Vulkan applications built with
[vulcao](https://github.com/Yang-Junjie/vulcao) and SDL3. It opens a resizable
window, clears it every frame, handles resize and out-of-date swapchains, and
exits cleanly. Use it as a GitHub template and add your own passes and shaders.

## What is included

- A runnable Vulkan application skeleton built on vulcao and SDL3
- Window, Vulkan surface, swapchain and per-frame synchronization already wired up
- Resize and out-of-date swapchain handling, plus clean shutdown
- CMake presets for Linux (clang) and Windows (clang-cl, MSVC)
- GitHub Actions CI for Linux and Windows
- Vendored `tinyobjloader` and `stb`

## Requirements

- CMake 3.28+
- A C++23 compiler
- A recent Vulkan SDK with 1.4 headers (vulcao targets Vulkan 1.3 devices)
- Ninja (recommended)

## Quick start

### Linux

```sh
cmake --preset clang-linux-debug
cmake --build --preset clang-linux-debug --target app
./build/bin/app
```

### Windows

```sh
cmake --preset clang-debug
cmake --build --preset clang-debug --target app
.\build\bin\app.exe
```

The `msvc` and `vs-clangcl` presets require Visual Studio 18 2026. Close the
window or press Escape to exit.

## Using this template

1. Click **Use this template** and create a new repository.
2. Rename the places that still say `VulcaoTemplate`:
   - `project(VulcaoTemplate)` in `CMakeLists.txt`
   - `app_name` and the window title in `src/main.cpp`
   - the `app` target in `src/CMakeLists.txt`, if you want a different executable name
   - preset display names in `CMakePresets.json`, if you want
3. Configure with one of the presets above and start adding passes.

## Project layout

```
cmake/                  dependency and toolchain fragments
  dependencies.cmake    includes every third-party fragment
  third_party_*.cmake   FetchContent declarations and pinned versions
src/                    application sources and CMake target
shaders/                shader sources for your own pipelines
vendor/                 small vendored dependencies
```

## Dependencies

`vulcao`, SDL3, glm and spdlog are downloaded with FetchContent and pinned in
`cmake/third_party_*.cmake`. `tinyobjloader` and `stb` are vendored under
`vendor/` and added as subdirectories. Delete any dependency you do not need from
`cmake/dependencies.cmake` and `src/CMakeLists.txt`.

## License

MIT, see [LICENSE](LICENSE).
