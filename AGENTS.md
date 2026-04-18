# AGENTS

## Scope
- This project is a single-file raylib game.
- Source of truth is the workspace root, especially [main.cpp](main.cpp), [Makefile](Makefile), [Score.txt](Score.txt), and [assets/](assets/).

## Build And Debug
- Preferred in VS Code: run task build debug for development and build release for release builds (see [.vscode/tasks.json](.vscode/tasks.json)).
- Windows toolchain expected by default:
  - C:/raylib/w64devkit/bin/mingw32-make.exe
  - RAYLIB_PATH=C:/raylib/raylib
- Makefile defaults:
  - PLATFORM=PLATFORM_DESKTOP
  - BUILD_MODE=RELEASE
  - C++14
- Debug launch config uses preLaunchTask build debug (see [.vscode/launch.json](.vscode/launch.json)).

## Runtime Assumptions
- Run the executable with workspace root as working directory so relative asset paths resolve.
- Assets are loaded from assets/... paths in [main.cpp](main.cpp).
- High score persistence uses root [Score.txt](Score.txt).

## File Ownership Guidance
- Edit gameplay logic in [main.cpp](main.cpp).
- Keep build behavior in [Makefile](Makefile) and [.vscode/tasks.json](.vscode/tasks.json) aligned when changing build flags.
- Do not modify generated/distribution artifacts unless explicitly asked:
  - *.exe, *.zip
  - nested distribution folders under [Trash Rush/](Trash Rush/)

## Project Conventions
- Module-level constants are UPPER_SNAKE_CASE.
- Functions and mutable variables are camelCase.
- Global state and game loop are intentionally centralized in [main.cpp](main.cpp).

## Common Pitfalls
- If raylib is not installed in expected locations, build will fail until paths are adjusted.
- Running from the wrong working directory causes missing assets and score file issues.
- There are duplicate packaged copies under [Trash Rush/](Trash Rush/); treat root files as canonical for development.

## Useful References
- Project note: [README.txt](README.txt)
- Build system: [Makefile](Makefile)
- VS Code build tasks: [.vscode/tasks.json](.vscode/tasks.json)
- VS Code debugger setup: [.vscode/launch.json](.vscode/launch.json)