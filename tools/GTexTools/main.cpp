#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

#include <gctk/gctk.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

extern "C" {
	#include "tinyfiledialogs.h"
}

#ifdef MessageBox
	#undef MessageBox
#endif

enum class MessageBoxType {
	Info,
	Warning,
	Error,
	Question
};
enum class MessageBoxButtons {
	Ok,
	OkCancel,
	YesNo,
	YesNoCancel
};

namespace fs = std::filesystem;

static bool ClampR = false, ClampS = false, ClampT = false;
static bool ClampR_Prev = false, ClampS_Prev = false, ClampT_Prev = false;
static bool Nearest = false;
static bool Nearest_Prev = false;
static bool MipMaps = false;
static bool MipMaps_Prev = false;
static bool ShowTextureSplitDialogue = false;
static int  SplitMode;
static bool Changes = false;
static int MipMapPreview = 0, MipMapPreview_Prev = 0;
static int FramePreview = 0, FramePreview_Prev = 0;
static int JpgQuality = 100;

static Texture TEXTURE = { };
static Sprite SPRITE = { };
static Transform2D TRANSFORM = {
	MAT4_IDENTITY,
	VEC2_ZERO,
	VEC2_ZERO,
	VEC2(500.0f, 500.0f),
	0, 0,
	nullptr
};

static Vec2i FrameSize_Prev;
static Vec2i FrameSize;
static int ImageSize[3] = { 0 };
static fs::path TexturePath = "";

static const char* TargetNames[] = {
	"1D", "2D", "3D", "Cubemap",
	"1D Array", "2D Array", "???", "Cubemap Array",
};

static const char* FormatName(TextureFormat format) {
	switch (format & ~GCTK_WITH_RLE) {
		case GCTK_GRAYSCALE: return "L 8 (Grayscale)";
		case GCTK_GRAYSCALE_ALPHA: return "LA 8-8 (Grayscale with alpha)";
		case GCTK_INDEXED_8: return "IDX 8 (Indexed 8-bit)";
		case GCTK_INDEXED_8_ALPHA: return "IDA88 (Indexed 8-bit with alpha)";
		case GCTK_INDEXED_16: return "IDX 16 (Indexed 16-bit)";
		case GCTK_INDEXED_16_ALPHA: return "IDA 16-8 (Indexed 8-bit with alpha)";
		case GCTK_RGB: return "RGB 8-8-8";
		case GCTK_RGBA: return "RGBA 8-8-8-8 (RGB with 8-bit alpha)";
		default: return "???";
	}
}

enum class TextureTargetSelection : int {
	Target1D,
	Target2D,
	Target3D,
	Cubemap,
	Target1D_Array,
	Target2D_Array,
	Cubemap_Array
};

enum class TextureFormatSelection : int {
	Grayscale,
	Indexed8,
	Indexed16,
	Rgb
};

enum class ImageFileFormat {
	Invalid,
	GTex,
	Png,
	Jpg,
	Bmp,
	Tga
};

static TextureTargetSelection TargetSelect = TextureTargetSelection::Target2D;
static TextureFormatSelection FormatSelect = TextureFormatSelection::Rgb;
static TextureTargetSelection TargetSelectPrevious = TextureTargetSelection::Target2D;
static TextureFormatSelection FormatSelectPrevious = TextureFormatSelection::Rgb;

static ImGuiIO* io;

