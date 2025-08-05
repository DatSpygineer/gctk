#pragma once

#include <memory>
#include <unordered_map>

#include "gctk_filesys.hpp"
#include "gctk_debug.hpp"
#include "gctk_version.hpp"

namespace gctk {
	enum class AssetType {
		Invalid,
		PlainText,
		JsonData,
		Script,
#ifdef GCTK_CLIENT
		Texture,
		Mesh,
		Animation,
		Shader,
		Material,
		SoundEffect,
#else // GCTK_SERVER
		Map
#endif
	};

	class Asset;
	class AssetPack;

	using AssetRef = std::shared_ptr<Asset>;
	using AssetPackRef = std::shared_ptr<AssetPack>;

	class Asset final {
		void* m_pData;
		size_t m_uSize;
		AssetType m_eType;
	public:
		Asset() : m_pData(nullptr), m_uSize(0), m_eType(AssetType::Invalid) { }
		~Asset();

		[[nodiscard]] constexpr void* data() { return m_pData; }
		[[nodiscard]] constexpr const void* data() const { return m_pData; }
		[[nodiscard]] constexpr size_t size() const { return m_uSize; }
		[[nodiscard]] constexpr AssetType type() const { return m_eType; }

		static AssetRef Load(const std::string& path);
		template<typename T>
		static const T* Load(const std::string& path) {
			const auto asset = Load(path);
			if (asset == nullptr) {
				return nullptr;
			}

			auto ptr = static_cast<const T*>(asset->data());
			Assert(ptr != nullptr, std::format("Failed to cast asset \"{}\" to type \"{}\"", path, typeid(T).name()));
			return ptr;
		}
	};

	class AssetPack final {
		struct EntryInfo {
			size_t origin, size;
		};

		FILE* m_pStream;
		std::unordered_map<std::string, EntryInfo> m_entries;
		uint32_t m_uDataOrigin;
		Version m_version;
	public:
		AssetPack() : m_pStream(nullptr), m_uDataOrigin(0), m_version(CurrentVersion()) { }
		~AssetPack();

		[[nodiscard]] constexpr bool is_open() const { return m_pStream != nullptr; }
		static AssetPackRef Open(const std::string& path);

		[[nodiscard]] constexpr size_t entry_count() const { return m_entries.size(); }
		[[nodiscard]] inline bool contains_entry(const std::string& name) const { return m_entries.contains(name); }
		[[nodiscard]] constexpr Version version() const { return m_version; }

		static constexpr Version CurrentVersion() { return Version { 0, 1 }; }

		friend class Asset;
	};

	namespace AssetSystem {
		bool Initialize(const std::vector<std::string>& asset_packs, const std::optional<Path>& mod_path = std::nullopt);
	}
}
