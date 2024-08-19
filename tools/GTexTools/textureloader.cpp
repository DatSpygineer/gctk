#include <gctk/gctk.h>

#include <string>
#include <vector>
#include <filesystem>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

bool LoadTexture(const std::filesystem::path& path, Texture& texture, std::vector<uint8_t>& data) {
	if (path.extension() == ".gtex") {
		uint8_t* pdata;
		size_t data_size;
		bool result = false;
		if (GctkLoadTextureFromFileStoreData(&texture, path.string().c_str(), &pdata, &data_size)) {
			for (size_t i = 0; i < data_size; i++) {
				data.push_back(pdata[i]);
			}
			result = true;
		}
		free(pdata);
		return result;
	}
	return GctkLoadImageFromFile(&texture, path.string().c_str(), GCTK_IMAGE_FLAG_DEFAULT);
}
bool SaveTexture(const std::filesystem::path& path, const Texture& texture, std::vector<uint8_t>& data) {
	auto ext = path.extension().string();
	if (ext == ".gtex") {
		return GctkWriteTextureToFile(path.string().c_str(), &texture, data.data(), data.size());
	}

	int comp = 3;
	switch (texture.format) {
		case GCTK_GRAYSCALE: comp = 1; break;
		case GCTK_GRAYSCALE_ALPHA: comp = 2; break;
		case GCTK_RGB:
		case GCTK_INDEXED_8:
		case GCTK_INDEXED_16: comp = 3; break;
		default:
		case GCTK_RGBA:
		case GCTK_INDEXED_8_ALPHA:
		case GCTK_INDEXED_16_ALPHA: comp = 4; break;
	}

	if (ext == ".tga") {
		return stbi_write_tga(path.string().c_str(), texture.width, texture.height, comp, data.data());
	}
	if (ext == ".jpg" || ext == ".jpeg") {
		return stbi_write_jpg(path.string().c_str(), texture.width, texture.height, comp, data.data(), 100);
	}
	if (ext == ".png") {
		return stbi_write_png(path.string().c_str(), texture.width, texture.height, comp, data.data(), comp);
	}
	if (ext == ".bmp") {
		return stbi_write_bmp(path.string().c_str(), texture.width, texture.height, comp, data.data());
	}
	return false;
}