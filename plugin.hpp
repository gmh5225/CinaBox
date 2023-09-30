#pragma once

#include "handle.hpp"

namespace pluginsplusplus {

	template<std::derived_from<pluginsplusplus::plugin_base> PluginBase = pluginsplusplus::plugin_base>
	struct CPluginLoader: public pluginsplusplus::plugin_host_plugin_base<PluginBase> {
        includeable::tempary_filesystem tmp;
    
		void start() override {
			// Unpack the tcc libraries that are packed into the binary
            CPluginHandle<PluginBase>::archive = tmp.materialize_binary_file("libtcc1.a",
                #include "tinycc/libtcc1.a.hpp"
            );
            CPluginHandle<PluginBase>::header = tmp.materialize_file("libtcc.h",
                #include "tinycc/libtcc.h.hpp"
            );

			// CPluginHandle<PluginBase>::register_loaders();
		}

		static CPluginLoader* create() { return new CPluginLoader; }
	};

}