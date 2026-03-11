![Atreus](./Resources/Media/Logo-White.svg)

# Atreus Engine

A small, low-level game engine project focused on learning and experimentation with
engine architecture and systems programming. The project intentionally keeps scope
limited while focusing on fundamental systems such as memory management,
multithreading, ECS architecture, rendering infrastructure, and runtime modularity.

This repository serves both as a learning project and as a long-term portfolio piece
for engine programming.

## Repository Structure

```sh
Engine/
  Base/           # Foundational and platform-agnostic code
  Launcher/       # Entry point
  Platform/       # Platform-specific implementations
  Runtime/        # Engine modules
    Core/         # Core engine systems (e.g. memory management, job system)
    Graphics/     # (Future)
    Physics/      # (Future)
Resources/
  Documentation/  # Markdown documentation for engine architecture and systems (WIP)
  Media/          # Images, diagrams, and other media assets
Sandbox/          # Testbed for experimenting with engine features and systems
```

## Design Principles

While Atreus is primarily a learning project, I have some guiding principles that I want
to follow in its design and implementation:

### Clear Dependency Direction

Dependencies always flow inward toward foundational code. Higher-level modules can
depend on lower-level modules, but not the other way around.

![Dependency Graph](./Resources/Media/Architecture.png)

### Runtime Modularity

While `Core/` is required for all programs built with Atreus, other modules like
`Graphics/` and `Physics/` are optional. This allows:

- headless builds
- simpler experimentation
- cleaner boundaries between systems

### Data Oriented Design

Where appropriate, I want to follow data-oriented approaches to make better use of
cache and memory as well as parallelism. This includes:

- archetype-based ECS
- chunk iteration
- job system scheduling

## Documentation

At the moment, documentation is a bit sparse, but I intend to write more detailed documentation
for the engine's architecture and systems as I continue development. For now, you can find some
basic high-level overviews in [Documentation/Index.md](./Resources/Documentation/Index.md).

## Status

Early development.

The project is currently focused on building the engine foundation and runtime structure.
It is not yet in a state where it can be used to build games, but the goal is to reach that
point eventually and then gradually add more features and systems over time, with actual
deliverables in mind.

## Motivation

Atreus exists primarily as an educational project focused on understanding how game engines
are built internally. By implementing core systems from scratch, I hope to gain a deeper
understanding of engine architecture, systems programming, and the trade-offs involved in
engine design.

## License

Atreus is licensed under the GNU General Public License v3.0 (GPL-3.0). See
[LICENSE](./LICENSE) for more details.
