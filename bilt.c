#define MATE_IMPLEMENTATION
#include "bilt.h"

i32 main() {
  StartBuild();
  {
    // Sets the output name and different flags
    CreateExecutable((ExecutableOptions){.output = "main", .flags = "-Wall -ggdb"});

    // Files to compile
    AddFile("./src/main.c");

    // Compiles all files parallely with ninja
    String exePath = InstallExecutable();

    // Runs `./build/main.exe` or whatever your main file is
    RunCommand(exePath);

    // Creates a `compile_commands.json`
    CreateCompileCommands();
  }
  EndBuild();
}