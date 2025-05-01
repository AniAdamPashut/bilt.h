#pragma once

#ifdef BILT_IMPLEMENTATION
#define BASE_IMPLEMENTATION
#endif

#include "base/base.h"

typedef struct {
  i64 lastBuild;
  bool firstBuild;
} BiltCache;

typedef struct {
  char *compiler;
  char *buildDirectory;
  char *source;
  char *exe;
  char *cachePath;
} BiltOptions;

typedef struct {
  String compiler;

  // Paths
  String buildDirectory;
  String source;
  String exe;
  String cachePath;

  // Cache
  BiltCache cache;

  // Misc
  Arena arena;
  bool customConfig;
  i64 startTime;
  i64 totalTime;
} BiltConfig;

typedef struct {
  String output;
  String flags;
  String linkerFlags;
  // TODO: add target
  // TODO: optimize
  // TODO: warnings
  // TODO: debugSymbols
  String includes;
  String libs;
  StringVector sources;
} Executable;

typedef struct {
  char *output;
  char *flags;
  char *linkerFlags;
  char *includes;
  char *libs;
} ExecutableOptions;

void CreateConfig(BiltOptions options);
void StartBuild();
void reBuild();
void CreateExecutable(ExecutableOptions executableOptions);

#define AddLibraryPaths(...)                                                                                                                                                                                                                   \
  ({                                                                                                                                                                                                                                           \
    StringVector vector = {0};                                                                                                                                                                                                                 \
    StringVectorPushMany(vector, __VA_ARGS__);                                                                                                                                                                                                 \
    addLibraryPaths(&vector);                                                                                                                                                                                                                  \
  })
static void addLibraryPaths(StringVector *vector);

#define AddIncludePaths(...)                                                                                                                                                                                                                   \
  ({                                                                                                                                                                                                                                           \
    StringVector vector = {0};                                                                                                                                                                                                                 \
    StringVectorPushMany(vector, __VA_ARGS__);                                                                                                                                                                                                 \
    addIncludePaths(&vector);                                                                                                                                                                                                                  \
  })
static void addIncludePaths(StringVector *vector);

#define LinkSystemLibraries(...)                                                                                                                                                                                                               \
  ({                                                                                                                                                                                                                                           \
    StringVector vector = {0};                                                                                                                                                                                                                 \
    StringVectorPushMany(vector, __VA_ARGS__);                                                                                                                                                                                                 \
    linkSystemLibraries(&vector);                                                                                                                                                                                                              \
  })
static void linkSystemLibraries(StringVector *vector);

#define AddFile(source) addFile(S(source));
static void addFile(String source);
String InstallExecutable();
i32 RunCommand(String command);
void EndBuild();

static bool needRebuild();
static void setDefaultState();

#ifdef BILT_IMPLEMENTATION
static BiltConfig state = {0};
static Executable executable = {0};

String FixPathExe(String *str) {
  String path = ConvertPath(&state.arena, ConvertExe(&state.arena, *str));
  String cwd = GetCwd();
#if defined(PLATFORM_WIN)
  String formatted_path = F(&state.arena, "%s\\%s", cwd.data, path.data);
#elif defined(PLATFORM_LINUX)
  String formatted_path = F(&state.arena, "%s/%s", cwd.data, path.data);
#endif
  StrFree(cwd);
  return formatted_path;
}

String FixPath(String *str) {
  String path = ConvertPath(&state.arena, *str);
  String cwd = GetCwd();
#if defined(PLATFORM_WIN)
  String formatted = F(&state.arena, "%s\\%s", cwd.data, path.data);
#elif defined(PLATFORM_LINUX)
  String formatted = F(&state.arena, "%s/%s", cwd.data, path.data);
  StrFree(cwd);
  return formatted;
#endif
}

String ConvertNinjaPath(String str) {
#ifdef PLATFORM_WIN
  String copy = StrNewSize(&state.arena, str.data, str.length + 1);
  memmove(&copy.data[2], &copy.data[1], str.length - 1);
  copy.data[1] = '$';
  copy.data[2] = ':';
  return copy;
#else
  return str;
#endif
}

static void setDefaultState() {
  state.arena = ArenaInit(12000 * sizeof(String));
  state.source = FixPath(&S("./bilt.c"));
  state.cachePath = FixPath(&S("./build/bilt-cache.json"));
  state.exe = FixPath(&S("./bilt"));
  state.buildDirectory = ConvertPath(&state.arena, S("./build"));
  state.compiler = GetCompiler();
}

