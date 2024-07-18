# Personal C++ Game Engine

A versatile personal C++ game engine that features 2D/3D math and physics libraries, advanced rendering systems, skeletal animation system and essential game development tools.

## Key Features

- **2D/3D Math & Physics Library:**
  - Raycast utilities for AABBs, OBBs, Capsules, Discs, Spheres, Line Segments, Cylinders, Planes, and Convex Hulls.
  - Collision detection for Discs and Spheres vs AABBs, OBBs, Capsules, Lines, Cylinders, and Convex Hulls.
  - Math utility functions including dot product, cross product, projections, easing functions, splines, rangemap, and lerp.
  
- **Animation System:**
  - Real-time character animation system with a state machine-based animation controller.
  - Playback of animation from FBX files with LERP blending and crossfade controller for smooth transitions between keyframes.
  - Runtime root motion data manipulation using curve editing and selective sampling.
    
- **General Systems:**
  - Multi-threaded job system for enhanced performance.
  - Debug rendering system.
  - Buffer reader and writer for efficient file operations.
  - Event system for managing in-game events.
  - Development console for debugging and real-time command execution.
  - Audio system utilizing FMOD.
  - Networking capabilities with TCP.
  - Parsers for various file types including binary files, FBX, OBJs, XMLs, and text files.

- **Rendering Pipeline:**
  - DirectX 11 support for texturing, vertex normal lightin, HLSL shaders, sprite animations, sprite sheets, and bitmap fonts.

- **Additional Features:**
  - Split screen functionality.

## What I Learned

### 2D/3D Math & Physics
Developing the math and physics library deepened my understanding of advanced mathematical concepts and their applications in game development. Implementing raycasting and collision detection taught me about spatial algorithms and geometric computations.

### Animation System
Building the real-time character animation system improved my knowledge of animation techniques, state machines, and FBX file handling. Implementing LERP blending and crossfade controllers taught me about smooth keyframe transitions, while runtime root motion data manipulation using curve editing and selective sampling enhanced my ability to manage and modify animations dynamically.

### Multi-threading and Performance
Creating a multi-threaded job system enhanced my knowledge of concurrent programming, allowing me to optimize performance by efficiently distributing workloads across multiple threads.

### File I/O and Networking
Implementing file parsers and networking capabilities broadened my understanding of data serialization, deserialization, and network protocols, enabling me to handle various file formats and establish TCP connections for multiplayer features.

### Audio and Rendering
Integrating FMOD for audio and DirectX 11 for rendering expanded my skills in handling multimedia components, from sound effects to complex shader programming, sprite animations, and texturing techniques.

This project showcases the learning journey I undertook, demonstrating my ability to tackle complex problems, design efficient systems, and implement advanced features in a game engine.
