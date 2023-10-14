#include "cinabox.h"
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
	if(argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <file to compile>" << std::endl;
		return -1;
	}

	// Create a new tcc instance
	TCCState* state = tcc_new();
	// Point that instance at the standard library embedded in the binary
	tcc_setup_c_in_a_box(state);
	// Run in "interpretor" mode (compile into memory)
	tcc_set_output_type(state, TCC_OUTPUT_MEMORY);
	// Load a C file into the interpretor
	tcc_add_hashbang_file(state, argv[1]);

	// Take ownership of the compiled code
	void* memory = malloc(tcc_relocate(state, nullptr));
	tcc_relocate(state, memory);

	// Extract and run a compiled symbol!
	void (*load)() = (void (*)())tcc_get_symbol(state, "load");
	if(load) load();

	// Clean up after ourselves...
	tcc_delete(state);
	free(memory);
}