#include <print>
#include <vector>
#include <fstream>
#include <cstring>
#include <filesystem>
#include <unordered_map>

#ifdef _WIN32
#define strcasecmp stricmp
#endif

#define GPKG_VERSION_MAJOR 0
#define GPKG_VERSION_MINOR 1

constexpr std::string_view GPKG_VERSION = "1.0";
constexpr uint8_t GPKG_IDENTIFIER[] = { 'G', 'P', 'K', 'G' };

int main(int argc, char** argv) {
	if (argc < 2) {
		std::println("Expected an argument!");
		return 1;
	}
	if (argc == 2) {
		if (strcasecmp(argv[1], "--help") == 0 || strcasecmp(argv[1], "-h") == 0) {
			std::println("GPkg v{}.{}", GPKG_VERSION, GPKG_VERSION_MAJOR, GPKG_VERSION_MINOR);
			std::println(
				"Usage:\n"
				"gpkg --help|-h    ==> Show help message\n"
				"gpkg --version|-v ==> Show tool version\n"
				"gpkg --input <path> --output <path> <flags> ==> Create archive from input directory and save it to specific output path\n"
				"Additional flags:\n<No additional flags implemented yet>"
				// "-v|--version <major>.<minor> ==> Specify archive version\n"
				// "-c|--compress                ==> Compress archive"
			);
			return 0;
		}
		if (strcasecmp(argv[1], "--version") == 0 || strcasecmp(argv[1], "-v") == 0) {
			std::println("GPkg v{}.{}", GPKG_VERSION, GPKG_VERSION_MAJOR, GPKG_VERSION_MINOR);
			return 0;
		}

		std::println("Invalid argument: {}", argv[1]);
		return 1;
	}

	if (argc < 5) {
		std::string args = "";
		for (int i = 1; i < argc; i++) {
			if (!args.empty()) {
				args += " ";
			}
			args += argv[i];
		}
		std::println("Invalid arguments: {}", args);
		return 1;
	}

	std::filesystem::path input_path;
	std::filesystem::path output_path;

	if (strcasecmp(argv[1], "--input") == 0 || strcasecmp(argv[1], "-i") == 0) {
		input_path = argv[2];
		if (strcasecmp(argv[3], "--output") == 0 || strcasecmp(argv[3], "-o") == 0) {
			output_path = argv[4];
		}
	} else if (strcasecmp(argv[1], "--output") == 0 || strcasecmp(argv[1], "-o") == 0) {
		output_path = argv[2];
		if (strcasecmp(argv[3], "--input") == 0 || strcasecmp(argv[3], "-i") == 0) {
			input_path = argv[4];
		}
	}

	if (input_path.empty()) {
		std::println("Input is not specified!");
		return 1;
	}
	if (output_path.empty()) {
		std::println("Output is not specified!");
		return 1;
	}

	std::unordered_map<std::string, std::filesystem::path> entries;

	uint32_t data_origin = 10;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(input_path)) {
		auto entry_path = entry.path();
		auto name = std::filesystem::relative(entry_path, input_path).string();
		if (std::filesystem::is_regular_file(entry_path)) {
			entries.emplace(name, entry_path);
			data_origin += 9 + name.size();
		}
	}
	const uint32_t entry_count = entries.size();

	constexpr uint8_t major = GPKG_VERSION_MAJOR;
	constexpr uint8_t minor = GPKG_VERSION_MINOR;

	std::println("Writing archive v{}.{}, data origin: {}, entry count: {}", major, minor, data_origin, entry_count);
	output_path.replace_extension(".gpkg");
	std::ofstream ofs(output_path, std::ios::binary);
	ofs.write(reinterpret_cast<const char*>(GPKG_IDENTIFIER), 4);
	ofs.write(reinterpret_cast<const char*>(&major), 1);
	ofs.write(reinterpret_cast<const char*>(&minor), 1);
	ofs.write(reinterpret_cast<const char*>(&data_origin), 4);
	ofs.write(reinterpret_cast<const char*>(&entry_count), 4);

	std::vector<uint8_t> data_result;

	uint32_t origin = 0;
	for (const auto& [name, path] : entries) {
		std::ifstream ifs(path, std::ios::binary);
		ifs.seekg(0, std::ios::end);
		uint32_t size = ifs.tellg();
		ifs.seekg(0, std::ios::beg);

		data_result.resize(data_result.capacity() + size);
		ifs.read(reinterpret_cast<char*>(data_result.data()), size);

		ifs.close();

		uint16_t name_length = name.size();
		ofs.write(reinterpret_cast<const char*>(&name_length), 2);
		ofs.write(name.data(), name_length);
		ofs.write(reinterpret_cast<const char*>(&origin), 4);
		ofs.write(reinterpret_cast<const char*>(&size), 4);

		std::println("Added entry \"{}\"! Origin: {}, size: {}", path.string(), origin, size);
		origin += size;
	}

	ofs.write(reinterpret_cast<const char*>(data_result.data()), static_cast<std::streamsize>(data_result.size()));
	ofs.flush();
	ofs.close();

	std::println("Archive \"{}\" has been created!", output_path.string());

	return 0;
}