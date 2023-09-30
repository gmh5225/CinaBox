#ifndef __831f9716044f51c53a9863450669a023_INCLUDEABLE__
#define __831f9716044f51c53a9863450669a023_INCLUDEABLE__

#include <filesystem>
#include <random>
#include <fstream>

namespace includeable {
	struct tempary_filesystem {
		std::filesystem::path base = std::filesystem::temp_directory_path() / []{ return std::to_string(std::random_device{}()); }();

		std::filesystem::path materialize_file(std::filesystem::path path, std::string_view content) {
			std::filesystem::create_directories((base / path).parent_path());
			std::ofstream fout(base / path);
			fout << content;
			return base / path;
		}

		std::filesystem::path materialize_binary_file(std::filesystem::path path, const std::vector<uint8_t>& content) {
			std::filesystem::create_directories((base / path).parent_path());
			std::ofstream fout(base / path, std::ios::binary);
			fout.write((const char*)content.data(), content.size());
			return base / path;
		}
	};

	std::string materialize_string(std::string_view s) { return std::string(s); }

	template<typename T = uint8_t>
	std::vector<T> materialize_binary_vector(const std::vector<uint8_t>& v) {
		return {(T*)v.data(), (T*)v.data() + v.size() / sizeof(T)};
	}

	std::string materialize_binary_string(const std::vector<uint8_t>& v) {
		auto tmp = materialize_binary_vector<char>(v);
		return {tmp.begin(), tmp.end()};
	}
}

#endif // __831f9716044f51c53a9863450669a023_INCLUDEABLE__