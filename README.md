# MYY702 - Computer Graphics and Interaction Systems

**Type:** Academic / Group Project  
**Development Platform:** C++, OpenGL  

# 3D Maze Game

## Project Overview
This is a 3D interactive maze game implemented in C++ using the OpenGL API, developed for the "MYY702: Computer Graphics and Interaction Systems" course. The player controls a character (cube) navigating through a grid-based maze. The objective is to explore the environment, avoid obstacles, and collect textured treasures while experiencing dynamic visual and audio feedback.

## Controls

### Player Movement
| Key     | Action         |
|---------|----------------|
| **J** | Move Left      |
| **L** | Move Right     |
| **I** | Move Up (Forward) |
| **K** | Move Down (Backward) |
| **SPACE**| Exit Game     |

### Camera Controls
| Key           | Action                 |
|---------------|------------------------|
| **W / X** | Rotate Camera          |
| **Q / Z** | Pan Camera             |
| **H / G** | Zoom In / Out          |
| **B / T** | Additional Adjustments |

## Gameplay & Technical Mechanics

* **Collision Detection:** Grid-based coordinate checking prevents the character from passing through maze walls.
* **Treasure Interaction:** Collecting treasure objects triggers asynchronous visual effects (like scaling animations) and audio playback.
* **Teleportation Events:** Reaching specific, predefined coordinates instantly teleports the player to new areas of the maze.
* **Multi-threading:** Sound playback (`playMP3Thread`) and visual animations (`FlashThread`, `minimizeTreasureThread`) utilize C++11 `std::thread` to run asynchronously, ensuring the main rendering loop remains non-blocking and smooth.
* **Rendering System:** Utilizes Vertex Buffer Objects (VBOs) for drawing 3D geometry and UV texture mapping for objects like treasures.

## Dependencies

* **Core API:** OpenGL 3.x or higher
* **Libraries:** GLFW (Window/Input), GLEW (Extension Loader), GLM (Mathematics)
* **Language:** C++11 or higher (required for `std::thread` support)
* **Audio:** MP3/WAV playback library (native or third-party depending on the OS)

## Building and Running

1. Install a C++ compiler that supports C++11 (e.g., MSVC via Visual Studio).
2. Configure your IDE/Makefile to link against `opengl32.lib`, `glfw3.lib`, and `glew32.lib` (or equivalent for your OS).
3. Ensure all local assets (textures, `.mp3`/`.wav` files, and shader files) are located in the executable's working directory.
4. Build the project and run the executable.
