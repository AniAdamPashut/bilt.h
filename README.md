# Bilt
> fastly built

## What is this

This was forked from [this repo]( https://github.com/TomasBorquez/mate.h). I liked what I saw and realized I wanted to take it to another direction working on this in my free time. This creates ninja files under the hood and uses them to build.

Just experimenting with my own ideas

Tested on gcc (both windows and linux). Feel free to open an issue for whatever thing you feel you need.

# How to compile this

You can use the following command to compile it to generate an header file to your platform. 

```sh
gcc -E -P -fdirectives-only bilt.h -DBILT_IMPLEMENTATION > bilt.h   
```

# How to use it

```c 
#define BILT_IMPLEMENTATION
#include "bilt.h"

i32 main() {
  StartBuild();
  {
    // Sets the output name and different flags
    CreateExecutable((ExecutableOptions){.output = "main", .flags = "-Wall -ggdb"});

    // Adds file to compile
    AddFile("./src/main.c");

    AllowFileExtensions("c", "cc");

    // Recursively adds the whole directory
    AddDirectory("./src");

    // Compiles all files parallely with ninja
    String exePath = InstallExecutable();

    // Libraries and includes
    AddIncludePaths("C:/raylib/include", "./src");
    AddLibraryPaths("C:/raylib/lib");
    LinkSystemLibraries("raylib", "opengl32", "gdi32", "winmm");

    // Runs `./build/main.exe` or whatever your main file is
    RunCommand(exePath);

    // Creates a `compile_commands.json`
    CreateCompileCommands();
  }
  EndBuild();
}
```

To run the included minimal example just 

```sh
gcc ./bilt.c -o bilt && ./bilt
```

(if you use linux put it in your `.*rc` file)



love yourz