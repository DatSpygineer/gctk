#include "gctk/gctk.h"

#include <string>
#include <optional>
#include <vector>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_DXT_IMPLEMENTATION
#include "stb_dxt.h"

struct FileFilter {
	std::string name;
	std::string extension;
};

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
enum class MessageBoxResult {
	Invalid,
	Ok,
	Cancel,
	Yes,
	No
};

enum class FileFormat {
	Png,
	Jpg,
	Bmp,
	Tga,
	Dds,
	Hdr,
	GTex
};

#ifdef _WIN32

#include <windows.h>
#include <commdlg.h>
#include <stdexcept>

static std::optional<std::string> OpenFileDiag(const char* filters) {
	OPENFILENAMEA ofn;
	char filePath[MAX_PATH] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = MAX_PATH;

	ofn.lpstrFilter = filters;
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = nullptr;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn) == TRUE) {
		return std::string { ofn.lpstrFile };
	}

	return { };
}
static std::optional<std::string> SaveFileDiag(const char* filters) {
	OPENFILENAMEA ofn;
	char filePath[MAX_PATH] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = MAX_PATH;

	ofn.lpstrFilter = filters;
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = nullptr;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

	if (GetSaveFileNameA(&ofn) == TRUE) {
		return std::string { ofn.lpstrFile };
	}

	return { };
}

static MessageBoxResult ShowMessageBox(const std::string& title, const std::string& message,
									   MessageBoxType type = MessageBoxType::Info,
									   MessageBoxButtons buttons = MessageBoxButtons::Ok) {
	int flags = 0;
	switch (type) {
		case MessageBoxType::Info:     flags = MB_ICONINFORMATION; break;
		case MessageBoxType::Warning:  flags = MB_ICONWARNING; break;
		case MessageBoxType::Error:    flags = MB_ICONERROR; break;
		case MessageBoxType::Question: flags = MB_ICONQUESTION; break;
	}
	switch (buttons) {
		case MessageBoxButtons::Ok: flags |= MB_OK; break;
		case MessageBoxButtons::OkCancel: flags |= MB_OKCANCEL; break;
		case MessageBoxButtons::YesNo: flags |= MB_YESNO; break;
		case MessageBoxButtons::YesNoCancel: flags |= MB_YESNOCANCEL; break;
	}

	switch (MessageBoxA(nullptr, message.c_str(), title.c_str(), flags)) {
		case IDOK: return MessageBoxResult::Ok;
		case IDCANCEL: return MessageBoxResult::Cancel;
		case IDYES: return MessageBoxResult::Yes;
		case IDNO: return MessageBoxResult::No;
		default: return MessageBoxResult::Invalid;
	}
}

#else
	#include <gtk/gtk.h>

static std::optional<std::string> OpenFileDiag(const std::initializer_list<FileFilter>& filters) {
	return { };
}
static std::optional<std::string> SaveFileDiag(const std::initializer_list<FileFilter>& filters) {
	return { };
}

static MessageBoxResult ShowMessageBox(const std::string& title, const std::string& message,
									   MessageBoxType type = MessageBoxType::Info,
									   MessageBoxButtons buttons = MessageBoxButtons::Ok) {
	GtkMessageType gtkType;
	GtkButtonsType gtkButtons;

	// Map MessageBoxType to GtkMessageType
	switch (type) {
		case MessageBoxType::Info:
			gtkType = GTK_MESSAGE_INFO;
			break;
		case MessageBoxType::Warning:
			gtkType = GTK_MESSAGE_WARNING;
			break;
		case MessageBoxType::Error:
			gtkType = GTK_MESSAGE_ERROR;
			break;
		case MessageBoxType::Question:
			gtkType = GTK_MESSAGE_QUESTION;
			break;
	}

	// Map MessageBoxButtons to GtkButtonsType
	switch (buttons) {
		case MessageBoxButtons::Ok:
			gtkButtons = GTK_BUTTONS_OK;
			break;
		case MessageBoxButtons::OkCancel:
			gtkButtons = GTK_BUTTONS_OK_CANCEL;
			break;
		case MessageBoxButtons::YesNo:
			gtkButtons = GTK_BUTTONS_YES_NO;
			break;
		case MessageBoxButtons::YesNoCancel:
			gtkButtons = GTK_BUTTONS_NONE; // Will add buttons manually
			break;
	}

	// Create the message dialog
	GtkWidget* dialog = gtk_message_dialog_new(NULL,
											   GTK_DIALOG_MODAL,
											   gtkType,
											   gtkButtons,
											   "%s",
											   message.c_str());
	gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());

	// Add Yes/No/Cancel buttons manually if needed
	if (buttons == MessageBoxButtons::YesNoCancel) {
		gtk_dialog_add_buttons(GTK_DIALOG(dialog),
							   "_Yes", GTK_RESPONSE_YES,
							   "_No", GTK_RESPONSE_NO,
							   "_Cancel", GTK_RESPONSE_CANCEL,
							   NULL);
	}

	// Run the dialog and get the response
	gint response = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	// Map GTK response to MessageBoxResult
	switch (response) {
		case GTK_RESPONSE_OK:
			return MessageBoxResult::Ok;
		case GTK_RESPONSE_CANCEL:
			return MessageBoxResult::Cancel;
		case GTK_RESPONSE_YES:
			return MessageBoxResult::Yes;
		case GTK_RESPONSE_NO:
			return MessageBoxResult::No;
		default:
			return MessageBoxResult::Invalid;
	}
}

