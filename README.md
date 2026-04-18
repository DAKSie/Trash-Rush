# Trash Rush

## Introduction
Trash Rush is a fast paced 2D arcade game built with C++ and raylib. Dodge hazards, collect trash, and chase higher scores in short replayable sessions. The project includes a modernized source layout, build scripts, and asset based presentation today.

## Prerequisites

Install these required tools and files before building:

1. Windows 10 or Windows 11
2. MSYS2 UCRT64 toolchain with g++ at:
	- C:\msys64\ucrt64\bin\g++.exe
3. raylib runtime and headers available to that toolchain:
	- Include path: C:/msys64/ucrt64/include
	- Library path: C:/msys64/ucrt64/lib
	- Runtime DLL source: C:\msys64\ucrt64\bin
4. Project files at the workspace root:
	- main.cpp
	- src/ (all .cpp and .h files)
	- assets/
	- Score.txt
	- build.ps1

If you use VS Code, make sure PowerShell scripts are allowed for the current session.

## Build And Run

### Option 1: VS Code Tasks (recommended)

1. Run task: build debug
2. Launch the game from the workspace root:
	- .\bin\main.exe

For optimized builds, run task: build release

### Option 2: PowerShell

From the project root:

Debug build:
```powershell
.\build.ps1 -Configuration Debug
```

Release build:
```powershell
.\build.ps1 -Configuration Release
```

Run after build:
```powershell
.\bin\main.exe
```

Notes:
- Keep the working directory as the project root when launching.
- If the linker fails during build, close any running instance of main.exe and build again.

## How To Contribute

1. Fork the repository and create a feature branch.
2. Keep gameplay changes focused and easy to review.
3. Follow project naming style:
	- Constants: UPPER_SNAKE_CASE
	- Functions and mutable variables: camelCase
4. Build and test your changes locally before opening a pull request.
5. In your pull request, include:
	- What changed
	- Why it changed
	- How you tested it

Small, clear pull requests are preferred over very large ones.