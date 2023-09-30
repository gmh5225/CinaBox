#define PPP_C_NO_CUSTOM_SYMBOLS
#define PPP_C_NO_CUSTOM_OPTIONS
#include "plugin.hpp"

namespace ppp = pluginsplusplus;

int main(int argc, char** argv) {
	auto loader = ppp::CPluginLoader<ppp::plugin_base>::create();
	loader->start();

	if(argc != 2) {
		std::cout << "Usage: " << argv[0] << " <script path>" << std::endl;
		return -1;
	}

	auto plugin = ppp::CPluginHandle<ppp::plugin_base>::load(argv[1]);
	while(plugin.step()) {
		// std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}

// #include <cstdlib>
// #include <cstdio>
// #include <libtcc.h>
// #include <iostream>
// #include <string_view>
// #include <vector>
// #include <cstdint>

// #include "defer.hpp"
// #include "includeable.hpp"

// int add(int a, int b) { return a + b; }

// char my_program[] =
// "#include <libtcc.h>\n"
// "int fib(int n) {\n"
// "	if (n <= 2) return 1;\n"
// "	else return fib(n-1) + fib(n-2);\n"
// "}\n"
// "int foobar(int n) {\n"
// "	printf(\"fib(%d) = %d\\n\", n, fib(n));\n"
// "	printf(\"add(%d, %d) = %d\\n\", n, 2 * n, add(n, 2 * n));\n"
// "	return 1337;\n"
// "}\n";



// int main(int argc, char **argv) {
// 	includeable::tempary_filesystem tmp;
// 	auto archive = tmp.materialize_binary_file("libtcc1.a",
// 		#include "tinycc/libtcc1.a.hpp"
// 	);
// 	auto header = tmp.materialize_file("libtcc.h",
// 		#include "tinycc/libtcc.h.hpp"
// 	);

// 	auto s = tcc_new();
// 	defer { tcc_delete(s); };
// 	tcc_set_lib_path(s, archive.parent_path().c_str());
// 	tcc_add_include_path(s, header.parent_path().c_str());
// 	tcc_set_output_type(s, TCC_OUTPUT_MEMORY);
// 	tcc_compile_string(s, my_program);
// 	tcc_add_symbol(s, "add", (const void*)add);

// 	auto mem = malloc(tcc_relocate(s, nullptr));
// 	defer { free(mem); };
// 	tcc_relocate(s, mem);

// 	auto foobar_func = (int (*)(int))tcc_get_symbol(s, "foobar");

// 	auto ret = foobar_func(32);
// 	std::cout << "foobar returned: " << ret << std::endl;
// }