#endif

static ImGuiIO* io;

static Texture TEXTURE = { 0 };
static Transform2D TRANSFORM = { .position = VEC2(0, 0), .scale = VEC2(100, 100) };
static Sprite SPRITE;

static bool ClampR = false, ClampS = false, ClampT = false;
static bool PointFilter = true, Mipmaps = false;
static bool PointFilterPrev = false, MipmapsPrev = false;
static int MipmapLevel = 0, MipmapLevelPrev = 0;
static float ZoomLevel = 1;
static bool EnableAlpha = true;
static bool EnableAlphaPrev = true;
static bool Panning = false;

static const char* FORMAT_LIST[] = {
	"Grayscale",
	"Rgb",
	"Indexed8",
	"Indexed16"
};
static const char* TARGET_LIST[] = {
	"1D", "2D", "3D", "Cubemap"
};

static int FORMAT_INDEX = 1;
static int TARGET_INDEX = 1;

static Vec2 Offset = VEC2_ZERO;
static double mouse_xprev = 0, mouse_yprev = 0;

static void update(double delta) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImVec2 menu_size;
	ImGui::BeginMainMenuBar();
		menu_size = ImGui::GetWindowSize();
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open texture..")) {
				if (auto path = OpenFileDiag("GCTk Texture\0*.gtex\0"); path.has_value()) {
					if (!GctkLoadTextureFromFile(&TEXTURE, path->c_str())) {
						ShowMessageBox("Failure", "Failed to load texture file \"" + path.value() + "\"", MessageBoxType::Error);
					}

					PointFilter = false;
					Mipmaps = false;
					MipmapLevel = 0;
					FORMAT_INDEX = 1;
					TARGET_INDEX = 1;
					ClampR = ClampS = ClampT = false;
					EnableAlpha = true;
				}
			}
			if (ImGui::MenuItem("Save texture")) {

			}
			if (ImGui::MenuItem("Save texture as..")) {

			}
			if (ImGui::MenuItem("Import image..")) {
				if (auto path = OpenFileDiag("All files\0*.*\0Png file\0*.png\0Jpeg file\0*.jpg\0Bitmap file\0*.bmp\0DDS file\0*.dds\0Targa file\0*.tga\0"); path.has_value()) {
					if (!GctkLoadImageFromFile(&TEXTURE, path->c_str(), GCTK_IMAGE_FLAG_DEFAULT)) {
						ShowMessageBox("Failure", "Failed to load texture file \"" + path.value() + "\"", MessageBoxType::Error);
					}

					PointFilter = false;
					Mipmaps = false;
					FORMAT_INDEX = 1;
					TARGET_INDEX = 1;
				}
			}
			if (ImGui::MenuItem("Export image..")) {

			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) {
				GctkClose();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) {
			if (ImGui::MenuItem("Reset zoom")) {
				ZoomLevel = 1.0f;
			}
			if (ImGui::MenuItem("Reset position")) {
				Offset = VEC2_ZERO;
			}
			ImGui::EndMenu();
		}
	ImGui::EndMainMenuBar();

	int display_w, display_h;
	glfwGetFramebufferSize((GLFWwindow*)GctkGetWindowHandle(), &display_w, &display_h);
	ImGui::SetNextWindowPos(ImVec2 { 0, menu_size.y });
	ImGui::SetNextWindowSize(ImVec2 { static_cast<float>(display_w) * 0.25f, static_cast<float>(display_h) });
	ImGui::Begin("Image Settings", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::Text("Texture wrapping:");
		ImGui::Checkbox("Clamp R", &ClampR); ImGui::SameLine();
		ImGui::Checkbox("Clamp S", &ClampS); ImGui::SameLine();
		ImGui::Checkbox("Clamp T", &ClampT);
		ImGui::Separator();
		ImGui::Text("Texture Parameters:");
		ImGui::Checkbox("Nearest filter", &PointFilter); ImGui::SameLine();
		if (PointFilterPrev != PointFilter) {
			GctkBindTexture(&TEXTURE);
			glTexParameteri(GctkGetGLTarget(TEXTURE.target), GL_TEXTURE_MAG_FILTER, PointFilter ? GL_NEAREST : GL_LINEAR);
			glTexParameteri(GctkGetGLTarget(TEXTURE.target), GL_TEXTURE_MIN_FILTER,
							Mipmaps ?
							(PointFilter ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR) :
							(PointFilter ? GL_NEAREST : GL_LINEAR)
			);
			glBindTexture(GctkGetGLTarget(TEXTURE.target), 0);
			PointFilterPrev = PointFilter;
		}
		ImGui::Checkbox("Generate MipMaps", &Mipmaps);
		if (Mipmaps != MipmapsPrev) {
			GctkBindTexture(&TEXTURE);
			glTexParameteri(GctkGetGLTarget(TEXTURE.target), GL_TEXTURE_MAG_FILTER, PointFilter ? GL_NEAREST : GL_LINEAR);
			glTexParameteri(GctkGetGLTarget(TEXTURE.target), GL_TEXTURE_MIN_FILTER,
							Mipmaps ?
							(PointFilter ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR) :
							(PointFilter ? GL_NEAREST : GL_LINEAR)
			);
			glGenerateMipmap(GctkGetGLTarget(TEXTURE.target));
			glBindTexture(GctkGetGLTarget(TEXTURE.target), 0);
			MipmapsPrev = Mipmaps;
		}
		ImGui::SliderInt("Mipmap preview level", &MipmapLevel, 0, 10);
		if (MipmapLevel != MipmapLevelPrev) {
			GctkBindTexture(&TEXTURE);
			glTexParameteri(GctkGetGLTarget(TEXTURE.target), GL_TEXTURE_BASE_LEVEL, MipmapLevel);
			glBindTexture(GctkGetGLTarget(TEXTURE.target), 0);
			MipmapLevelPrev = MipmapLevel;
		}
		ImGui::Separator();
		ImGui::Text("Texture format:");
		if (ImGui::BeginCombo("Format", FORMAT_LIST[FORMAT_INDEX])) {
			for (int i = 0; i < 4; i++) {
				auto selected = FORMAT_INDEX == i;
				if (ImGui::Selectable(FORMAT_LIST[i], selected)) {
					FORMAT_INDEX = i;
				}

				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Checkbox("Use Alpha", &EnableAlpha);
		if (EnableAlpha != EnableAlphaPrev) {
			if (EnableAlphaPrev) {
				glDisable(GL_BLEND);
			} else {
				glEnable(GL_BLEND);
			}
			EnableAlphaPrev = EnableAlpha;
		}

		if (ImGui::BeginCombo("Target", TARGET_LIST[TARGET_INDEX])) {
			for (int i = 0; i < 4; i++) {
				auto selected = TARGET_INDEX == i;
				if (ImGui::Selectable(TARGET_LIST[i], selected)) {
					TARGET_INDEX = i;
				}

				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	ImGui::End();
	ImGui::SetNextWindowPos(ImVec2 { static_cast<float>(display_w) - 64 , menu_size.y + 16 });
	ImGui::SetNextWindowSize(ImVec2 { 32, static_cast<float>(display_h) - menu_size.y - 32 });
	ImGui::Begin("Zoom slider", nullptr,	ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
											ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
											ImGuiWindowFlags_NoBackground);
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 24.0f) * 0.5f);
	ImGui::VSliderFloat("Zoom", ImVec2 { 24, static_cast<float>(display_h) - menu_size.y - 32 }, &ZoomLevel, 1.0f, 10.0f);
	ImGui::End();

	Vec2i size = GctkGetWindowSize();
	TRANSFORM.scale = VEC2(100.0f * ZoomLevel, 100.0f * ZoomLevel);
	TRANSFORM.position = Vec2Add(VEC2(((float)size.x) * 0.5f - TRANSFORM.scale.x * 0.5f + size.x * 0.125f,
							  ((float)size.y) * 0.5f - TRANSFORM.scale.y * 0.5f), Offset);
	GctkRenderEnqueueSprite2D(&SPRITE, COLOR(1.0, 1.0, 1.0, 1.0), TRANSFORM);
}
static void post_render(double delta) {
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
static void pre_render(double delta) {
	ImGui::Render();
}

static void write_texture(const std::string& target_path, FileFormat file_format) {
	if (TEXTURE.id == 0) {
		ShowMessageBox("Failed to write texture!", "Could not get texture data: No texture is loaded!",
					   MessageBoxType::Error);
	} else {
		GLenum format;
		int stride = 0;
		switch (FORMAT_INDEX) {
			case 0: { // Grayscale
				format = EnableAlpha ? GL_RG : GL_R;
				stride = EnableAlpha ? 2 : 1;
			} break;
			default: { // Anything else
				format = EnableAlpha ? GL_RGBA : GL_RGB;
				stride = EnableAlpha ? 4 : 3;
			} break;
		}

		GctkBindTexture(&TEXTURE);
		GLenum target = GctkGetGLTarget(TEXTURE.target);

		int w, h, d;
		glGetTexParameteriv(target, GL_TEXTURE_WIDTH, &w);
		glGetTexParameteriv(target, GL_TEXTURE_HEIGHT, &h);
		glGetTexParameteriv(target, GL_TEXTURE_DEPTH, &d);

		if (w <= 0) {
			w = 1;
		}
		if (h <= 0) {
			h = 1;
		}
		if (d <= 0) {
			d = 1;
		}

		std::vector<uint8_t> result;
		result.reserve(stride * w * h * d);
		glGetTexImage(target, 0, format, GL_UNSIGNED_BYTE, result.data());
		glBindTexture(TEXTURE.target, 0);

		switch (file_format) {
			case FileFormat::Png: {
				stbi_write_png(target_path.c_str(), w * d, h * d, stride, result.data(), w * d * stride);
			} break;
			case FileFormat::Jpg: {
				stbi_write_jpg(target_path.c_str(), w * d, h * d, stride, result.data(), 100);
			} break;
			case FileFormat::Bmp: {
				stbi_write_bmp(target_path.c_str(), w * d, h * d, stride, result.data());
			} break;
			case FileFormat::Hdr: {
				stbi_write_hdr(target_path.c_str(), w * d, h * d, stride, reinterpret_cast<const float*>(result.data()));
			} break;
			case FileFormat::Dds: {
				// Not supported
				throw std::runtime_error("DDS format is not implemented!");
			} break;
			case FileFormat::Tga: {
				stbi_write_tga(target_path.c_str(), w * d, h * d, stride, result.data());
			} break;
			case FileFormat::GTex: {
				// Indexed8/16
				if (FORMAT_INDEX == 3 || FORMAT_INDEX == 4) {

					// Raw and compressed format
				} else {

				}
			} break;
		}
	}
}

int main(int argc, char** argv) {
	if (!GctkInit(argc, argv, "GTexTools", "Gctk", VERSION(1, 0, 0, GCTK_VERSION_RELEASE))) {
		return 1;
	}

#ifndef _WIN32
	gtk_init(&argc, &argv);
#endif

	GctkSetBackgroundColor(COLOR_NO_ALPHA(0.25f, 0.25f, 0.25f));

	GctkLoadImageFromFile(&TEXTURE, "test.png", GCTK_IMAGE_FLAG_POINT_FILTER);
	GctkCreateSprite(&SPRITE, GctkSprite2DDefaultShader(), &TEXTURE);

	glfwSetScrollCallback((GLFWwindow*)GctkGetWindowHandle(), [](GLFWwindow* window, double xoffset, double yoffset) {
		if (yoffset != 0) {
			ZoomLevel = GctkClamp(ZoomLevel + (float)yoffset * 0.1f, 1.0f, 10.0f);
		}
	});
	glfwSetMouseButtonCallback((GLFWwindow*)GctkGetWindowHandle(), [](GLFWwindow* window, int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			Panning = action == GLFW_PRESS;
		}
	});
	glfwSetCursorPosCallback((GLFWwindow*)GctkGetWindowHandle(), [](GLFWwindow* window, double xpos, double ypos) {
		if (Panning) {
			Offset = Vec2Sub(Offset, VEC2(static_cast<float>(mouse_xprev - xpos), static_cast<float>(ypos - mouse_yprev)));
		}
		mouse_xprev = xpos;
		mouse_yprev = ypos;
	});

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)GctkGetWindowHandle(), true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	GctkSetUpdateCallback(&update);
	GctkSetPreRenderCallback(&pre_render);
	GctkSetPostRenderCallback(&post_render);
	while (GctkUpdate());

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	GctkDispose();

	GctkDeleteTexture(&TEXTURE);
	GctkDeleteSprite(&SPRITE);

#ifndef _WIN32
	gtk_main_quit();
#endif

	return 0;
}