static std::vector<std::string> OpenFile(const std::string& title, bool allow_multiple, const std::string& filter_desc,
					 const std::initializer_list<std::string>& filters) {
	std::vector<const char*> filters_cstr;
	for (const auto& filter : filters) {
		filters_cstr.push_back(filter.c_str());
	}

	auto path = tinyfd_openFileDialog(title.c_str(), nullptr, static_cast<int>(filters.size()),
											 filters_cstr.data(), filter_desc.c_str(), allow_multiple);

	if (path == nullptr || strlen(path) == 0) {
		return { };
	}

	std::vector<std::string> result;
	if (allow_multiple && std::string(path).find('|') != std::string::npos) {
		std::istringstream iss(path);
		std::string token;
		while (std::getline(iss, token, '|')) {
			result.push_back(token);
		}
	} else {
		result = { path };
	}

	return result;
}
static std::string SaveFile(const std::string& title, const std::string& filter_desc,
								  const std::initializer_list<std::string>& filters) {
	std::vector<const char*> filters_cstr;
	for (const auto& filter : filters) {
		filters_cstr.push_back(filter.c_str());
	}

	auto path = tinyfd_saveFileDialog(title.c_str(), nullptr, static_cast<int>(filters.size()), filters_cstr.data(),
								 filter_desc.c_str());
	return path == nullptr ? "" : std::string { path };
}

static int MessageBox(const std::string& title, const std::string& message, MessageBoxType type = MessageBoxType::Info,
					  MessageBoxButtons buttons = MessageBoxButtons::Ok, int default_button = 0) {
	const char* type_cstr;
	switch (type) {
		default:
		case MessageBoxType::Info: type_cstr = "info"; break;
		case MessageBoxType::Warning: type_cstr = "warning"; break;
		case MessageBoxType::Error: type_cstr = "error"; break;
		case MessageBoxType::Question: type_cstr = "question"; break;
	}
	const char* btn_cstr;
	switch (buttons) {
		case MessageBoxButtons::Ok: btn_cstr = "ok"; break;
		case MessageBoxButtons::OkCancel: btn_cstr = "okcancel"; break;
		case MessageBoxButtons::YesNo: btn_cstr = "yesno"; break;
		case MessageBoxButtons::YesNoCancel: btn_cstr = "yesnocancel"; break;
	}

	return tinyfd_messageBox(title.c_str(), message.c_str(), btn_cstr, type_cstr, default_button);
}

static GLsizei GetImageData(std::vector<uint8_t>& result) {
	GLsizei image_size = GctkMin(TEXTURE.width, 1) * GctkMin(TEXTURE.height, 1) * GctkMin(TEXTURE.depth, 1);
	GLenum gl_format = GL_RGBA;
	switch (TEXTURE.format) {
		case GCTK_GRAYSCALE: {
			gl_format = GL_RED;
		} break;
		case GCTK_GRAYSCALE_ALPHA: {
			gl_format = GL_RG;
			image_size *= 2;
		} break;
		case GCTK_RGB:
		case GCTK_INDEXED_16:
		case GCTK_INDEXED_8: {
			gl_format = GL_RGB;
			image_size *= 3;
		} break;
		default:
		case GCTK_RGBA:
		case GCTK_INDEXED_16_ALPHA:
		case GCTK_INDEXED_8_ALPHA: {
			gl_format = GL_RGBA;
			image_size *= 4;
		} break;
	}

	result = { };
	result.reserve(image_size);

	GctkGLCall(glGetTextureImage(TEXTURE.id, 0, gl_format, GL_UNSIGNED_BYTE, image_size, result.data()));
	return image_size;
}

