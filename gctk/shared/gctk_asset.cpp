#include "gctk_asset.hpp"

#include <algorithm>
#include <fstream>
#include <ranges>
#include <vector>
#include <cstring>

#include "gctk_str.hpp"

namespace gctk {
	static std::unordered_map<std::string, AssetRef> s_assets;
	static std::vector<AssetPackRef> s_asset_packs;
	static std::optional<Path> s_mod_path;
	static bool s_assets_initialized = false;

	static constexpr uint8_t s_pack_identifier[4] = { 'G', 'P', 'K', 'G' };
	static constexpr uint8_t s_pack_identifier_mirrored[4] = { 'G', 'K', 'P', 'G' };

	Asset::~Asset() {
		if (m_pData != nullptr) {
			free(m_pData);
		}
	}

	AssetRef Asset::Load(const std::string& path) {
		if (s_assets.contains(path)) {
			return s_assets.at(path);
		}

		const auto trimmed_path = StringUtil::Trim(path);

		for (auto& pack : std::ranges::reverse_view(s_asset_packs)) {
			if (pack->contains_entry(trimmed_path)) {
				auto ext = StringUtil::ToLower(Path(trimmed_path).extension().string());
				AssetType type;

				if (ext == ".txt" || ext == ".cfg" || ext == ".ini" || ext == ".glsl") {
					type = AssetType::PlainText;
				} else if (ext == ".xml") {
					type = AssetType::XmlData;
				} else if (ext == ".lua") {
					type = AssetType::Script;
				}
#ifdef GCTK_CLIENT
				else if (ext == ".png" || ext == ".tga") {
					type = AssetType::TextureImage;
				} else if (ext == ".gtex") {
					type = AssetType::TextureDef;
				} else if (ext == ".gmdl") {
					type = AssetType::Mesh;
				} else if (ext == ".gani") {
					type = AssetType::Animation;
				} else if (ext == ".gshd") {
					type = AssetType::Shader;
				} else if (ext == ".gmat") {
					type = AssetType::Material;
				} else if (ext == ".gsnd") {
					type = AssetType::SoundEffect;
				}
#else
				else if (ext == ".gmap") {
					type = AssetType::Map;
				}
#endif
				else {
					LogErr("Cannot load asset \"{}\". Asset extension \"{}\" is not supported!", path, ext);
					return nullptr;
				}

				const auto& [origin, size] = pack->m_entries.at(path);
				fseek(pack->m_pStream, static_cast<long>(origin), SEEK_SET);

				void* p = malloc(size);
				fread(p, 1, size, pack->m_pStream);
				fseek(pack->m_pStream, pack->m_uDataOrigin, SEEK_SET);

				auto asset = std::make_shared<Asset>();
				asset->m_pData = p;
				asset->m_uSize = size;
				asset->m_eType = type;

				s_assets.emplace(path, asset);
				return asset;
			}
		}
		return nullptr;
	}

	AssetReader::AssetReader(Asset& asset) : std::istream(&m_buffer), m_buffer(asset) {
		rdbuf(&m_buffer);
	}

	AssetReader::AssetReaderBuffer::AssetReaderBuffer(Asset& asset) : m_asset(asset) {
		setg(
			static_cast<char*>(m_asset.m_pData),
			static_cast<char*>(m_asset.m_pData),
			static_cast<char*>(m_asset.m_pData) + m_asset.m_uSize
		);
	}

	std::string AssetReader::read_string(const size_t char_n) {
		std::string result;
		result.reserve(char_n);
		read(result.data(), static_cast<std::streamsize>(char_n));
		result.shrink_to_fit();
		return result;
	}

	std::string AssetReader::read_string() {
		std::string result;
		result.reserve(remaining_size());
		while (!eof()) {
			const auto c = get();
			if (c == EOF) {
				break;
			}
			result.push_back(static_cast<char>(c));
		}
		result.shrink_to_fit();
		return result;
	}

	std::string AssetReader::read_all_to_string() {
		std::string result;
		result.reserve(size());
		read(result.data(), size());
		result.shrink_to_fit();
		return result;
	}

	std::streamsize AssetReader::size() {
		const auto pos = tellg();
		seekg(0, std::ios::end);
		const auto size = tellg();
		seekg(pos, std::ios::beg);

		return size;
	}

	std::streamsize AssetReader::remaining_size() {
		const auto pos = tellg();
		seekg(0, std::ios::end);
		const auto size = tellg();
		seekg(pos, std::ios::beg);
		return size - pos;
	}

