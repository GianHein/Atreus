<!--Yes, I shamelessly pulled this from floooh/sokol's README.md -->

<p align="center">
  <img src="Documentation/Assets/logo_full_large.png" style="width: 60%" /><br/><br/>
  Lightweight GPLv3-licensed game making framework/engine written in C++
</p>

# Atreus

Atreus is my personal game engine programming project. It's first and foremost an educational project meant for experimentation, not intended for production use.

In many places the architecture and implementation I settled on are over-engineered for the sake of learning and experimentation, while in other places it's underdeveloped due to a lack of time and skill.

## Current Status

Atreus is not yet in a state where it can really be used for anything substantial. My first milestone is a simple Breakout clone. Before I finish that though, I'll first revisit some OpenGL and rendering fundamentals and improve my 2D rendering infrastructure.

> [!WARNING]
> Although Atreus includes a Linux/X11 platform implementation, Windows 11 is currently the only platform I actively develop and test on.

## Building Atreus

After cloning the repository, the first thing you'll most likely want to do is build the whole thing. Atreus uses CMake.

To build you need: CMake v3.25+, a C++20-compatible compiler

Execute the following commands in the root directory of the project to configure and build the project:

```bash
cmake -S . -B Build
cmake --build Build
```

Normally, if everything worked, the executable and libraries should all be placed in `./Bin/Debug`, given that you didn't use another build configuration.

> [!NOTE]
> At this point in time, the only way to build Atreus is to build separate libraries for engine and application while having the launcher be the executable. In the future, fully static release builds are definitely planned, I just haven't yet had the time (or was too lazy) to implement that.

To then run an application using Atreus (i.e. the provided Breakout sample):

```bash
./Bin/Debug/Launcher.exe Breakout
```

Replace `Breakout` with the name of the application library.

## Project Structure

Atreus is organized into the following architectural layers:

| Name                                                | Description                                                                         |
| --------------------------------------------------- | ----------------------------------------------------------------------------------- |
| [Common](Engine/Common/Public/Atreus/)              | Dependency root; type aliases, global macros, allocators, logging, containers       |
| [Platform](Engine/Platform/Public/Atreus/Platform/) | Platform abstraction layer and implementations for Win32 and Linux (X11)            |
| [Core](Engine/Core/Public/Atreus/Core/)             | State-owning subsystems; for now: input; in the future: jobs, registry              |
| [Runtime](Engine/Runtime/Public/Atreus/Runtime/)    | Application and simulation; for now: application; in the future: ECS, world, assets |
| [Graphics](Engine/Graphics/Public/Atreus/Graphics/) | Graphics module API and OpenGL implementation                                       |

The project also includes the following:

| Name                                                | Description                                                                            |
| --------------------------------------------------- | -------------------------------------------------------------------------------------- |
| [Launcher](Launcher/Source/Main.cpp)                | Executable and thin bootstrapper, initializes and loads the engine and application     |
| [Application](Samples/Breakout/Source/Breakout.cpp) | Implements the application API defined in Runtime and contains application-level logic |

## Project History

The state of this branch in the repo is the result of multiple rewrites and an engine programming journey that started in 2024, following [Java tutorials](https://www.youtube.com/playlist?list=PLtrSb4XxIVbp8AKuEAlwNXDxr99e3woGE) without understanding or being able to extend the code I was writing in any meaningful capacity. Feel free to explore the branches marked as `archived/...` to see what I was doing since March 2026.

In hindsight, none of these rewrites were fully necessary but each time I felt I'd made fundamental mistakes that were too significant to address through refactoring. The separation in my project structure, my very intentional and rather minimal platform API, approach for allocators, and a lot more is all stuff that has accumulated over the course of those rewrites.

Over time, I've come to recognize how counterproductive starting from scratch every couple of months can be. Going forward, I'll improve this codebase incrementally and actually finish what I've started.

## License

Atreus is licensed under the GNU General Public License v3.0. See [LICENSE](LICENSE) for details.