static bool SaveImageAs(const fs::path& path, ImageFileFormat file_format) {
	std::vector<uint8_t> data;
	if (GetImageData(data) > 0) {
		int stride;
		switch (TEXTURE.format) {
			case GCTK_GRAYSCALE: {
				stride = 1;
			} break;
			case GCTK_GRAYSCALE_ALPHA: {
				stride = 2;
			} break;
			case GCTK_RGB:
			case GCTK_INDEXED_16:
			case GCTK_INDEXED_8: {
				stride = 3;
			} break;
			default:
			case GCTK_RGBA:
			case GCTK_INDEXED_16_ALPHA:
			case GCTK_INDEXED_8_ALPHA: {
				stride = 4;
			} break;
		}

		switch (file_format) {
			case ImageFileFormat::GTex: {
				
			} break;
			case ImageFileFormat::Png: {
				stbi_write_png(path.string().c_str(), TEXTURE.width, TEXTURE.height, stride, data.data(), stride);
			} break;
			case ImageFileFormat::Jpg: {
				stbi_write_jpg(path.string().c_str(), TEXTURE.width, TEXTURE.height, stride, data.data(), JpgQuality);
			} break;
			case ImageFileFormat::Bmp: {
				stbi_write_bmp(path.string().c_str(), TEXTURE.width, TEXTURE.height, stride, data.data());
			} break;
			case ImageFileFormat::Tga: {
				stbi_write_tga(path.string().c_str(), TEXTURE.width, TEXTURE.height, stride, data.data());
			} break;
			default: return false;
		}

		return true;
	}

	MessageBox("Error", "Failed to get image data!", MessageBoxType::Error);
	return false;
}

