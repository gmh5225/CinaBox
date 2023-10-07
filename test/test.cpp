#define PPP_C_NO_CUSTOM_SYMBOLS
#define PPP_C_NO_CUSTOM_OPTIONS
#include "../plugin.hpp"

namespace ppp = pluginsplusplus;

int main(int argc, char** argv) {
	auto loader = ppp::CPluginLoader<ppp::plugin_base>::create();
	loader->start();

	if(argc != 2) {
		std::cout << "Usage: " << argv[0] << " <script path>" << std::endl;
		return -1;
	}

	auto plugin = ppp::CPluginHandle<ppp::plugin_base>::load(argv[1]);
	while(plugin.step()) {}
}