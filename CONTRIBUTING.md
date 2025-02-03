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

Templates should use `class` instead of `typename`.

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