static BiltConfig parseBiltConfig(BiltOptions options) {
  BiltConfig result;
  result.compiler = StrNew(&state.arena, options.compiler);
  result.buildDirectory = StrNew(&state.arena, options.buildDirectory);
  result.exe = StrNew(&state.arena, options.exe);
  result.cachePath = StrNew(&state.arena, options.cachePath);
  result.source = StrNew(&state.arena, options.source);
  return result;
}

void CreateConfig(BiltOptions options) {
  setDefaultState();
  BiltConfig config = parseBiltConfig(options);

  if (!StrIsNull(&config.source)) {
    state.source = FixPath(&config.source);
  }

  if (!StrIsNull(&config.cachePath)) {
    state.cachePath = FixPath(&config.cachePath);
  }

  if (!StrIsNull(&config.exe)) {
    state.exe = FixPath(&config.exe);
  }

  if (!StrIsNull(&config.buildDirectory)) {
    state.buildDirectory = ConvertPath(&state.arena, config.buildDirectory);
  }

  if (!StrIsNull(&config.compiler)) {
    state.compiler = config.compiler;
  }

  state.customConfig = true;
}

errno_t readCache() {
  String cache = S("");
  errno_t err = FileRead(&state.arena, &state.cachePath, &cache);

  if (err == FILE_NOT_EXIST) {
    String modifyTime = F(&state.arena, "%llu", TimeNow() / 1000);
    FileWrite(&state.cachePath, &modifyTime);
    state.cache.firstBuild = true;
    cache = modifyTime;
  } else if (err != SUCCESS) {
    return err;
  }

  char *endptr;
  state.cache.lastBuild = strtoll(cache.data, &endptr, 10);
  return SUCCESS;
}

void StartBuild() {
  LogInit();
  if (!state.customConfig) {
    setDefaultState();
  }

  state.startTime = TimeNow();
  Mkdir(state.buildDirectory);
  readCache();
  reBuild();
}

static bool needRebuild() {
  File stats = {0};
  errno_t result = FileStats(&state.source, &stats);
  if (result != SUCCESS) {
    LogError("Could not read fileStats %d", result);
    return false;
  }

  if (stats.modifyTime > state.cache.lastBuild) {
    String modifyTime = F(&state.arena, "%llu", stats.modifyTime);
    FileWrite(&state.cachePath, &modifyTime);
    return true;
  }

  return false;
}

void reBuild() {
  if (state.cache.firstBuild || !needRebuild()) {
    return;
  }

  String exeNew = F(&state.arena, "%s/bilt-new", state.buildDirectory.data);
  String exeOld = F(&state.arena, "%s/bilt-old", state.buildDirectory.data);
  String exe = ConvertExe(&state.arena, state.exe);
  exeNew = FixPathExe(&exeNew);
  exeOld = FixPathExe(&exeOld);

  String compileCommand;
  if (StrEqual(&state.compiler, &S("gcc"))) {
    compileCommand = F(&state.arena, "gcc -o \"%s\" -Wall -g \"%s\"", exeNew.data, state.source.data);
  }

  if (StrEqual(&state.compiler, &S("clang"))) {
    compileCommand = F(&state.arena, "clang -o \"%s\" -Wall -g \"%s\"", exeNew.data, state.source.data);
  }

  if (StrEqual(&state.compiler, &S("MSVC"))) {
    compileCommand = F(&state.arena, "cl.exe /Fe:\"%s\" /W4 /Zi \"%s\"", exeNew.data, state.source.data);
  }

  LogWarn("%s changed rebuilding...", state.source.data);
  errno_t rebuildResult = RunCommand(compileCommand);
  if (rebuildResult != 0) {
    LogError("Rebuild failed, with error: %d", rebuildResult);
    abort();
  }

  errno_t renameResult = FileRename(&exe, &exeOld);
  if (renameResult != SUCCESS) {
    LogError("Error renaming original executable: %d", renameResult);
    abort();
  }

  renameResult = FileRename(&exeNew, &exe);
  if (renameResult != SUCCESS) {
    LogError("Error moving new executable into place: %d", renameResult);
    FileRename(&exeOld, &state.exe);
    abort();
  }

  LogInfo("Rebuild finished, running %s", exe.data);
  errno_t result = RunCommand(exe);
  return exit(result);
}

