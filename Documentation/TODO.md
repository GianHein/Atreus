1. [/] Foundation
   - [x] Window and platform layer
   - [x] Input
   - [ ] Console + file logging
   - [ ] Tracy
   - [ ] Minimal 2D renderer
   - [ ] Fixed timestep with accumulator
   - [ ] Basic collision
   - [ ] Small finished game shipped as a zip
2. [ ] Scale
   - Arena allocator
   - Stack allocator
   - Pool allocator
   - Tracking/debug allocator wrapper
   - Scoped CPU profiler
   - Array, SmallArray, Span
   - HashMap (open addressing)
   - SparseSet
   - RingBuffer
   - BitSet, StringId
   - Thread-based job system with work stealing
   - Single-threaded fallback route
   - Lock-free primitives
   - Per-thread arenas
   - Harness: large agent simulation, spatial hashing, parallel systems, frame-time graph (50,000 dots avoiding each other at 144Hz, flat frame time)
3. [ ] Interlude: standalone Vulkan project
   - Outside the engine repo
   - glTF scene viewer
   - Shadows
   - Small render graph
   - Not integrated, discarded afterwards
4. [ ] Content and 3D
   - 3D camera, perspective, depth buffer, frustum culling
   - Offline bake pipeline and importers
   - Virtual file system with pak archives
   - Handle-based asset registry
   - Async loading on the job system
   - Reference counting, eviction, memory budgets
   - Asset dependency graph
   - Hot reload
   - Archetype ECS
   - Type reflection and component metadata
   - Deferred structural changes via command buffers
   - System scheduler on the job system
   - Scene serialization
   - Audio backend and mixer
   - Lock-free SPSC command queue to audio thread
   - Voice pool, streaming voices, sample-accurate scheduling
   - Harness: fly through a scene that's bigger than RAM fits without hitching frames
5. [ ] Vulkan
   - Instance, device, queues, validation layers
   - Swapchain with recreation
   - Frames in flight
   - VMA
   - Descriptor management, bindless set, pipeline cache
   - Render graph with barrier inference
   - Offline SPIR-V compilation with reflection
   - Shader hot reload
   - Multithreaded command buffer recording
   - GPU-driven culling and indirect draw
   - RHI abstraction extracted from both backends
   - Harness: large-scale scene, many materials, dynamic shadows, post-processing (same scene, old renderer chokes, this one doesn't)
6. [ ] Capstone
   - Fiber job system, keeping thread and serial modes working
   - Editor and tooling
   - Physics via Jolt
   - Spatial audio and DSP graph
   - Animation with blend trees
   - Navmesh
   - Remaining gameplay systems
   - Whatever you actually want to build

Deferred until there is a reason:

- Linux platform implementation
- PAL extracted from two platforms
- Emscripten platform implementation
- WebGPU backend
- Module/plugin architecture

Rules:

- No abstraction until two implementations exist
- Refactor between projects, timeboxed to two weeks, never mid-project
- Every system needs a measurement that justifies it
- One running artifact at the end of every project
- Write publicly about each one

Where

- [ ] = not started
- [/] = in progress
- [x] = completed
