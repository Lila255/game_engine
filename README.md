## Game Engine
I have created this engine + game so I could learn OpenGL and get more experience with C++. But mainly, I wanted to create a game.<br>

The game that I am developing is a 2D side-on texel based sandbox game(idea copied from Noita) with a bunch of physics simulations. 
I want to create a resource/factory management game where the player builds a factory to automate resource gathering and refining(copied from Oxygen not Included/Factorio). 
This would require an ever increasing supply of resources, which would require more and more power generation to sustain the factory.<br>

I want to make everything in the game a physical tile. Trees grow as solid tiles, which can be chopped down to yield wood tiles. The wood tiles can be burned as fuel to release heat energy, which can be used to boil water for steam power or to melt metal ore tiles. It would roughly follow the same power generation progression as Oxygen not Included/Factorio: Wood/Coal -> Oil -> Nuclear.<br>

Combustion would also release CO2 tiles, which the trees would then need to absorb in order to grow. 
It should be a circular system, where mass is conserved (mostly). 
Energy will not be conserved, and I do not plan to simulate this accurately as it is far to complex and not fun to play.
The player would need to balance the factory's resource consumption with the environment's ability to sustain it. If the player burns too much coal/wood and fills the environment with CO2 tiles, then combustion would stop working. 
They would need to ensure that the factory does not fail from heat buildup, which would prevent trees from growing and could cause fires or eventually melt the factory itself.<br>

I have added a flying creature system(bees) and a legged creature system(spiders, centipedes). However, these depend on the box2d physics simulation which is only generated for the chunks surrounding the player. Therefore, these creatures would only be effective as an enemy against the player, and wouldn't be able to interact with the environment/factory if the player is not nearby.
I will need to add some external threat that would oppose the factory's expansion itself, rather than just being a threat to the player character.<br>

### Gifs
https://github.com/user-attachments/assets/580635cc-e4bf-42a1-9ac8-97a31d6aa7ca


https://github.com/user-attachments/assets/f4b30d4f-84f9-4ff1-b6db-df04b9da78fe

### Current Issues
- The lighting system needs work, something is wrong with the way the normals are calculated when the light rays bounce of the tiles.
- Something is wrong with tiles along the edge/moving between chunks, they sometimes have visual artifacts. Sometimes the tiles get deleted.
- The game is not thread safe. There are a bunch of mutexes to prevent concurrency issues, but a lot of stuff is not properly synchronized and will occasionally lead to a crash.
- Performance. I have not needed to do any performance optimizations yet. I should probably do some in the future.
  - The tiles that make up the chunks are moved by a task that can be run with a seperate thread for each chunk(or at least as many threads as allowed). On my machine, this task takes somewhere between 300us and 1,500us per chunk update, depending on how much is happening in the chunk. Considering the chunk updates happen at only 20 times a second, there is up to 50,000us for processing all the chunks. Of course, this would begin impacting the other systems that depend on the chunk system before hitting that limit.
  - The collision mesh generation is also multithreaded. This one is taking between 500us and 1500us per chunk as well. This is only run if the solid tiles in the chunk have been modified, and is also only run against the 3x3 chunks surrounding the player. This task is more important to optimise, as it locks the box2d thread while the mesh is being updated(although this isn't the entire run time of the task). This would cause stuttering and make the game never feel smooth if it takes too long.
  - The ray-tracing is done via a compute shader on the GPU. I had no idea what I was doing when I wrote this, so it is certainly not optimal. It runs fine for now, and I want to fix the appearance of the lighting before worrying about performance.
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
I have used ChatGPT and Github Copilot to assist with writing some of the code in this project. 
Although entirely my fault, I attribute most of the mess that is this codebase to the use of these tools.

### License
This project is source-available and free to use for any non-commercial purpose. 
You may use this code for learning and personal projects, but do not use it for commercial purposes without obtaining permission from the author.
