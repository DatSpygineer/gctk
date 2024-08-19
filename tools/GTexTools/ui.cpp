#include <fstream>

#include <gctk/gctk.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>

static ImGuiIO* io;

static bool ClampR_Prev = false, ClampS_Prev = false, ClampT_Prev = false;
static bool ClampR = false, ClampS = false, ClampT = false;
static bool PointFiler_Prev = false;
static bool PointFiler = false;
static bool MipMaps_Prev = false;
static bool MipMaps = false;
static int MipmapPreview_Prev = 0;
static int MipmapPreview = 0, MipmapPreviewMax = 10;

static bool Changes = false;

static Texture TEXTURE;
static Sprite SPRITE;
static Transform2D TRANSFORM = {
	.position = VEC2_ZERO,
	.scale = VEC2(100.0f, 100.0f)
};

static std::vector<uint8_t> ImageData;
static std::vector<std::filesystem::path> RecentFiles;

static void AddRecentFile(const std::filesystem::path& path) {
	if (auto it = std::ranges::find(RecentFiles, path); it != RecentFiles.end()) {
		RecentFiles.erase(it);
	}

	if (RecentFiles.size() > 4) {
		RecentFiles.pop_back();
	}
	RecentFiles.insert(RecentFiles.begin(), path);
}

extern bool LoadTexture(const std::filesystem::path& path, Texture& texture, std::vector<uint8_t>& data);
extern bool SaveTexture(const std::filesystem::path& path, const Texture& texture, std::vector<uint8_t>& data);

static void SaveDialog() {
	auto path = QFileDialog::getSaveFileName(nullptr, "Open image/texture file", QDir::homePath(),
		"Supported files (*.gtex *.png *.tga *.jpg *.jpeg *.bmp);;GTex format (*.gtex);;Image formats (*.png *.tga *.jpg *.jpeg *.bmp);;All files (*.*)"
	);
	if (!path.isEmpty()) {
		TEXTURE.clamp_r = ClampR;
		TEXTURE.clamp_s = ClampS;
		TEXTURE.clamp_t = ClampT;
		TEXTURE.point_filter = PointFiler;
		TEXTURE.mipmaps = MipMaps;

		if (SaveTexture(path.toStdString(), TEXTURE, ImageData)) {
			Changes = false;
		} else {
			QMessageBox msg;
			msg.setText(QString("Failed to save texture \"%1\"").arg(path));
		}
	}
}
static void LoadDialog() {
	if (Changes) {
		QMessageBox msg;
		msg.setText("Would you like to save changes before opening a new?");
		msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msg.setDefaultButton(QMessageBox::Save);
		switch (msg.exec()) {
			case QMessageBox::Save: {
				SaveDialog();
			} break;
			case QMessageBox::Discard: /* Continue */ break;
			default: return;
		}
	}

	char basedir[GCTK_PATH_MAX];
	GctkGetBaseDirectory(basedir);
	auto path = QFileDialog::getOpenFileName(nullptr, "Open image/texture file", QString(basedir),
		"Supported files (*.gtex *.png *.tga *.jpg *.jpeg *.bmp);;GTex format (*.gtex);;Image formats (*.png *.tga *.jpg *.jpeg *.bmp);;All files (*.*)"
	);
	if (!path.isEmpty()) {
		if (TEXTURE.id != 0) {
			GctkDeleteTexture(&TEXTURE);
		}

		if (!LoadTexture(path.toStdString(), TEXTURE, ImageData)) {
			QMessageBox msg;
			msg.setText(QString("Failed to load texture \"%1\"").arg(path));
		} else {
			ClampR = TEXTURE.clamp_r;
			ClampS = TEXTURE.clamp_s;
			ClampT = TEXTURE.clamp_t;
			PointFiler = TEXTURE.point_filter;
			MipMaps = TEXTURE.mipmaps;
			MipmapPreview = 0;
			AddRecentFile(path.toStdString());
		}

		if (!GctkIsSpriteValid(&SPRITE)) {
			GctkCreateSprite(&SPRITE, GctkSprite2DDefaultShader(), &TEXTURE);
		}

		Changes = false;
	}
}

