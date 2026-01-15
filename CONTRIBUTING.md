# Contributing Guidelines

Please format code in accordance to the .clang-format file at the root of this repository.

## Naming Conventions

| Item                | Convention                         | Notes                                                                  |
|:--------------------|:-----------------------------------|:-----------------------------------------------------------------------|
| Namespaces          | `PascalCase`                       |                                                                        |
| Classes             | `PascalCase`                       |                                                                        |
| Macros              | `SCREAMING_SNAKE_CASE`             |                                                                        |
| Constants           | `SCREAMING_SNAKE_CASE`             |                                                                        |
| Local Variables     | `camelCase`                        |                                                                        |
| Function Parameters | `camelCase`                        |                                                                        |
| Private Methods     | `camelCase`                        |                                                                        |
| Public Methods      | `PascalCase`                       |                                                                        |
| Private Fields      | `_camelCase`                       |                                                                        |
| Public Fields       | `camelCase`                        |                                                                        |
| Enums               | `PascalCase`                       |                                                                        |
| Enum Members        | `{enum name}_SCREAMING_SNAKE_CASE` | The enum name is written verbatim. For example: `EnumName_SOME_MEMBER` |
| Header Guards       | `NAMESPACE_CLASS_H`                | e.g. `Marmalade::GUI::Window` would be `MARMALADE_GUI_WINDOW_H`        |

## Ordering

Fields before methods.

```c++
class SomeClass {
    public:
        Type fieldA;
        
        SomeClass();
        ~SomeClass();
        
        void SomeMethod();
    private:
        Type _fieldB;
        
        void privateMethod();
};
```

## Project Structure

The project uses the following directories:

- `.github`: GitHub CI workflows.
- `.idea`: Shared IntelliJ settings.
- `cmake`: CMake extensions.
- `include`: Marmalade Engine API headers. These must be C headers.
- `misc`: Miscellaneous (docs, compilation resources).
- `packages`: Built-in Marmalade Engine packages.
- `patches`: Patches for vendored submodules.
- `plugins`: Built-in Marmalade Engine plugins.
- `po`: Localisation files.
- `res`: Runtime resources. This whole directory will be copied to build directory.
- `src`: Marmalade Engine sources. Each directory is typically its own subsystem:
  - `animation`: Animations.
  - `application`: Core application code, e.g. logging, config, init, utilities
  - `audio`: Marmalade Engine audio subsystem.
  - `ecs`: Entity component system.
  - `graphics`: Marmalade Engine graphics subsystem.
  - `gui`: Marmalade Engine GUI.
  - `io`: [subject to change] Input code.
  - `mathematics`: Marmalade Engine mathematics library (MEGA branch).
  - `packages`: Package manager
  - `physics`
  - `project`: Project management
  - `scene`: Scene management
- `toolchains`: CMake toolchain definitions.
- `tools`: Various development utilities for Marmalade Engine.
- `vendor`: Vendored submodules.

## Commit Messages

Commit messages should use the following format:

```
<type>(<scope>): <summary>

[description]
```

The scope is typically the directory or system affected in the commit.

### Commit Types

| Type       | Description                                                                |
|------------|----------------------------------------------------------------------------|
| `feat`     | A new feature, or modification to an existing feature                      |
| `fix`      | A bug fix                                                                  |
| `refactor` | Code refactoring that doesn’t change behavior (e.g., structure or cleanup) |
| `chore`    | Non-code changes (e.g., updating README, versioning, or metadata)          |
| `docs`     | Documentation-only changes                                                 |
| `style`    | Code style or formatting changes (no logic changes)                        |
| `test`     | Adding or updating tests                                                   |
| `perf`     | Performance improvements                                                   |
| `build`    | Changes to build scripts or dependencies                                   |

# Localisation

Any string displayed in the UI must be translatable. Make sure to include the `application/i18n.h` header.

Simply remember to use wrap the string in `_( )`, e.g. `_("My string")`.

Plurals should use the `ngettext` function. For example:

```c++
/* xgettext: range: 1..6 */
ngettext("An item", "%d items", n); // n is the number of items
```

xgettext comments can be useful to display information to translators.

If a translation has multiple contents, like in menu bars, use the `pgettext` function, for example:

```c++
pgettext("File|", "Print")
```

Include the trailing `|` if the context is a menu.

Finally, you can include messages for translators at any time, by appending `Translators: ` to be the beginning of your
comment:

```c++
/* Translators: This is the verb, not the noun */
_("Shop")
```

POT files can be generated with the `generate-pot.sh` script.
