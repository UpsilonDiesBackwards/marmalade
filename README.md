# Marmalade

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
