## Game Engine

I have created this engine and game as a learning experience to teach myself OpenGL as well as gain more experience with C++. <br>
I wanted to create a game.<br><br>

The game that I am developing is a 2D side-on texel based sandbox game(idea copied from Noita) with a bunch of physics simulations. I want to create a resource/factory management game where the player builds a factory to automate resource gathering and refining(copied from Oxygen not Included/Factorio). 
This would require an ever increasing supply of resources, which would require more and more power generation to sustain the factory. 
I want to make everything in the game a physical tile. Trees grow as solid tiles, which can be chopped down to yield wood tiles. The wood tiles can be burned as fuel to release heat energy, which can be used to boil water for steam power or to refine metal ore tiles into pure liquid metal tiles. Combustion would also release CO2 tiles, which the trees would then need to absorb in order to grow.<br>
The player would need to balance the factory's resource consumption with the environment's ability to sustain it. They would need to ensure that the factory does not fail from heat buildup, which would prevent trees from growing and could potentially cause fires or eventually melt the factory itself.<br>

### Gifs
https://github.com/user-attachments/assets/580635cc-e4bf-42a1-9ac8-97a31d6aa7ca


https://github.com/user-attachments/assets/f4b30d4f-84f9-4ff1-b6db-df04b9da78fe

### Current Issues
- The lighting system needs work, something is wrong with the way the normals are calculated when the light rays bounce of the tiles.
- Something is wrong with tiles along the edge/moving between chunks, they sometimes have visual artifacts.
- Performance. I have not needed to do any performance optimizations yet. I should probably do some in the future.
- There is no character controller yet. Key inputs merely apply forces/set velocities to the player's box2d body. This means the player can get stuck on corners and edges, and the movement feels very sluggish.
- Chunks are not saved to disk or unloaded from memory. Travelling far from spawn will fill up more and more memory.
- No audio system yet.
- No UI system yet.

### External libraries used
- Box2D [https://github.com/erincatto/box2d] - This is a fantastic open source library that provides 2d physics simulation.
- GLFW [https://www.glfw.org/] - This library provides a simple API for creating windows, contexts and handling input and events for OpenGL.
- GLEW [http://glew.sourceforge.net/] - The OpenGL Extension Wrangler Library. This helps with managing OpenGL extensions.
- siv::PerlinNoise [https://github.com/Reputeless/PerlinNoise] - A simple header-only Perlin Noise implementation in C++.
- Poly2Tri [https://github.com/jhasse/poly2tri] - A library for polygon triangulation.

### Use of AI/LLMs
I have used ChatGPT and Github Copilot to assist with writing some of the code in this project. That is likely a contributing factor to why the code is a complete mess.

### License
This project is source-available and free to use for any non-commercial purpose. You may use this code for learning and personal projects, but do not use it for commercial purposes without obtaining permission from the author.
