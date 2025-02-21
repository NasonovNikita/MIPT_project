# Naming Conventions

### Directories and Files
- `camelCase`

### Functions
- `camelCase`

### Classes, Structs, Enums, and Other Types
- `PascalCase`
    - Member Variables: `camelCase`
    - Static Variables: `s_camelCase`

### Global Variables
- `g_camelCase`

### Constants
- `ALL_CAPS_WITH_UNDERSCORES`

### Local Variables and Function Parameters
- `camelCase`

### Enum Values
- `ALL_CAPS_WITH_UNDERSCORES`

### Namespaces
- `snake_case`

# Namespace Rules

- Namespaces must be named after their folders and nested accordingly.
- Use **inline namespace nesting**.
- Use namespace block for implementing, instead of inline namespace qualifying for each implementation.

# Header File Rules

- Put header files in `include` to a folder with the same name as the source file in `src`.
- If the implementations are small (1-3 lines) and easy, keep them in header file.
- Complex and big functions must be implemented in .cpp as usual.