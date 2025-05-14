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

Finally, you can include messages for translators at any time, by appending `Translators: ` to be the beginning of your comment:

```c++
/* Translators: This is the verb, not the noun */
_("Shop")
```

POT files can be generated with the `generate-pot.sh` script.
