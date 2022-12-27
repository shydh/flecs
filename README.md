![flecs](docs/img/logo.png)

## Introduction
[![Version](https://img.shields.io/github/v/release/sandermertens/flecs?include_prereleases&style=for-the-badge)](https://github.com/SanderMertens/flecs/releases)
[![MIT](https://img.shields.io/badge/license-MIT-blue.svg?style=for-the-badge)](/LICENSE)
[![Documentation](https://img.shields.io/badge/docs-flecs-blue?style=for-the-badge&color=blue)](https://www.flecs.dev/flecs/md_docs_Docs.html)
[![actions](https://img.shields.io/github/actions/workflow/status/SanderMertens/flecs/ci.yml?branch=master&style=for-the-badge)](https://github.com/SanderMertens/flecs/actions?query=workflow%3ACI)
[![Discord Chat](https://img.shields.io/discord/633826290415435777.svg?style=for-the-badge&color=%235a64f6)](https://discord.gg/BEzP5Rgrrp)

Flecs is a fast and lightweight Entity Component System that lets you build games and simulations with millions of entities ([join the Discord!](https://discord.gg/BEzP5Rgrrp)). Here are some of the framework's highlights:

- Fast and [portable](#language-bindings) zero dependency [C99 API](https://www.flecs.dev/flecs/group__c.html)
- Modern type-safe [C++11 API](https://www.flecs.dev/flecs/group__cpp.html) that doesn't use STL containers
- First open source ECS with full support for [Entity Relationships](https://www.flecs.dev/flecs/md_docs_Relationships.html)!
- Fast native support for [hierarchies](https://www.flecs.dev/flecs/md_docs_Relationships.html#autotoc_md277) and [prefabs](https://www.flecs.dev/flecs/md_docs_Relationships.html#autotoc_md275)
- Minimal ECS core with optional [addons](#addons)
- Entire codebase builds in less than 5 seconds
- Runs [in the browser](https://flecs.dev/city) without modifications with emscripten
- Cache friendly [archetype/SoA storage](https://ajmmertens.medium.com/building-an-ecs-2-archetypes-and-vectorization-fe21690805f9) that can process millions of entities every frame
- Supports entities with hundreds of components and applications with tens of thousands of archetypes
- Automatic component registration that works out of the box across shared libraries/DLLs
- Write free functions with [queries](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/queries/basics) or run code automatically in [systems](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/systems/pipeline)
- Run games on multiple CPU cores with a fast lockless scheduler
- Compiles warning-free on 8 compilers on all major platforms, with [CI](https://github.com/SanderMertens/flecs/actions) running more than 4000 tests
- Integrated [reflection framework](https://www.flecs.dev/flecs/group__c__addons__meta.html) with [JSON serializer](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/reflection/basics_json) and support for [runtime components](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/reflection/runtime_component)
- [Unit annotations](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/reflection/units) for components
- Powerful [query language](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/rules) with support for [joins](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/rules/setting_variables) and [inheritance](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/rules/component_inheritance)
- [Statistics addon](https://www.flecs.dev/flecs/group__c__addons__stats.html) for profiling ECS performance
- A web-based dashboard ([demo](https://flecs.dev/explorer), [code](https://github.com/flecs-hub/explorer)) for inspecting entities, running ECS queries and monitoring games:

[![Dashboard image](docs/img/explorer.png)](https://flecs.dev/explorer)

## What is an Entity Component System?
ECS is a new way of organizing code and data that lets you build games that are larger, more complex and are easier to extend.

Something is called an ECS when it:
- Has _entities_ that uniquely identify objects in a game
- Has _components_ which are datatypes that can be added to entities
- Has _systems_ which are functions that run for all entities matching a component _query_

For example, a game has a `Move` _system_ that has a _query_ with two _components_, `Position, Velocity`. When the system is ran it is dynamically matched with all _entities_ that have at least these two components.

For more info on ECS, check the [ECS FAQ](https://github.com/SanderMertens/ecs-faq)!

## Getting Started
To use Flecs, add the [flecs.c](https://raw.githubusercontent.com/SanderMertens/flecs/master/flecs.c) and [flecs.h](https://raw.githubusercontent.com/SanderMertens/flecs/master/flecs.h) files to your project. When importing the files into a C++ project, make sure to compile [flecs.c](https://raw.githubusercontent.com/SanderMertens/flecs/master/flecs.c) as C code (for example by using `gcc` and `clang` instead of `g++` and `clang++`).

Flecs can also be built as a standalone library, by using the cmake, meson, bazel or [bake](https://github.com/SanderMertens/bake) build files. If you are using a custom build file to compile Flecs as a library, make sure to define `flecs_EXPORTS`, for example by adding `-Dflecs_EXPORTS` to the compiler command.

If you want to use the [flecs.c](https://raw.githubusercontent.com/SanderMertens/flecs/master/flecs.c) and [flecs.h](https://raw.githubusercontent.com/SanderMertens/flecs/master/flecs.h) files to build a standalone library, make sure to remove this line from the top of the [flecs.h](https://raw.githubusercontent.com/SanderMertens/flecs/master/flecs.h) file:

```c
#define flecs_STATIC
```

If you are building on Windows with mingw/gcc/clang, add `-lWs2_32` to the linker command (only needed for the HTTP/REST addons).

Make sure to compile C++ files as at least C++11 by adding `-std=c++0x` or higher to gcc/clang compile commands.

By default Flecs includes many features that may not be useful for every project. Builds can be customized to minimize the overhead of the library. See the [Addons](#addons) section for more information on customized builds.

## Show me the code!
C99 example:
```c
typedef struct {
  float x, y;
} Position, Velocity;

void Move(ecs_iter_t *it) {
  Position *p = ecs_field(it, Position, 1);
  Velocity *v = ecs_field(it, Velocity, 2);

  for (int i = 0; i < it->count; i++) {
    p[i].x += v[i].x;
    p[i].y += v[i].y;
  }
}

int main(int argc, char *argv[]) {
  ecs_world_t *ecs = ecs_init();

  ECS_COMPONENT(ecs, Position);
  ECS_COMPONENT(ecs, Velocity);

  ECS_SYSTEM(ecs, Move, EcsOnUpdate, Position, Velocity);

  ecs_entity_t e = ecs_new_id(ecs);
  ecs_set(ecs, e, Position, {10, 20});
  ecs_set(ecs, e, Velocity, {1, 2});

  while (ecs_progress(ecs, 0)) { }
}
```

Same example in C++11:
```cpp
struct Position {
  float x, y;
};

struct Velocity {
  float x, y;
};

int main(int argc, char *argv[]) {
  flecs::world ecs;

  ecs.system<Position, const Velocity>()
    .each([](Position& p, const Velocity& v) {
      p.x += v.x;
      p.y += v.y;
    });

  auto e = ecs.entity()
    .set([](Position& p, Velocity& v) {
      p = {10, 20};
      v = {1, 2};
    });

  while (ecs.progress()) { }
}
```

## Projects using Flecs
If you have a project you'd like to share, let me know on [Discord](https://discord.gg/BEzP5Rgrrp)!

### Territory Control
https://store.steampowered.com/app/690290/Territory_Control_2/
![image](docs/img/projects/territory_control.png)

### Sol Survivor
https://nicok.itch.io/sol-survivor-demo
![image](docs/img/projects/sol_survivor.png)

### The Forge
https://github.com/ConfettiFX/The-Forge
![image](docs/img/projects/the_forge.png)

### Equilibrium Engine
https://github.com/clibequilibrium/EquilibriumEngine
![image](docs/img/projects/equilibrium_engine.png)

### Gravitas
https://thepunkcollective.itch.io/gravitas
![image](docs/img/projects/gravitas.png)

### After Sun
https://github.com/foxnne/aftersun
![image](docs/img/projects/after_sun.png)

### Tower defense (open source demo)
https://www.flecs.dev/tower_defense/etc ([repository](https://github.com/SanderMertens/tower_defense))
![image](docs/img/projects/tower_defense.png)

### Procedural City (open source demo)
https://www.flecs.dev/city ([repository](https://github.com/flecs-hub/city))
![image](docs/img/projects/city.png)

## Resources

### Documentation
- [Flecs Documentation](https://www.flecs.dev/flecs/index.html)

### Resources provided by the community :heart:
- [Unreal Minimum Viable Flecs Project](https://github.com/PreyK/Unreal-Minimum-Viable-Flecs)
- [Bgfx/Imgui module](https://github.com/flecs-hub/flecs-systems-bgfx/tree/bgfx_imgui)
- [Tower defense example](https://gist.github.com/oldmanauz/b4ced44737bf9d248233538fa06a989e)
- [Flecs + UE4 is magic](https://jtferson.github.io/blog/flecs_and_unreal/)
- [Quickstart with Flecs in UE4](https://jtferson.github.io/blog/quickstart_with_flecs_in_unreal_part_1/) 
- [Automatic component registration in UE4](https://jtferson.github.io/blog/automatic_flecs_component_registration_in_unreal/)
- [Building a space battle with Flecs in UE4](https://twitter.com/ajmmertens/status/1361070033334456320) 
- [Flecs + SDL + Web ASM example](https://github.com/HeatXD/flecs_web_demo) ([live demo](https://heatxd.github.io/flecs_web_demo/))
- [Flecs + Raylib example](https://github.com/Lexxicon/FlecsRaylib)
- [Flecs + gunslinger example](https://github.com/MrFrenik/gs_examples/blob/main/ex_demos/flecs/source/main.c)
- [Flecs based 3D game engine with editor](https://bit.ly/3T9cc1o)

### Flecs links
- [Discord](https://discord.gg/BEzP5Rgrrp)
- [Medium](https://ajmmertens.medium.com)
- [ECS FAQ](https://github.com/SanderMertens/ecs-faq)
- [Twitter](https://twitter.com/ajmmertens)
- [Reddit](https://www.reddit.com/r/flecs)

## Addons
Flecs has a modular architecture that makes it easy to only build the features you really need. By default all addons are built. To customize a build, first define `FLECS_CUSTOM_BUILD`, then add defines for the addons you need. For example:

```c
#define FLECS_CUSTOM_BUILD  // Don't build all addons
#define FLECS_SYSTEM        // Build FLECS_SYSTEM
```

Additionally, you can also specify addons to exclude from a build by adding `NO` to the define:

```c
#define FLECS_NO_LOG
```

The following addons can be configured:

Addon         | Description                                      | Define              |
--------------|--------------------------------------------------|---------------------|
[Cpp](/flecs/group__cpp.html)                          | C++11 API                                        | FLECS_CPP           |
[Module](/flecs/group__c__addons__module.html)     | Organize game logic into reusable modules        | FLECS_MODULE        |
[System](flecs/group__c__addons__system.html)      | Create & run systems                             | FLECS_SYSTEM        |
[Pipeline](/flecs/group__c__addons__pipeline.html) | Automatically schedule & multithread systems     | FLECS_PIPELINE      |
[Timer](/flecs/group__c__addons__timer.html)       | Run systems at time intervals or at a rate       | FLECS_TIMER         |
[Meta](/flecs/group__c__addons__meta.html)         | Flecs reflection system                          | FLECS_META          |
[Meta_C](/flecs/group__c__addons__meta_c.html)     | (C) Utilities for auto-inserting reflection data | FLECS_META_C        |
[Units](/flecs/group__c__addons__units.html)       | Builtin unit types                               | FLECS_UNITS         |
[Expr](/flecs/group__c__addons__expr.html)         | String format optimized for ECS data             | FLECS_EXPR          |
[JSON](/flecs/group__c__addons__json.html)         | JSON format                                      | FLECS_JSON          |
[Doc](/flecs/group__c__addons__doc.html)           | Add documentation to components, systems & more  | FLECS_DOC           |
[Coredoc](/flecs/group__c__addons__coredoc.html)   | Documentation for builtin components & modules   | FLECS_COREDOC       |
[Http](/flecs/group__c__addons__http.html)         | Tiny HTTP server for processing simple requests  | FLECS_HTTP          |
[Rest](/flecs/group__c__addons__rest.html)         | REST API for showing entities in the browser     | FLECS_REST          |
[Parser](/flecs/group__c__addons__parser.html)     | Create entities & queries from strings           | FLECS_PARSER        |
[Plecs](/flecs/group__c__addons__plecs.html)       | Small utility language for asset/scene loading   | FLECS_PLECS         |
[Rules](/flecs/group__c__addons__rules.html)       | Powerful prolog-like query language              | FLECS_RULES         |
[Snapshot](/flecs/group__c__addons__snapshot.html) | Take snapshots of the world & restore them       | FLECS_SNAPSHOT      |
[Stats](/flecs/group__c__addons__stats.html)       | See what's happening in a world with statistics  | FLECS_STATS         |
[Monitor](/flecs/group__c__addons__monitor.html)   | Periodically collect & store statistics          | FLECS_MONITOR       |
[Log](/flecs/group__c__addons__log.html)           | Extended tracing and error logging               | FLECS_LOG           |
[Journal](/flecs/group__c__addons__journal.html)   | Journaling of API functions                      | FLECS_JOURNAL       |
[App](/flecs/group__c__addons__app.html)           | Flecs application framework                      | FLECS_APP           |
[OS API Impl](/flecs/group__c__addons__os__api__impl.html) | Default OS API implementation for Posix/Win32 | FLECS_OS_API_IMPL   |

## Flecs Hub
[Flecs Hub](https://github.com/flecs-hub) is a collection of repositories that show how Flecs can be used to build game systems like input handling, hierarchical transforms and rendering.

Module      | Description      
------------|------------------
[flecs.components.cglm](https://github.com/flecs-hub/flecs-components-cglm) | Component registration for cglm (math) types
[flecs.components.input](https://github.com/flecs-hub/flecs-components-input) | Components that describe keyboard and mouse input
[flecs.components.transform](https://github.com/flecs-hub/flecs-components-transform) | Components that describe position, rotation and scale
[flecs.components.physics](https://github.com/flecs-hub/flecs-components-physics) | Components that describe physics and movement
[flecs.components.geometry](https://github.com/flecs-hub/flecs-components-geometry) | Components that describe geometry
[flecs.components.graphics](https://github.com/flecs-hub/flecs-components-graphics) | Components used for computer graphics
[flecs.components.gui](https://github.com/flecs-hub/flecs-components-gui) | Components used to describe GUI components
[flecs.systems.transform](https://github.com/flecs-hub/flecs-systems-transform) | Hierarchical transforms for scene graphs
[flecs.systems.physics](https://github.com/flecs-hub/flecs-systems-physics) | Systems for moving objects and collision detection
[flecs.systems.sokol](https://github.com/flecs-hub/flecs-systems-sokol) | Sokol-based renderer
[flecs.game](https://github.com/flecs-hub/flecs-game) | Generic game systems, like a camera controller

## Language bindings
The following language bindings have been developed with Flecs! Note that these are projects built and maintained by helpful community members, and may not always be up to date with the latest commit from master!
- [Lua](https://github.com/flecs-hub/flecs-lua)
- [Zig #1](https://github.com/foxnne/zig-flecs) [#2](https://github.com/prime31/zig-flecs)
- [C#](https://github.com/flecs-hub/flecs-cs) [#2](https://git.mcft.net/copygirl/gaemstone.ECS)
- [Rust](https://github.com/jazzay/flecs-rs)

## Supporting Flecs ♥️
Supporting Flecs goes a long way towards keeping the project going and the community alive! If you like the project, consider:
- Giving it a star 🌟
- Becoming a sponsor: https://github.com/sponsors/SanderMertens
