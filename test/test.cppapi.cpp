#include "cinabox.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
	if(argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <file to compile>" << std::endl;
		return -1;
	}

	// Create an interpreter (C in a Box is setup automatically)
	tcc::Interpreter i;
	// Load a C file into the interpreter
	i.add_hashbang_file(argv[1]);

	// NOTE: If desired manual relocation is possible: i.relocate(...);
	// If you do so, pass false to the following calls to prevent automatic relocation: ex i.get_symbol<void()>("load", false);

	// Extract and run a compiled symbol!
	auto load = i.get_symbol<void()>("load");
	// or auto load = (void(*)())i.get_symbol("load");
	if(load) load();



	// Setting up an interpreter instance isn't even nessicary, you can simply run a string!
	// It even supports passing arguments and receiving a returned result!
	tcc::Interpreter::run(
		"#include <stdio.h>\n"
		"int main() {\n"
		"	printf(\"hello world\\n\");\n"
		"	return 0;\n"
		"}"
	);
}