int main(int argc, char** argv) {
	if (!GctkInit(argc, argv, "GTexTools", "Gctk", Version { 1, 0, 0, GCTK_VERSION_ALPHA })) {
		return 1;
	}

	GctkSetMessageCallback([](GctkDebugMessageType message_type, const char* message, GctkDebugInfo debug_info) {
		static_cast<void>(debug_info);
		if (message_type == GCTK_MESSAGE_ERROR) {
			MessageBox("Engine Error!", "Engine error has occured: " + std::string(message), MessageBoxType::Error);
		}
	});

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow*>(GctkGetWindowHandle()), true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	GctkSetBackgroundColor(Color { 0.25f, 0.25f, 0.25f, 1.0f });
	GctkSetUpdateCallback([](double delta) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImVec2 menu_size;
		ImGui::BeginMainMenuBar();
			menu_size = ImGui::GetWindowSize();
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Open Texture..")) {
					auto results = OpenFile("Select your texture file", false, "Texture file", { "*.gtex" });
					if (!results.empty()) {
						if (TEXTURE.id != 0) {
							GctkDeleteTexture(&TEXTURE);
						}

						auto path = results.front();
						if (fs::path(path).extension() != ".gtex") {
							MessageBox("Error", "Unsupported extension \"" + fs::path(path).extension().string() + "\", expected *.gtex", MessageBoxType::Error);
						} else {
							if (!GctkLoadTextureFromFile(&TEXTURE, path.c_str())) {
								MessageBox("Error", "Failed to load texture \"" + path + "\"", MessageBoxType::Error);
								TEXTURE.id = 0;
							} else {
								ClampR  = ClampR_Prev  = TEXTURE.clamp_r;
								ClampS  = ClampS_Prev  = TEXTURE.clamp_s;
								ClampT  = ClampT_Prev  = TEXTURE.clamp_t;
								Nearest = Nearest_Prev = TEXTURE.point_filter;
								MipMaps = MipMaps_Prev = TEXTURE.mipmaps;

								TRANSFORM.scale = VEC2(
									500.0f,
									500.0f * (static_cast<float>(TEXTURE.height) / static_cast<float>(TEXTURE.width))
								);

								if (SPRITE.mesh.vbo == 0) {
									GctkCreateSprite(&SPRITE, GctkSprite2DDefaultShader(), &TEXTURE);
								} else {
									SPRITE.texture = &TEXTURE;
								}

								ImageSize[0] = TEXTURE.width;
								ImageSize[1] = TEXTURE.height;
								ImageSize[2] = TEXTURE.depth;

								TexturePath = path;
							}
						}
					}
				}
				ImGui::BeginDisabled(TEXTURE.id == 0 || SPRITE.mesh.vbo == 0);
				if (ImGui::MenuItem("Save Texture")) {
					if (TexturePath.extension() != ".gtex") {
						auto path = SaveFile("Save texture file", "GTex texture", {  "*.gtex" });
						if (!path.empty()) {
							TexturePath = path;
							SaveImageAs(TexturePath, ImageFileFormat::GTex);
						}
					} else {
						SaveImageAs(TexturePath, ImageFileFormat::GTex);
					}
				}
				if (ImGui::MenuItem("Save Texture as..")) {
					auto path = SaveFile("Save texture file", "GTex texture", {  "*.gtex" });
					if (!path.empty()) {
						TexturePath = path;
						SaveImageAs(TexturePath, ImageFileFormat::GTex);
					}
				}
				if (ImGui::MenuItem("Close texture")) {
					if (Changes) {
						switch (MessageBox("Confirm exit", "There are unsaved changes, would you like to save before exiting?",
								   MessageBoxType::Question, MessageBoxButtons::YesNoCancel)) {
							case 1: { // Yes
								// TODO: Save
							} break;
							case 2: { // No
								GctkDeleteSprite(&SPRITE);
								GctkDeleteTexture(&TEXTURE);
								memset(&TEXTURE, 0, sizeof(TEXTURE));
							} break;
							default: /* Cancel, do nothing */ break;
						}
					} else {
						GctkDeleteSprite(&SPRITE);
						GctkDeleteTexture(&TEXTURE);
						memset(&TEXTURE, 0, sizeof(TEXTURE));
					}
				}
				ImGui::EndDisabled();
				ImGui::Separator();
				if (ImGui::MenuItem("Import image..")) {
					auto results = OpenFile("Select image to import", false, "Image file",
						{ "*.png", "*.jpg", "*.jpeg", "*.bmp", "*.tga" });
					if (!results.empty()) {
						if (TEXTURE.id != 0) {
							GctkDeleteTexture(&TEXTURE);
						}

						auto path = results.front();
						if (std::string(".png.jpg.jpeg.bmp.tga").find(fs::path(path).extension().string())) {
							MessageBox("Error", "Unsupported extension \"" + fs::path(path).extension().string() +
								"\", expected: \"*.png\", \"*.jpg\", \"*.jpeg\", \"*.bmp\", \"*.tga\"",
								MessageBoxType::Error);
						} else {
							if (!GctkLoadImageFromFile(&TEXTURE, path.c_str(), GCTK_IMAGE_FLAG_DEFAULT)) {
								MessageBox("Error", "Failed to import image \"" + path + "\"", MessageBoxType::Error);
								TEXTURE.id = 0;
							} else {
								ClampR  = ClampR_Prev  = false;
								ClampS  = ClampS_Prev  = false;
								ClampT  = ClampT_Prev  = false;
								Nearest = Nearest_Prev = false;
								MipMaps = MipMaps_Prev = false;
								TRANSFORM.scale = VEC2(
									500.0f,
									500.0f * (static_cast<float>(TEXTURE.height) / static_cast<float>(TEXTURE.width))
								);

								if (SPRITE.mesh.vbo == 0) {
									GctkCreateSprite(&SPRITE, GctkSprite2DDefaultShader(), &TEXTURE);
								}

								TexturePath = path;

								ImageSize[0] = TEXTURE.width;
								ImageSize[1] = TEXTURE.height;
								ImageSize[2] = TEXTURE.depth;
							}
						}
					}
				}
				ImGui::BeginDisabled(TEXTURE.id == 0 || SPRITE.mesh.vbo == 0);
				if (ImGui::MenuItem("Export image..")) {
					auto path = SaveFile("Save image file", "Image file", {  "*.png", "*.jpg", "*.jpeg", "*.bmp", "*.tga" });
					if (!path.empty()) {
						ImageFileFormat format;
						auto ext = fs::path(path).extension().string();
						if (ext == ".png") {
							format = ImageFileFormat::Png;
						} else if (ext == ".jpg" || ext == ".jpeg") {
							format = ImageFileFormat::Jpg;
						} else if (ext == ".bmp") {
							format = ImageFileFormat::Bmp;
						} else if (ext == ".tga") {
							format = ImageFileFormat::Tga;
						} else if (ext == ".gtex") {
							format = ImageFileFormat::GTex;
						} else {
							MessageBox("Error", "Unsupported extension \"" + ext + "\"", MessageBoxType::Error);
							format = ImageFileFormat::Invalid;
						}

						if (format != ImageFileFormat::Invalid) {
							SaveImageAs(path, format);
						}
					}
				}
				ImGui::EndDisabled();
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) {
					if (Changes) {
						switch (MessageBox("Confirm exit", "There are unsaved changes, would you like to save before exiting?",
								   MessageBoxType::Question, MessageBoxButtons::YesNoCancel)) {
							case 1: { // Yes
								// TODO: Save
							} break;
							case 2: { // No
								GctkClose();
							} break;
							default: /* Cancel, do nothing */ break;
						}
					} else {
						GctkClose();
					}
				}
				ImGui::EndMenu();
			}
		if (ImGui::BeginMenu("Edit")) {
			ImGui::BeginDisabled(TEXTURE.id == 0);
			if (ImGui::MenuItem("Split texture")) {
				ShowTextureSplitDialogue = true;
			}
			ImGui::EndDisabled();
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();

		Vec2i window_size = GctkGetWindowSize();
		ImGui::SetNextWindowPos({ 0, menu_size.y });
		ImGui::SetNextWindowSize({
			static_cast<float>(window_size.x) * 0.25f,
			static_cast<float>(window_size.y) - static_cast<float>(menu_size.y)
		});
		ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration);
				ImGui::SeparatorText("Texture Properties");
			ImGui::Checkbox("Clamp R", &ClampR); ImGui::SameLine();
			ImGui::Checkbox("Clamp S", &ClampS); ImGui::SameLine();
			ImGui::Checkbox("Clamp T", &ClampT);
			ImGui::Checkbox("Use nearest filter", &Nearest);
				ImGui::Spacing();
			ImGui::Checkbox("Generate MipMaps", &MipMaps);
			ImGui::BeginDisabled(!MipMaps);
				ImGui::Text("Preview MipMap:"); ImGui::SameLine();
				ImGui::SliderInt("##PreviewMipMap", &MipMapPreview, 0, 10);
			ImGui::EndDisabled();
				ImGui::SeparatorText("Image Info");
			ImGui::Text("Size:"); ImGui::SameLine();
			ImGui::InputInt3("##Size", ImageSize, ImGuiInputTextFlags_ReadOnly);

			ImGui::BeginDisabled(
				!(TEXTURE.target == GCTK_TEXTURE_1D_ARRAY  ||
				 TEXTURE.target ==  GCTK_TEXTURE_2D_ARRAY ||
				 TEXTURE.target ==  GCTK_TEXTURE_3D)
			);
			ImGui::Text("Frame Index:"); ImGui::SameLine();
			ImGui::InputInt("##FrameIndex", &FramePreview);
			ImGui::EndDisabled();

			ImGui::Text("Target: %s", TEXTURE.id == 0 ? "???" : TargetNames[TEXTURE.target]);
			ImGui::Text("Format: %s", TEXTURE.id == 0 ? "???" : FormatName(TEXTURE.format));
			ImGui::Text("OpenGL Texture ID: %d", TEXTURE.id);

			if (ClampR != ClampR_Prev) {
				ClampR_Prev = ClampR;
				glBindTexture  (GL_TEXTURE_2D, TEXTURE.id);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ClampT ? GL_CLAMP : GL_REPEAT);
				glGenerateMipmap(GL_TEXTURE_2D);
				Changes = true;
			}
			if (ClampS != ClampS_Prev) {
				ClampS_Prev = ClampS;
				glBindTexture  (GL_TEXTURE_2D, TEXTURE.id);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ClampS ? GL_CLAMP : GL_REPEAT);
				glGenerateMipmap(GL_TEXTURE_2D);
				Changes = true;
			}
			if (ClampT != ClampT_Prev) {
				ClampT_Prev = ClampT;
				Nearest_Prev = Nearest;
				glBindTexture  (GL_TEXTURE_2D, TEXTURE.id);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ClampT ? GL_CLAMP : GL_REPEAT);
				glGenerateMipmap(GL_TEXTURE_2D);
				Changes = true;
			}
			if (MipMaps != MipMaps_Prev) {
				MipMaps_Prev = MipMaps;
				Changes = true;

				glBindTexture(GL_TEXTURE_2D, TEXTURE.id);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			if (Nearest != Nearest_Prev) {
				Nearest_Prev = Nearest;
				glBindTexture  (GL_TEXTURE_2D, TEXTURE.id);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Nearest ? GL_NEAREST : GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					MipMaps ?
					(Nearest ? GL_NEAREST : GL_LINEAR) :
					(Nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR)
				);
				glGenerateMipmap(GL_TEXTURE_2D);
				Changes = true;
			}

			if (MipMapPreview != MipMapPreview_Prev) {
				glBindTexture  (GL_TEXTURE_2D, TEXTURE.id);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, MipMapPreview);
			}
		ImGui::End();

		if (ShowTextureSplitDialogue) {
			if (ImGui::Begin("Split texture", &ShowTextureSplitDialogue)) {
				if (TEXTURE.width == TEXTURE.height) {
					FrameSize = VEC2I(TEXTURE.width, TEXTURE.height);
				} else if (TEXTURE.width > TEXTURE.height) {
					FrameSize = VEC2I(
						static_cast<int>(TEXTURE.width * (static_cast<float>(TEXTURE.height) / TEXTURE.width)),
						TEXTURE.height
					);
				} else {
					FrameSize = VEC2I(
						TEXTURE.width,
						static_cast<int>(TEXTURE.height * (static_cast<float>(TEXTURE.width) / TEXTURE.height))
					);
				}

				ImGui::Text("Frame Size:");
				ImGui::SameLine();
				ImGui::InputInt2("##FrameSize", FrameSize.items);

				ImGui::Text("Target type:");
				ImGui::SameLine();
				ImGui::Combo("##Targettype", &SplitMode, "Animated Sprite\000Tilemap\000");
				ImGui::Spacing();
				if (ImGui::Button("Apply")) {
					if (FrameSize.x <= 0 || FrameSize.y <= 0) {
						MessageBox("Error!",
								   "Invalid frame size! Frame width and height must be non-zero positive number!",
								   MessageBoxType::Error);
					} else {
						if (SplitMode == 0) {
							// TODO: Split texture into Texture2DArray
							ShowTextureSplitDialogue = false;
						} else if (SplitMode == 1) {
							// TODO: Split texture into Texture3D
							ShowTextureSplitDialogue = false;
						} else {
							MessageBox("Error!", "Failed to split texture: Split mode is out of range!",
									   MessageBoxType::Error);
						}
					}
				}
				ImGui::End();
			}
		}
		ImGui::EndFrame();

		if (SPRITE.mesh.vbo != 0) {
			auto size = Vec2FromVec2i(GctkGetWindowSize());
			TRANSFORM.position = Vec2Add(Vec2Sub(Vec2DivF(size, 2.0f), Vec2DivF(TRANSFORM.scale, 2.0f)),
										 VEC2(size.x * 0.25f - TRANSFORM.scale.x * 0.5f, 0.0f));
			GctkRenderEnqueueSprite2DNoTint(&SPRITE, TRANSFORM);
		}
	});
	GctkSetPreRenderCallback([](double delta) {
		ImGui::Render();
	});
	GctkSetPostRenderCallback([](double delta) {
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	});

	while (GctkUpdate()) { }

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	if (SPRITE.mesh.vbo != 0) {
		GctkDeleteSprite(&SPRITE);
	}
	if (TEXTURE.id != 0) {
		GctkDeleteTexture(&TEXTURE);
	}

	GctkDispose();

	return 0;
}