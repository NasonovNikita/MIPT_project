NAMING:

directory, file - camelCase<br>
functions - camelCase<br>
class, struct, enum, other types - PascalCase<br>
    member variables - m_camelCase<br>
    static variables - s_camelCase<br>
global variables - g_camelcase<br>
constants - ALL_CAPS_WITH_UNDERSCORES<br>
local variables, function parameters - camelCase<br>
enum values - ALL_CAPS_WITH_UNDERSCORES<br>
namespace - snake_case

<br>
NAMESPACES:

Namespaces must be named after their folders and nested accordingly.<br>
Use **inline namespace nesting**.<br>
Use namespace block for implementing, instead of inline namespace qualifying
for each implementation

<br>
HEADER FILES (.h/.hpp):

Put them in *include* to a folder with the same name as the source file in *src*