void defaultExecutable() {
  String executableOutput = ConvertExe(&state.arena, S("main"));
  executable.output = ConvertPath(&state.arena, executableOutput);
  executable.flags = S("-Wall -g");
  executable.linkerFlags = S("");
  executable.includes = S("");
  executable.libs = S("");
}

static Executable parseExecutableOptions(ExecutableOptions options) {
  Executable result;
  result.output = StrNew(&state.arena, options.output);
  result.flags = StrNew(&state.arena, options.flags);
  result.linkerFlags = StrNew(&state.arena, options.linkerFlags);
  result.includes = StrNew(&state.arena, options.includes);
  result.libs = StrNew(&state.arena, options.libs);
  return result;
}

void CreateExecutable(ExecutableOptions executableOptions) {
  defaultExecutable();
  Executable options = parseExecutableOptions(executableOptions);

  if (!StrIsNull(&options.output)) {
    String executableOutput = ConvertExe(&state.arena, options.output);
    executable.output = ConvertPath(&state.arena, executableOutput);
  }

  if (!StrIsNull(&options.flags)) {
    executable.flags = options.flags;
  }

  if (!StrIsNull(&options.linkerFlags)) {
    executable.linkerFlags = options.linkerFlags;
  }

  if (!StrIsNull(&options.includes)) {
    executable.includes = options.includes;
  }

  if (!StrIsNull(&options.libs)) {
    executable.libs = options.libs;
  }
}

// TODO: Implement for linux
// TODO: Add error enum
errno_t CreateCompileCommands() {
  FILE *ninjaPipe;
  FILE *outputFile;
  char buffer[4096];
  size_t bytes_read;
  
  String cwd = GetCwd();

  String buildPath = StrNew(&state.arena, F(&state.arena, "%s/%s", cwd.data, ParsePath(&state.arena, state.buildDirectory).data).data);
  String compileCommandsPath = ConvertPath(&state.arena, F(&state.arena, "%s/compile_commands.json", buildPath.data));

  StrFree(cwd);

  outputFile = fopen(compileCommandsPath.data, "w");
  if (outputFile == NULL) {
    LogError("Failed to open output file '%s'", compileCommandsPath.data);
    return 1;
  }

  String compdbCommand = ConvertPath(&state.arena, F(&state.arena, "ninja -f %s/build.ninja -t compdb", buildPath.data));

  ninjaPipe = popen(compdbCommand.data, "r");
  if (ninjaPipe == NULL) {
    LogError("Failed to run command");
    fclose(outputFile);
    return 1;
  }

  while ((bytes_read = fread(buffer, 1, sizeof(buffer), ninjaPipe)) > 0) {
    fwrite(buffer, 1, bytes_read, outputFile);
  }

  fclose(outputFile);
  i32 status = pclose(ninjaPipe);
  if (status != 0) {
    LogError("Command failed with status %d\n", status);
    return status;
  }

  LogSuccess("Successfully created %s\n", compileCommandsPath.data);
  return SUCCESS;
}

static void addFile(String source) {
  VecPush(executable.sources, source);
}

static StringVector outputTransformer(StringVector vector) {
  StringVector result = {0};
  for (size_t i = 0; i < vector.length; i++) {
    String *currentExecutable = VecAt(vector, i);
    String output = S("");
    for (size_t j = currentExecutable->length - 1; j > 0; j--) {
      String currentChar = StrNewSize(&state.arena, &currentExecutable->data[j], 1);
      if (currentChar.data[0] == '/') {
        break;
      }
      output = StrConcat(&state.arena, &currentChar, &output);
    }
    output.data[output.length - 1] = 'o';
    VecPush(result, output);
  }

  return result;
}

