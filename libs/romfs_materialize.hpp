#ifndef __831f9716044f51c53a9863450669a023_ROMFS_MATERIALIZE__
#define __831f9716044f51c53a9863450669a023_ROMFS_MATERIALIZE__


#include <romfs/romfs.hpp>
#include <filesystem>
#include <random>
#include <fstream>


namespace romfs {
	struct tempary_filesystem {
		std::filesystem::path base = std::filesystem::temp_directory_path() / "libromfs_materialized";

		std::filesystem::path randomize_base_path() {
			return base = std::filesystem::temp_directory_path() / []{ return std::to_string(std::random_device{}()); }();
		}

		std::filesystem::path materialize_file(std::filesystem::path path) {
			if(exists(base / path)) return base / path;

			std::filesystem::create_directories((base / path).parent_path());
			std::ofstream fout(base / path, std::ios::binary);
			auto fileData = romfs::get(path); 
			fout.write((const char*)fileData.data(), fileData.size());
			return base / path;
		}

		std::vector<std::filesystem::path> materialize_all_files() {
			auto files = list();
			std::vector<std::filesystem::path> out;
			out.reserve(files.size());
			for(auto& path: files)
				out.emplace_back(materialize_file(path));
			return out;
		}
	};
}

#endif // __831f9716044f51c53a9863450669a023_ROMFS_MATERIALIZE__