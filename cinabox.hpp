#ifndef __95e6ad99ac85a8011d8841ff70c3ed96_C_IN_A_BOX_HPP__
#define __95e6ad99ac85a8011d8841ff70c3ed96_C_IN_A_BOX_HPP__

#include <filesystem>
#include <vector>
#include <map>
#include <optional>
#include <utility>

#include "cinabox.h"

namespace tcc {

    enum class OutputType {
        INVALID = 0,
        Memory = TCC_OUTPUT_MEMORY, /* output will be run in memory */
        EXE = TCC_OUTPUT_EXE, /* executable file */
        DLL = TCC_OUTPUT_DLL, /* dynamic library */
        OBJ = TCC_OUTPUT_OBJ, /* object file */
        PreprocessOnly = TCC_OUTPUT_PREPROCESS /* only preprocess */
    };

    enum class UnpackType {
        None = 0,
        Compiler = 1,
        Libc = 2,
        LibcHeaders = 3,
    };

    // Compiler capable of creating executables and, shared and static objects
    struct Compiler {
        TCCState* state;

        Compiler(OutputType output_type = OutputType::EXE, UnpackType unpack_type = UnpackType::LibcHeaders) {
            state = tcc_new();
            tcc_set_output_type(state, (int)output_type);
            switch(unpack_type) {
            break; case UnpackType::LibcHeaders: tcc_setup_c_in_a_box(state);
            break; case UnpackType::Libc: tcc_setup_c_in_a_box_no_includes(state);
            break; case UnpackType::Compiler: tcc_setup_c_in_a_box_no_libc(state);
            break; default: { /* No C in a Box setup! */ }
            }
        }
        Compiler(const Compiler&) = delete;
        Compiler(Compiler&&) = default;
        Compiler& operator=(const Compiler&) = delete;
        Compiler& operator=(Compiler&&) = default;

        /* free a TCC compilation context */
        Compiler& free() { tcc_delete(state); state = nullptr; return *this; }

        /* set CONFIG_TCCDIR at runtime */
        Compiler& set_lib_path(std::filesystem::path path) { tcc_set_lib_path(state, path.c_str()); return *this; }

        /* set error/warning display callback */
        Compiler& set_error_func(void *error_opaque, TCCErrorFunc error_func) { tcc_set_error_func(state, error_opaque, error_func); return *this; }

        /* return error/warning callback */
        TCCErrorFunc get_error_func() { return tcc_get_error_func(state); }

        /* return error/warning callback opaque pointer */
        void* get_error_opaque() { return tcc_get_error_opaque(state); }

        /* set options as from command line (multiple supported) */
        bool set_options(std::string_view str) { return tcc_set_options(state, str.data()); }

        /* add include path */
        bool add_include_path(std::filesystem::path pathname) { return tcc_add_include_path(state, pathname.c_str()); }

        /* add in system include path */
        bool add_sysinclude_path(std::filesystem::path pathname) { return tcc_add_sysinclude_path(state, pathname.c_str()); }

        /* define preprocessor symbol 'sym'. value can be NULL, sym can be "sym=val" */
        Compiler& define_symbol(std::string_view sym, std::string_view value) { tcc_define_symbol(state, sym.data(), value.data()); return *this; }

        /* undefine preprocess symbol 'sym' */
        Compiler& undefine_symbol(std::string_view sym) { tcc_undefine_symbol(state, sym.data()); return *this; }

        /* add a file (C file, dll, object, library, ld script). Return -1 if error. */
        bool add_file(std::filesystem::path filename) { return tcc_add_file(state, filename.c_str()); }

        /* add a C file, if there is a hashbang in the first line any command line options following it are applied */
        bool add_hashbang_file(std::filesystem::path filename) { return tcc_add_hashbang_file(state, filename.c_str()); }

        /* compile a string containing a C source. Return -1 if error. */
        bool compile_string(std::string_view source) { return tcc_compile_string(state, source.data()); }

        /* compile a string containing a C source. If there is a hashbang in the first line any command line options following it are applied */
        bool compile_hashbang_string(std::string_view source) { return tcc_compile_hashbang_string(state, source.data()); }

        /* set output type. MUST BE CALLED before any compilation */
        bool override_output_type(OutputType output_type) { return tcc_set_output_type(state, (int)output_type); }

        /* equivalent to -Lpath option */
        bool add_library_path(std::filesystem::path pathname) { return tcc_add_library_path(state, pathname.c_str()); }

