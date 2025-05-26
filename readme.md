# 3D Maze Game with OpenGL

## Overview

This is a simple 3D maze game implemented using OpenGL and GLFW, where the player controls a cube character navigating through a maze to collect treasures. The game features keyboard controls, collision detection with maze walls, treasure interactions, special teleport conditions, and multi-threaded sound and visual effects.

---

## Features

* **Character Movement:** Move the player cube with keys `J` (left), `L` (right), `I` (up), and `K` (down).
* **Collision Detection:** Prevent movement through walls in the maze.
* **Treasure Interaction:** Collect treasure cubes to trigger special effects like sound playback and animations.
* **Special Teleport Conditions:** Teleport the character when reaching specific coordinates.
* **Multi-threading:** Plays MP3 sounds and triggers visual effects (like flashing) in separate threads for smooth gameplay.
* **Textured Treasures:** Treasures are rendered with textures.
* **Basic 3D Rendering:** Maze walls and characters are rendered as colored cubes.

---

## Controls

| Key   | Action     |
| ----- | ---------- |
| J     | Move Left  |
| L     | Move Right |
| I     | Move Up    |
| K     | Move Down  |
| SPACE | Exit Game  |

---

## Code Structure Highlights

* **Input Handling:** Uses GLFW key callbacks and flags (`key_J_pressed`, etc.) to manage movement and prevent repeated actions.
* **Collision and Interaction Logic:** Checks maze grid before moving the character to avoid walls.
* **Multi-threading:** Sound and animation functions (`playMP3Thread`, `minimizeTreasureThread`, `FlashThread`) run asynchronously.
* **Rendering:**

  * Maze and character are drawn using vertex buffers.
  * Treasure cubes use texture mapping with UV coordinates.
* **Special Conditions:** Teleportation when character reaches designated positions.
* **Performance:** Frame delay (`std::this_thread::sleep_for`) approximates \~60 FPS.

---

## Dependencies

* OpenGL 3.x or higher
* GLFW
* GLEW or another OpenGL extension loader
* C++11 or higher (for `std::thread` support)
* MP3 playback library (for `playMP3Thread` function)

---

## Building and Running

1. Install GLFW and GLEW libraries on your system.
2. Compile the program with a C++ compiler that supports C++11 or higher.
3. Link with OpenGL, GLFW, GLEW, and your MP3 playback library.
4. Run the executable. Use the keyboard controls to navigate the maze.

---

## Notes

* Make sure the maze array (`maze`) and all necessary assets (textures, MP3 files) are correctly loaded.
* The current teleport conditions and treasure interaction coordinates are hardcoded; modify them as needed for your maze layout.
* Sound and animation threads are detached to run independently and avoid blocking main rendering loop.
* This game was created by beginners, to make it easier for amateurs to understand the core ideas behind game development. It focuses on simple mechanics and clear code so we can all learn and improve step by step together. 

---
