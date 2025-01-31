# Marmalade

Marmalade is a modular 2D game engine.

## Features

- Modular design
  - Everything is a package
  - No unnecessary code or dependencies
  - Keeps packaged games as small as possible

## Building

This repository in itself, without building any packages, is essentially just an engine UI and API.

Multiple built-in packages and plugins are contained in `packages` and `plugins` directories respectively.

1. Clone the repository
2. Run `git submodule update --init`
3. Build with CMake