        /* the library name is the same as the argument of the '-l' option */
        bool add_library(std::string_view libraryname) { return tcc_add_library(state, libraryname.data()); }

        /* add a symbol to the compiled program */
        bool add_symbol(std::string_view name, const void *val) { return tcc_add_symbol(state, name.data(), val); }

        /* output an executable, library or object file. DO NOT call relocate() before. */
        bool output_file(std::string_view filename) { return tcc_output_file(state, filename.data()); }

        /* link and run main() function and return its value. DO NOT call relocate() before. */
        bool run(int argc, char **argv) { return tcc_run(state, argc, argv); }
    };

    // RAII Compiler that compiles into memory, allowing pointers to the compiled objects to be fetched
    class Interpreter: public Compiler {
        // Redefine these functions in the private section so that users will have to go out of their way to use them!
        int output_file(std::string_view filename) { return -1; }
        int run(int argc, char **argv) { return -1; }

        std::vector<std::byte> relocation;
    public:
        Interpreter(UnpackType unpack_type = UnpackType::LibcHeaders) : Compiler(OutputType::Memory, unpack_type) {}
        Interpreter(const Interpreter&) = delete;
        Interpreter(Interpreter&& o) { *this = std::move(o); }
        Interpreter& operator=(const Interpreter&) = delete;
        Interpreter& operator=(Interpreter&& o) {
            state = std::exchange(o.state, nullptr);
            relocation = std::move(o.relocation);
            return *this;
        }
        // Automatically free the state when the interpretor is no longer used
        ~Interpreter() { if(state) free(); }

        /* do all relocations (needed before using get_symbol()) */
        /* possible values for 'ptr':
        - TCC_RELOCATE_AUTO : Allocate and manage memory internally
        - TCC_NULL              : return required memory size for the step below
        - TCC_memory address    : copy code to memory passed by the caller
        returns -1 if error. */
        int relocate(void *ptr) { return tcc_relocate(state, ptr); }

        /* do all relocations (needed before using get_symbol()) into an automatically sized buffer*/
        std::vector<std::byte> relocate() {
            std::vector<std::byte> out;
            out.resize(relocate(nullptr));
            relocate(out.data());
            return out;
        }

        /* return symbol value or NULL if not found */
        template<typename F = void>
        F* get_symbol(std::string_view name, bool autoRelocate = true) {
            if(autoRelocate && relocation.empty()) relocation = relocate();
            return (F*)tcc_get_symbol(state, name.data());
        }

        /* calls the provided callback function once for every symbol providing a user pointer, the symbol name, and the associated function pointer */
        Interpreter& list_symbols(void *ctx, void (*symbol_cb)(void *ctx, const char* name, const void *val), bool autoRelocate = true) {
            if(autoRelocate && relocation.empty()) relocation = relocate();
            tcc_list_symbols(state, ctx, symbol_cb);
            return *this;
        }

        /* returns a map from symbol names to their associated function pointers */
        std::map<std::string, const void*> list_symbols(bool autoRelocate = true) {
            if(autoRelocate && relocation.empty()) relocation = relocate();

            std::map<std::string, const void*> out;
            list_symbols(&out, [](void *ctx, const char* name, const void *val){
                auto& out = *static_cast<std::map<std::string, const void*>*>(ctx);
                out[name] = val;
            });
            return out;
        }


        /* Creates an interpreter instance which compiles the given source, using the provided options and executes the function with a given name and args*/
        template<typename Ret = int, typename... Args>
        static std::optional<Ret> run_with_options(std::string_view source, std::string_view name, std::string_view options, bool useHashbang, Args... args) {
            tcc::Interpreter i;
            if(!options.empty()) i.set_options(options.data());
            if(useHashbang) {
	            if(i.compile_hashbang_string(source.data())) return {};
            } else if(i.compile_string(source.data())) return {};

            auto f = i.get_symbol<Ret(Args...)>(name.data());
            if(!f) return {};
            return f(std::forward<Args>(args)...);
        }

        /* Creates an interpreter instance which compiles the given source and executes the function with a given name and args, supports hashbang options*/
        template<typename Ret = int, typename... Args>
        static std::optional<Ret> run(std::string_view source, std::string_view name = "main", Args... args) {
            return run_with_options<Ret, Args...>(source, name, "", true, std::forward<Args>(args)...);
        }
    };
}

#endif // __95e6ad99ac85a8011d8841ff70c3ed96_C_IN_A_BOX_HPP__