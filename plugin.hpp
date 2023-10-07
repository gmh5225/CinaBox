#pragma once

#include "handle.hpp"

namespace pluginsplusplus {

	template<std::derived_from<pluginsplusplus::plugin_base> PluginBase = pluginsplusplus::plugin_base>
	struct CPluginLoader: public pluginsplusplus::plugin_host_plugin_base<PluginBase> {
        romfs::tempary_filesystem tmp;
    
		void start() override {
			// Unpack the tcc libraries that are packed into the binary
            CPluginHandle<PluginBase>::archive = tmp.materialize_file("libtcc1.a");
            CPluginHandle<PluginBase>::header = tmp.materialize_file("libtcc.h");
			// Unpack musl libc that is packed into the binary
			CPluginHandle<PluginBase>::libc = tmp.materialize_file("musl_libc.a");
			tmp.materialize_all_files();
			CPluginHandle<PluginBase>::libc_include = tmp.base / "musl" / "include";

			// CPluginHandle<PluginBase>::register_loaders();
		}

		static CPluginLoader* create() { return new CPluginLoader; }
	};

}