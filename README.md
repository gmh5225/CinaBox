# C in a Box

C in a box provides a small set of extra utility functions for [libTCC](https://github.com/TinyCC/tinycc) that expand a version of [MUSL LibC](https://musl.libc.org/) packed inside its binaries and automatically set the [libTCC](https://github.com/TinyCC/tinycc) paths to use the unpacked results:

```cpp
#include "cinabox.h"

// Create a new tcc instance
TCCState* state = tcc_new();
// Point that instance at the standard library embedded in the binary
tcc_setup_c_in_a_box(state); // ALL the magic happens here!
// Run in "interpretor" mode (compile into memory)
tcc_set_output_type(state, TCC_OUTPUT_MEMORY);
// Load a C file into the interpretor
tcc_add_hashbang_file(state, "path/to/file.c");

// Take ownership of the compiled code
void* memory = malloc(tcc_relocate(state, nullptr));
tcc_relocate(state, memory);

// Extract and run a compiled symbol!
void (*f)() = (void (*)())tcc_get_symbol(state, "function");
if(f) f();

// Clean up after ourselves...
tcc_delete(state);
free(memory);
```

## C++ Wrapper

Additionally, C in a Box provides a thin C++ wrapper around libTCC which by default will automatically setup the C in a Box paths!

```cpp
#include "cinabox.hpp"

// Create an interpreter (C in a Box is setup automatically)
tcc::Interpreter i; // or tcc::Compiler c; ... c.output_file(...);
// Load a C file into the interpreter
i.add_hashbang_file(argv[1]);

// NOTE: If desired manual relocation is possible: i.relocate(...);
// If you do so, pass false to the following calls to prevent automatic relocation: ex i.get_symbol<void()>("load", false);

// Extract and run a compiled symbol!
auto f = i.get_symbol<void()>("function");
// or: auto f = (void(*)())i.get_symbol("function");
if(f) f();
```

### tcc::Interpreter::run

The C++ wrapper also provides the ability to complie code, run it, pass arguments, and get results using a single function:

```cpp
tcc::Interpreter::run(
    "#include <stdio.h>\n"
    "int main() {\n"
    "	printf(\"hello world\\n\");\n"
    "	return 0;\n"
    "}"
); // By default a function called "main" is invoked
```

## Building C in a Box

Make sure to recursively clone C in a Box (or update your git submodules after the fact). Just add C in a Box as a subdirectory in your cmake build, and add the `cinabox` libary as one of your target link libraries!

The full build process is a tiny bit more complicated than usual:

```bash
    mkdir build && cd build
    cmake .. -DFIRST_BUILD=true # -G Ninja works fine
    make # Or Ninja, however TCC and MUSL will be built using make

    # The first time you build the project, some of the header files which need to be copied into the binary may not be created yet
    rm -rf ../libs/romfs # Everything in this folder is copied into the binary
    cmake .. -DFIRST_BUILD=false
    make # Second time around everything should be good to go!
```