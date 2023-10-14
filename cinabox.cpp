#include "libs/romfs_materialize.hpp"

#include "cinabox.h"

// Temporary filesystem used for materializing packed files
namespace cib { romfs::tempary_filesystem tmp; }

void tcc_setup_c_in_a_box_no_libc(TCCState *state) {
    // Unpack the tcc libraries that are packed into the binary
    static auto archive = cib::tmp.materialize_file("libtcc1.a");
    static auto header = cib::tmp.materialize_file("libtcc.h");

    // Link to/include libtcc1
    tcc_set_lib_path(state, archive.parent_path().c_str());
    tcc_add_include_path(state, header.parent_path().c_str());
}

void tcc_setup_c_in_a_box_no_includes(TCCState *state) {
    tcc_setup_c_in_a_box_no_libc(state);

    // Unpack musl libc that is packed into the binary
    static auto libc = cib::tmp.materialize_file("musl_libc.a");
    
    // Link to musl libc
    tcc_add_library_path(state, libc.parent_path().c_str());
    tcc_add_library(state, libc.filename().c_str());
}

void tcc_setup_c_in_a_box(TCCState *state) {
    tcc_setup_c_in_a_box_no_includes(state);

    // Unpack musl libc headers that are packed into the binary
    static auto libc_include = [] {
        cib::tmp.materialize_all_files();
        return cib::tmp.base / "musl" / "include";
    }();

    // Include musl libc
    tcc_add_include_path(state, libc_include.c_str());
}

int tcc_compile_hashbang_string(TCCState *state, const char *str) {
    // Allow the file to customize its settings
    if(str[0] == '#' && str[1] == '!'){
        std::string source = str;
        std::string firstLine = std::string(str + 2, str + source.find("\n"));
        if(auto ret = tcc_set_options(state, firstLine.c_str()); ret != 0)
            return ret;
    }

    // Compile the string now that its settings have been customized!
    return tcc_compile_string(state, str);
}

LIBTCCAPI int tcc_add_hashbang_file(TCCState *state, const char *filepath) {
    std::ifstream fin(filepath);
    if(!fin) return -2;

    std::string buffer;
    fin.seekg(0, std::ios::end);
    buffer.resize(fin.tellg());
    fin.seekg(0, std::ios::beg);
    
    fin.read(buffer.data(), buffer.size());

    return tcc_compile_hashbang_string(state, buffer.c_str());
}

