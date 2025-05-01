> [!WARNING]
> This is still very experimental

# About:
This was forked from [this repo]( https://github.com/TomasBorquez/mate.h). I liked what I saw and realized I wanted to take it to another direction working on this in my free time. Go sub his yt channel, he got lotta good stuff to say.

```c 
#define MATE_IMPLEMENTATION
#include "mate.h"

i32 main() {
  StartBuild();
  {
    // Sets the output name and different flags
    CreateExecutable((ExecutableOptions){.output = "main", .flags = "-Wall -ggdb"});

    // Files to compile
    AddFile("./src/main.c");

    // Libraries and includes
    AddIncludePaths("C:/raylib/include", "./src");
    AddLibraryPaths("C:/raylib/lib");
    LinkSystemLibraries("raylib", "opengl32", "gdi32", "winmm");

    // Compiles all files parallely with ninja
    String exePath = InstallExecutable();

    // Runs `./build/main.exe` or whatever your main file is
    RunCommand(exePath);

    // Creates a `compile_commands.json`
    CreateCompileCommands();
  }
  EndBuild();
}
```

love yourz