	AssetPack::~AssetPack() {
		if (m_pStream != nullptr) {
			fclose(m_pStream);
		}
	}

	AssetPackRef AssetPack::Open(const std::string& path) {
		FILE* f = fopen(path.c_str(), "rb");
		if (f == nullptr) {
			LogErr("Failed to open asset pack \"{}\"", path);
			return nullptr;
		}

		uint32_t identifier;
		if (fread(&identifier, sizeof(uint32_t), 1, f) <= 0) {
			LogErr("Failed to read identifier of asset pack \"{}\"", path);
			fclose(f);
			return nullptr;
		}

		if (memcmp(&identifier, s_pack_identifier, 4) != 0 && memcmp(&identifier, s_pack_identifier_mirrored, 4) != 0) {
			LogErr("Failed to load asset pack \"{}\": Identifier is invalid", path);
			fclose(f);
			return nullptr;
		}

		uint8_t major, minor;
		if (fread(&major, sizeof(uint8_t), 1, f) <= 0) {
			LogErr("Failed to read version of asset pack \"{}\"", path);
			fclose(f);
			return nullptr;
		}
		if (fread(&minor, sizeof(uint8_t), 1, f) <= 0) {
			LogErr("Failed to read version of asset pack \"{}\"", path);
			fclose(f);
			return nullptr;
		}

		const Version version(major, minor);

		if (version > CurrentVersion()) {
			LogErr("Failed to load asset pack \"{}\": Unsupported archive version {}.{}", path, version.major, version.minor);
			fclose(f);
			return nullptr;
		}

		if (version < CurrentVersion()) {
			// TODO: Add version migration, if needed
		}

		uint32_t data_origin = 0;
		if (fread(&data_origin, sizeof(uint32_t), 1, f) <= 0) {
			LogErr("Failed to read data origin of asset pack \"{}\"", path);
			fclose(f);
			return nullptr;
		}
		uint32_t entry_count;
		if (fread(&entry_count, sizeof(uint32_t), 1, f) <= 0) {
			LogErr("Failed to read entry count of asset pack \"{}\"", path);
			fclose(f);
			return nullptr;
		}

		if (entry_count == 0) {
			LogErr("Failed to load asset pack \"{}\": Entry count must be greater then zero!", path);
			fclose(f);
			return nullptr;
		}

		std::unordered_map<std::string, EntryInfo> entries;
		entries.reserve(entry_count);
		for (uint32_t i = 0; i < entry_count; i++) {
			uint32_t entry_origin, entry_size;
			uint16_t name_length;
			std::string name;

			fread(&name_length, sizeof(uint16_t), 1, f);
			name.resize(name_length);
			fread(name.data(), sizeof(char), name_length, f);

			fread(&entry_origin, sizeof(uint32_t), 1, f);
			fread(&entry_size, sizeof(uint16_t), 1, f);
			entries.emplace(name, EntryInfo { entry_origin, entry_size });
		}

		if (const long i = ftell(f); data_origin < i) {
			LogErr("Failed to load asset pack \"{}\": Data origin is out of range", path);
			fclose(f);
			return nullptr;
		}

		auto asset_pack = std::make_shared<AssetPack>();
		asset_pack->m_pStream = f;
		asset_pack->m_uDataOrigin = data_origin;
		asset_pack->m_entries = std::move(entries);
		asset_pack->m_version = version;
		fseek(f, data_origin, SEEK_SET);
		return asset_pack;
	}

	bool AssetSystem::Initialize(const std::vector<std::string>& asset_packs, const std::optional<Path>& mod_path) {
		if (s_assets_initialized) {
			return false;
		}

		s_mod_path = mod_path;
		for (const auto& pack : asset_packs) {
			const auto path = Paths::GameBasePath() / pack;
			if (auto asset_pack = AssetPack::Open(path); asset_pack != nullptr) {
				s_asset_packs.emplace_back(asset_pack);
			} else {
				FatalError("Failed to initialize asset pack \"{}\"", path);
			}
		}

		if (s_mod_path.has_value()) {
			const auto list_path = s_mod_path.value() / "mods.txt";
			std::string name;
			std::ifstream modlist_fs(list_path);
			while (std::getline(modlist_fs, name)) {
				const auto path = s_mod_path.value() / name;
				if (!Paths::exists(path)) {
					LogErr("Failed to load mod pack \"{}\": File doesn't exists!", path);
					continue;
				}
				if (auto asset_pack = AssetPack::Open(path); asset_pack != nullptr) {
					s_asset_packs.emplace_back(asset_pack);
				}
			}
		}

		s_assets_initialized = true;
		return true;
	}
}