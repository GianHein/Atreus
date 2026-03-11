# `Engine/Platform`

The `Engine/Platform` module provides the platform abstraction layer as well as the platform-specific
implementations for Atreus. It's statically linked into all of the runtime modules, and the launcher.

The PAL is minimal, and only provides abstractions for which a specific need arose during development. Eventually,
the PAL should be extended to cover a broader range of platform-specific functionality, and to provide a more
consistent API across all platforms but for now, it only provides the following functionality:

- `Console`: Console abstraction for logging and debugging output
- `Library`: Library abstraction for loading and unloading dynamic libraries at runtime
- `Time`: Monotonic and wall clock time abstractions for measuring time and scheduling tasks
- `Window`: Window abstraction for creating and managing windows

TODO:

- `File`: File system abstractions for reading and writing files, and for enumerating directories
- `Input`: Input abstractions for handling keyboard, mouse, and gamepad input
- `Threading`: Threading abstractions for creating and managing threads, mutexes, and other synchronization primitives

## Platform-Specific Implementations

| Platform | Console | Library | Time | Window |
| -------- | ------- | ------- | ---- | ------ |
| Windows  | ✅      | ✅\*    | ✅   | ⚠️     |
| Linux    | ❌      | ❌      | ❌   | ❌     |
| macOS    | ❌      | ❌      | ❌   | ❌     |

Legend:

- ✅: Implemented and fully functional
- ⚠️: Implemented but with some limitations or missing features
- ❌: Not implemented

\* The library abstraction is not yet running on `Atr::String`, also instead of having `LoadSymbol` return
a `void*`, there should be a `Symbols` field on the `LibraryHandle` struct to hold all loaded symbols
