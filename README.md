# Marmalade

![Nightly dev build Windows](https://img.shields.io/endpoint?url=https://www.ryanbester.com/badge.php?url=http%3A%2F%2Fteamcity.diesbackwards.com%2Fapp%2Frest%2Fbuilds%2FbuildType%3Aid%3AMarmalade_NightlyDevWindows%2Cbranch%3Aname%3Adev%2FstatusIcon.svg&label=Nightly%20%28dev%29%20Windows)
![Nightly dev build Linux](https://img.shields.io/endpoint?url=https://www.ryanbester.com/badge.php?url=http%3A%2F%2Fteamcity.diesbackwards.com%2Fapp%2Frest%2Fbuilds%2FbuildType%3Aid%3AMarmalade_NightlyDevLinux%2Cbranch%3Aname%3Adev%2FstatusIcon.svg&label=Nightly%20%28dev%29%20Linux)

<img src="res/icons/logo/logo192.png" alt="Marmalade Logo" height="128">

Marmalade is a modular 2D game engine.

## Features

- Modular design
  - Everything is a package
  - No unnecessary code or dependencies
  - Keeps packaged games as small as possible
- Code generation system
  - Does not rely on redistributable libraries
  - Reduces download size for games
- Entity Component System (ECS)
- 2D Lighting
- Animation
- Version Control

## Building

This repository in itself, without building any packages, is essentially just an engine UI and API.

Multiple built-in packages and plugins are contained in `packages` and `plugins` directories respectively.

1. Clone the repository
2. Run `python submodules.py restore`
3. Build with CMake

## Acknowledgements

This project is made possible thanks to the generous support of:

|   |
|:-:|
| [<img src="misc/readme-res/sentry.svg" alt="Sentry" height="64">](https://sentry.io) |