void InitUI() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(GctkGetWindowHandle()), true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	GctkSetClosingCallback([] {
		if (Changes && TEXTURE.id != 0) {
			QMessageBox msg;
			msg.setText("Would you like to save changes before exiting?");
			msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
			msg.setDefaultButton(QMessageBox::Save);
			switch (msg.exec()) {
				case QMessageBox::Save: {
					SaveDialog();
					return true;
				}
				case QMessageBox::Discard: return true;
				default: return false;
			}
		}
		return true;
	});

	if (std::ifstream ifs("recent.txt"); ifs.is_open()) {
		std::string line;
		while (std::getline(ifs, line, '\n')) {
			AddRecentFile(line);
		}
	}
}

void UpdateUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImVec2 menu_size = { };
	if (ImGui::BeginMainMenuBar()) {
		menu_size = ImGui::GetWindowSize();
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open..", "Ctrl+O")) {
				LoadDialog();
			}

			ImGui::BeginDisabled(RecentFiles.empty());
			if (ImGui::BeginMenu("Open recent")) {
				for (const auto& recent : RecentFiles) {
					if (ImGui::MenuItem(recent.c_str())) {
						if (TEXTURE.id != 0) {
							GctkDeleteTexture(&TEXTURE);
						}

						if (!LoadTexture(recent, TEXTURE, ImageData)) {
							QMessageBox msg;
							msg.setText(QString("Failed to load texture \"%1\"").arg(recent.c_str()));
						} else {
							ClampR = TEXTURE.clamp_r;
							ClampS = TEXTURE.clamp_s;
							ClampT = TEXTURE.clamp_t;
							PointFiler = TEXTURE.point_filter;
							MipMaps = TEXTURE.mipmaps;
							MipmapPreview = 0;
						}

						if (!GctkIsSpriteValid(&SPRITE)) {
							GctkCreateSprite(&SPRITE, GctkSprite2DDefaultShader(), &TEXTURE);
						}
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndDisabled();

			if (ImGui::MenuItem("Save..", "Ctrl+S", false, TEXTURE.id != 0)) {
				SaveDialog();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "Alt+F4")) {
				GctkClose();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	auto win_size = Vec2FromVec2i(GctkGetWindowSize());

	if (GctkIsSpriteValid(&SPRITE)) {
		TRANSFORM.position.x = win_size.x / 4.0f + 3.0f * win_size.x / 8.0f - static_cast<float>(TEXTURE.width) / 2.0f;
		TRANSFORM.position.y = win_size.y / 2.0f + static_cast<float>(TEXTURE.height) / 2.0f;
	}

	ImGui::SetNextWindowPos(ImVec2(0, menu_size.y));
	ImGui::SetNextWindowSize(ImVec2(win_size.x / 4, win_size.y - menu_size.y));
	if (ImGui::Begin("Texture properties", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
		ImGui::SeparatorText("Texture Properties");

		ImGui::BeginDisabled(TEXTURE.id == 0);
		ImGui::Checkbox("Clamp R", &ClampR); ImGui::SameLine();
		ImGui::Checkbox("Clamp S", &ClampS); ImGui::SameLine();
		ImGui::Checkbox("Clamp T", &ClampT);

		ImGui::Spacing();

		ImGui::Checkbox("Point Filter", &PointFiler);

		ImGui::Spacing();

		ImGui::Checkbox("Generate Mipmaps", &MipMaps);

		ImGui::BeginDisabled(!MipMaps);
			ImGui::SliderInt("Preview", &MipmapPreview, 0, MipmapPreviewMax);
		ImGui::EndDisabled();

		ImGui::EndDisabled();

		ImGui::Spacing();
		ImGui::SeparatorText("Texture Info");

		ImGui::Text("Texture size: %g, %g", 0.0, 0.0);
		ImGui::Text("Texture palette size: %d", 0);
		ImGui::Text("Texture format: %s", "RGB");
		ImGui::Text("Texture target: %s", "2D");
		ImGui::Text("Has alpha: %s", "Yes");

		ImGui::End();
	}

	if (ClampR != ClampR_Prev) {
		Changes = true;
		if (TEXTURE.id) {
			glBindTexture(GctkGetGLTarget(TEXTURE.target), TEXTURE.id);
			glTexParameteri(GctkGetGLTarget(TEXTURE.target), GL_TEXTURE_WRAP_R, ClampR ? GL_CLAMP : GL_REPEAT);
			glBindTexture(GctkGetGLTarget(TEXTURE.target), 0);
		}
		ClampR_Prev = ClampR;
	}
	if (ClampS != ClampS_Prev) {
		Changes = true;
		if (TEXTURE.id) {
			glBindTexture(GctkGetGLTarget(TEXTURE.target), TEXTURE.id);
			glTexParameteri(GctkGetGLTarget(TEXTURE.target), GL_TEXTURE_WRAP_S, ClampS ? GL_CLAMP : GL_REPEAT);
			glBindTexture(GctkGetGLTarget(TEXTURE.target), 0);
		}
		ClampS_Prev = ClampS;
	}
	if (ClampT != ClampT_Prev) {
		Changes = true;
		if (TEXTURE.id) {
			glBindTexture(GctkGetGLTarget(TEXTURE.target), TEXTURE.id);
			glTexParameteri(GctkGetGLTarget(TEXTURE.target), GL_TEXTURE_WRAP_T, ClampT ? GL_CLAMP : GL_REPEAT);
			glBindTexture(GctkGetGLTarget(TEXTURE.target), 0);
		}
		ClampT_Prev = ClampT;
	}
	if (PointFiler != PointFiler_Prev) {
		Changes = true;
		if (TEXTURE.id) {
			glBindTexture(GctkGetGLTarget(TEXTURE.target), TEXTURE.id);
			glTexParameteri(GctkGetGLTarget(TEXTURE.target), GL_TEXTURE_MAG_FILTER, PointFiler ? GL_NEAREST : GL_LINEAR);
			glTexParameteri(GctkGetGLTarget(TEXTURE.target), GL_TEXTURE_MIN_FILTER,
				MipMaps ?
				(PointFiler ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR) :
				(PointFiler ? GL_NEAREST : GL_LINEAR)
			);
			glBindTexture(GctkGetGLTarget(TEXTURE.target), 0);
		}
		PointFiler_Prev = PointFiler;
	}
	if (MipMaps != MipMaps_Prev) {
		Changes = true;
		if (TEXTURE.id) {
			glBindTexture(GctkGetGLTarget(TEXTURE.target), TEXTURE.id);
			glTexParameteri(GctkGetGLTarget(TEXTURE.target), GL_TEXTURE_MIN_FILTER,
				MipMaps ?
				(PointFiler ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR) :
				(PointFiler ? GL_NEAREST : GL_LINEAR)
			);
			glGenerateMipmap(GctkGetGLTarget(TEXTURE.target));
			glGetTextureParameteriv(GctkGetGLTarget(TEXTURE.target), GL_TEXTURE_MAX_LEVEL, &MipmapPreviewMax);
			glBindTexture(GctkGetGLTarget(TEXTURE.target), 0);
		}
		MipMaps_Prev = MipMaps;
	}
	if (MipmapPreview != MipmapPreview_Prev) {
		glBindTexture(GctkGetGLTarget(TEXTURE.target), TEXTURE.id);
		glTexParameteri(GctkGetGLTarget(TEXTURE.target), GL_TEXTURE_BASE_LEVEL, MipmapPreview);
		glBindTexture(GctkGetGLTarget(TEXTURE.target), 0);
	}

	if (GctkIsSpriteValid(&SPRITE)) {
		GctkRenderEnqueueSprite2D(&SPRITE, COLOR_WHITE, TRANSFORM);
	}
}

void DisposeUI() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	if (RecentFiles.empty()) {
		std::ofstream ofs("recent.txt");
		if (ofs.is_open()) {
			for (const auto& recent : RecentFiles) {
				ofs << recent << std::endl;
			}
		}
		ofs.flush();
		ofs.close();
	}
}