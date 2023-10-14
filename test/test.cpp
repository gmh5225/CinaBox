#include "cinabox.h"
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
	if(argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <file to compile>" << std::endl;
		return -1;
	}

	auto state = tcc_new();
	tcc_setup_c_in_a_box(state);
	tcc_set_output_type(state, TCC_OUTPUT_MEMORY);
	tcc_add_hashbang_file(state, argv[1]);

	auto memory = malloc(tcc_relocate(state, nullptr));
	tcc_relocate(state, memory);

	auto load = (void (*)())tcc_get_symbol(state, "load");
	if(load) load();
}