// TODO: Make linux version
String InstallExecutable() {
  if (executable.sources.length == 0) {
    LogError("Executable has zero sources, add at least one with AddFile(\"./main.c\")");
    abort();
  }

  String linkCommand;
  String compileCommand;
  if (StrEqual(&state.compiler, &S("gcc"))) {
    linkCommand = F(&state.arena, "rule link\n  command = $cc $flags $linker_flags -o $out $in $libs\n");
    compileCommand = F(&state.arena, "rule compile\n  command = $cc $flags $includes -c $in -o $out\n");
  }

  if (StrEqual(&state.compiler, &S("clang"))) {
    linkCommand = F(&state.arena, "rule link\n  command = $cc $flags $linker_flags -o $out $in $libs\n");
    compileCommand = F(&state.arena, "rule compile\n  command = $cc $flags $includes -c $in -o $out\n");
  }

  if (StrEqual(&state.compiler, &S("MSVC"))) {
    LogError("MSVC not yet implemented");
    abort();
  }
  String cwd = GetCwd();
  String ninjaOutput = F(&state.arena,
                         "cc = %s\n"
                         "linker_flag = %s\n"
                         "flags = %s\n"
                         "cwd = %s\n"
                         "builddir = $cwd/%s\n"
                         "target = $builddir/%s\n"
                         "includes = %s\n"
                         "libs = %s\n"
                         "\n"
                         "%s\n"
                         "%s\n",
                         state.compiler.data,
                         executable.linkerFlags.data,
                         executable.flags.data,
                         ConvertNinjaPath(StrNew(&state.arena, cwd.data)).data,
                         state.buildDirectory.data,
                         executable.output.data,
                         executable.includes.data,
                         executable.libs.data,
                         linkCommand.data,
                         compileCommand.data);
  StrFree(cwd);
  StringVector outputFiles = outputTransformer(executable.sources);

  assert(outputFiles.length == executable.sources.length && "Something went wrong in the parsing");

  String outputString = S("");
  for (size_t i = 0; i < executable.sources.length; i++) {
    String sourceFile = ParsePath(&state.arena, *VecAt(executable.sources, i));
    String outputFile = *VecAt(outputFiles, i);
    String source = F(&state.arena, "build $builddir/%s: compile $cwd/%s\n", outputFile.data, sourceFile.data);
    ninjaOutput = StrConcat(&state.arena, &ninjaOutput, &source);
    outputString = F(&state.arena, "%s $builddir/%s", outputString.data, outputFile.data);
  }

  String target = F(&state.arena,
                    "build $target: link%s\n"
                    "\n"
                    "default $target\n",
                    outputString.data);
  ninjaOutput = StrConcat(&state.arena, &ninjaOutput, &target);

  String relativeBuildPath = F(&state.arena, "%s/build.ninja", state.buildDirectory.data);
  String buildNinjaPath = FixPath(&relativeBuildPath);
  FileWrite(&buildNinjaPath, &ninjaOutput);

  
  errno_t result = RunCommand(F(&state.arena, "ninja -f %s", buildNinjaPath.data));
  if (result != 0) {
    LogError("Ninja file compilation failed with code: %d", result);
    abort();
  }
  
  LogSuccess("Ninja file compilation done");
  state.totalTime = TimeNow() - state.startTime;
  
  String relativeExePath = F(&state.arena, "%s/%s", state.buildDirectory.data, executable.output.data);
  String fullExePath = FixPath(&relativeExePath);
  return fullExePath;
}

errno_t RunCommand(String command) {
  return system(command.data);
}

// TODO: Allocate total string size instead of concat
// TODO: Do it depending on compiler
static void addLibraryPaths(StringVector *vector) {
  for (size_t i = 0; i < vector->length; i++) {
    String *currLib = VecAt((*vector), i);
    if (i == 0 && executable.libs.length == 0) {
      executable.libs = F(&state.arena, "-L\"%s\"", currLib->data);
      continue;
    }

    executable.libs = F(&state.arena, "%s -L\"%s\"", executable.libs.data, currLib->data);
  }
}

// TODO: Same thing here
static void addIncludePaths(StringVector *vector) {
  for (size_t i = 0; i < vector->length; i++) {
    String *currInclude = VecAt((*vector), i);
    if (i == 0 && executable.includes.length == 0) {
      executable.includes = F(&state.arena, "-I\"%s\"", currInclude->data);
      continue;
    }

    executable.includes = F(&state.arena, "%s -I\"%s\"", executable.includes.data, currInclude->data);
  }
}

static void linkSystemLibraries(StringVector *vector) {
  for (size_t i = 0; i < vector->length; i++) {
    String *currLib = VecAt((*vector), i);
    if (i == 0 && executable.libs.length == 0) {
      executable.libs = F(&state.arena, "-l%s", currLib->data);
      continue;
    }

    executable.libs = F(&state.arena, "%s -l%s", executable.libs.data, currLib->data);
  }
}

void EndBuild() {
  LogInfo("Build took: %llums", state.totalTime);
  ArenaFree(&state.arena);
}
#endif