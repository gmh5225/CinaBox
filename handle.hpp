#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <libtcc.h>
#include <string_view>
#include <thread>
#include <vector>

#include "../../plugin_handle.hpp"

#include "libs/includeable.hpp"

bool stop_requested(std::stop_token* stop) { return stop->stop_requested(); }
bool stop_possible(std::stop_token* stop) { return stop->stop_possible(); }

namespace pluginsplusplus {

    void register_custom_c_symbols(TCCState* state)
    #ifdef PPP_C_NO_CUSTOM_SYMBOLS
    {}
    #else
    ;
    #endif

    void register_custom_c_options(TCCState* state)
    #ifdef PPP_C_NO_CUSTOM_OPTIONS
    {}
    #else
    ;
    #endif

	template<std::derived_from<plugin_base> PluginBase = plugin_base>
	struct CPluginHandle: public PluginHandleBase<PluginBase> {
        static std::filesystem::path header;
        static std::filesystem::path archive;

        bool started = false;
		TCCState* state = nullptr;
        void* memory = nullptr;
        std::jthread thread;
        
		CPluginHandle(PluginBase** plugin = nullptr)
			: PluginHandleBase<PluginBase>(PluginHandleBase<PluginBase>::template GetTypeID<CPluginHandle>(), plugin) {}
		virtual ~CPluginHandle() {
			if (state) tcc_delete(state);
            if (memory) free(memory);
		}
		CPluginHandle(const CPluginHandle&) = delete;
		CPluginHandle(CPluginHandle&& o) { *this = std::move(o); }
		CPluginHandle& operator=(const CPluginHandle&) = delete;
		CPluginHandle& operator=(CPluginHandle&& o) {
			this->type = o.type;
			this->plugin = o.plugin;
			state = std::exchange(o.state, nullptr);
            memory = std::exchange(o.memory, nullptr);
            started = o.started;
            thread = std::move(o.thread);
            return *this;
		}

        static CPluginHandle load_memory(std::string source) {
            CPluginHandle out;
            out.state = tcc_new();
            tcc_set_lib_path(out.state, archive.parent_path().c_str());
            tcc_add_include_path(out.state, header.parent_path().c_str());
            // TODO: Need to also provide paths to the system's c standard library!
            register_custom_c_options(out.state);
            tcc_set_output_type(out.state, TCC_OUTPUT_MEMORY);
            tcc_compile_string(out.state, source.c_str());

            tcc_add_symbol(out.state, "stop_requested", (const void*)stop_requested);
            tcc_add_symbol(out.state, "stop_possible", (const void*)stop_possible);
            register_custom_c_symbols(out.state);

            out.memory = malloc(tcc_relocate(out.state, nullptr));
            tcc_relocate(out.state, out.memory);

            auto load = (void (*)())tcc_get_symbol(out.state, "load");
            if(load) load();
			
            return out;
        }

		static CPluginHandle load(std::string_view path) {
			if(!std::filesystem::exists(path))
				throw std::invalid_argument("Failed to open: " + std::string(path) + " (It doesn't seam to exist!)");

            std::string source;
			{
				std::ifstream fin(std::string{path});
				fin.seekg(0, std::ios::end);
				source.resize(fin.tellg());
				fin.seekg(0, std::ios::beg);
				fin.read((char*)source.data(), source.size());
			}

            return load_memory(source);
		}
		static std::unique_ptr<PluginHandleBase<PluginBase>> load_unique(std::filesystem::path path) {
			return std::make_unique<CPluginHandle>(load(path.string()));
		}

		static void register_loaders() {
			pluginsplusplus::PluginManager<PluginBase>::register_loader("c", CPluginHandle<PluginBase>::load_unique);
            pluginsplusplus::PluginManager<PluginBase>::register_loader("C", CPluginHandle<PluginBase>::load_unique);
		}

		
		bool step() override {
            static int (*stepCached)();
			if(!started) {
                auto start = (void (*)())tcc_get_symbol(state, "start");
                if(start) start();
				auto go = (void (*)(std::stop_token*))tcc_get_symbol(state, "go");
                if (go) thread = std::jthread([go](std::stop_token stop){
                    go(&stop);
                });

                stepCached = (int (*)())tcc_get_symbol(state, "step");
				started = true;
			}

            if (stepCached) return !stepCached();
            return true;
		}

		void stop() {
			auto stop = (void (*)())tcc_get_symbol(state, "stop");
            if (stop) stop();
            if (thread.joinable()) {
                thread.request_stop();
                thread.join();
            }
		}

	};

    // TODO: Is it fine for these to be here since they are templated?
    template<std::derived_from<plugin_base> PluginBase>
    std::filesystem::path CPluginHandle<PluginBase>::header;
    template<std::derived_from<plugin_base> PluginBase>
    std::filesystem::path CPluginHandle<PluginBase>::archive;

}