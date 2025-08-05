#include <GL/glew.h>

#include "gctk.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace gctk {
	static bool UpdateWindowSize(const CVar* self, const std::string& value);
	static bool UpdateWindowState(const CVar* self, const std::string& value);
	static bool UpdateSwapInterval(const CVar* self, const std::string& value);
	static GLFWmonitor* GetMonitorFromIndex(int idx);

	CVar vid_window_width("vid_window_width", "1024", CVAR_FLAG_USER_DATA, &UpdateWindowSize);
	CVar vid_window_height("vid_window_height", "768", CVAR_FLAG_USER_DATA, &UpdateWindowSize);
	CVar vid_fullscreen("vid_fullscreen", "false", CVAR_FLAG_USER_DATA, &UpdateWindowState);
	CVar vid_monitor("vid_monitor", "-1", CVAR_FLAG_USER_DATA, &UpdateWindowState);
	CVar vid_vsync("vid_vsync", "false", CVAR_FLAG_USER_DATA, &UpdateSwapInterval);

	static Client* s_client_instance = nullptr;

	Client::Client(const int argc, char** argv, const std::string& name,
			const std::vector<std::string>& asset_packs,
			const std::optional<Path>& mod_path) :
		m_pWindow(nullptr), m_bGlfwInitialized(false), m_pIconImage(nullptr), m_sName(name) {
		if (s_client_instance != nullptr) {
			FatalError("Client already running!");
		}

		Paths::Initialize(argv[0], name);
		AssetSystem::Initialize(asset_packs, mod_path);
		Console::LoadConfig("autoexec.cfg");
		Console::LoadConfig("userdata.cfg");

		s_client_instance = this;
	}

	Client::~Client() {
		Console::StoreUserData();
		Input::SaveInputs();

		if (m_pIconImage != nullptr && m_pIconImage->pixels != nullptr) {
			stbi_image_free(m_pIconImage->pixels);
			delete m_pIconImage;
		}

		if (m_pWindow != nullptr) {
			glfwDestroyWindow(m_pWindow);
		}
		if (m_bGlfwInitialized) {
			glfwTerminate();
		}

		if (s_client_instance == this) {
			s_client_instance = nullptr;
		}
		LogInfo("Client closed");
		CloseDebugLog();
	}

	void Client::init() {
		LogInfo("Initializing client...");

		Console::LoadConfig("keybinds.cfg");

		if (glfwInit() == GLFW_FALSE) {
			const char* errmsg;
			const int errcode = glfwGetError(&errmsg);
			FatalError(std::format("Failed to initialize GLFW:\n{} (error code {})", errmsg, errcode));
		}
		m_bGlfwInitialized = true;

		GLFWmonitor* monitor = nullptr;
		if (vid_fullscreen.get_boolean()) {
			const int idx = vid_monitor.get_integer();
			if (idx < 0) {
				monitor = glfwGetPrimaryMonitor();
			} else {
				int monitor_count;
				GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
				if (monitor_count <= idx) {
					monitor = glfwGetPrimaryMonitor();
					vid_monitor.set_value(-1);
				} else {
					monitor = monitors[idx];
				}
			}
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		m_pWindow = glfwCreateWindow(
			vid_window_width.get_integer(), vid_window_height.get_integer(),
			m_sName.c_str(), monitor, nullptr
		);
		if (m_pWindow == nullptr) {
			const char* errmsg;
			const int errcode = glfwGetError(&errmsg);
			FatalError(std::format("Failed to create window: \n{} (error code {})", errmsg, errcode));
		}
		glfwMakeContextCurrent(m_pWindow);

		if (auto glew_error = glewInit(); glew_error != GLEW_OK) {
			FatalError(std::format("Failed to initialize OpenGL: {} (error code {})",
				reinterpret_cast<const char*>(glewGetErrorString(glew_error)),
				glew_error
			));
		}

#ifndef NDEBUG
		set_window_title(m_sName);
#endif

		if (Paths::exists(Paths::ResourcePath() / "game_icon.png")) {
			int icon_x, icon_y;
			const auto icon_data = stbi_load((Paths::ResourcePath() / "game_icon.png").string().c_str(), &icon_x, &icon_y, nullptr, 4);

			if (icon_data != nullptr) {
				m_pIconImage = new GLFWimage;
				m_pIconImage->width = icon_x;
				m_pIconImage->height = icon_y;
				m_pIconImage->pixels = icon_data;
				glfwSetWindowIcon(m_pWindow, 1, m_pIconImage);
				LogInfo(std::format("Loaded icon \"{}\"", (Paths::ResourcePath() / "game_icon.png")));
			} else {
				LogErr(std::format("Failed to load window icon \"{}\"", (Paths::ResourcePath() / "game_icon.png")));
			}
		} else {
			LogWarn(std::format("Window icon \"{}\" cannot be found!", (Paths::ResourcePath() / "game_icon.png")));
		}

		Input::Initialize(*this);
		Time::Initialize();
		LogInfo("Client initialized successfully");
	}

	void Client::update() {
		glfwPollEvents();
		Input::Poll();
	}

	void Client::render() {
		glClearColor(m_cBackgroundColor.r, m_cBackgroundColor.g, m_cBackgroundColor.b, m_cBackgroundColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render code goes here

		glfwSwapBuffers(m_pWindow);
	}

	bool Client::should_exit() const {
		return m_pWindow != nullptr && glfwWindowShouldClose(m_pWindow);
	}

	void Client::set_window_title(const std::string& title) const {
#ifndef NDEBUG
		glfwSetWindowTitle(m_pWindow, std::format("{} | DEBUG | GCTk v{} | OpenGL v{}",
			title,
			GCTK_VERSION_STRING,
			reinterpret_cast<const char*>(glGetString(GL_VERSION))
		).c_str());
#else
		glfwSetWindowTitle(m_pWindow, title.c_str());
#endif
	}
	std::string Client::get_window_title() const {
#ifndef NDEBUG
		std::string title = glfwGetWindowTitle(m_pWindow);
		if (const auto idx = title.find('|'); idx != std::string::npos) {
			return StringUtil::Trim(title.substr(0, idx));
		}
		return title;
#else
		return glfwGetWindowTitle(m_pWindow);
#endif
	}
	void Client::set_window_location(const int x, const int y) const {
		glfwSetWindowPos(m_pWindow, x, y);
	}
	std::tuple<int, int> Client::get_window_location() const {
		int x, y;
		glfwGetWindowPos(m_pWindow, &x, &y);
		return std::make_tuple(x, y);
	}
	void Client::set_window_size(const int width, const int height) const {
		glfwSetWindowSize(m_pWindow, width, height);
	}
	std::tuple<int, int> Client::get_window_size() const {
		int w, h;
		glfwGetWindowSize(m_pWindow, &w, &h);
		return std::make_tuple(w, h);
	}
	void Client::set_window_fullscreen(const bool fullscreen) const {
		set_window_fullscreen(fullscreen, vid_monitor.get_integer());
	}
	void Client::set_window_fullscreen(const bool fullscreen, const int monitor_idx) const {
		GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;
		if (fullscreen && monitor_idx >= 0) {
			int monitor_count;
			GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
			if (monitor_idx < monitor_count) {
				monitor = monitors[monitor_idx];
			} else {
				monitor = glfwGetPrimaryMonitor();
			}
		}
		set_window_fullscreen(fullscreen, monitor);
	}
	void Client::set_window_fullscreen(const bool fullscreen, GLFWmonitor* monitor_ptr) const {
		set_window_monitor(fullscreen ? monitor_ptr : nullptr);
	}
	bool Client::get_window_fullscreen() const {
		return glfwGetWindowMonitor(m_pWindow) != nullptr;
	}
	void Client::set_window_monitor(const int idx) const {
		if (GLFWmonitor* monitor = GetMonitorFromIndex(idx); monitor != nullptr) {
			set_window_monitor(monitor);
		}
	}
	void Client::set_window_monitor(GLFWmonitor* monitor) const {
		int monitor_x, monitor_y, monitor_w, monitor_h;
		GLFWmonitor* target_monitor = monitor;
		if (target_monitor == nullptr) {
			target_monitor = GetMonitorFromIndex(vid_monitor.get_integer());
			if (target_monitor == nullptr) {
				target_monitor = glfwGetPrimaryMonitor();
			}
		}
		glfwGetMonitorWorkarea(target_monitor, &monitor_x, &monitor_y,  &monitor_w, &monitor_h);
		glfwSetWindowMonitor(m_pWindow, monitor,
			monitor_x, monitor_y,
			monitor_w, monitor_h,
			GLFW_DONT_CARE
		);
	}
	GLFWmonitor* Client::get_window_monitor() const {
		return glfwGetWindowMonitor(m_pWindow);
	}

	void Client::set_background_color(const Color& color) {
		m_cBackgroundColor = color;
	}
	Color Client::get_background_color() const {
		return m_cBackgroundColor;
	}

	Client* Client::Instance() {
		return s_client_instance;
	}

	static bool UpdateWindowSize(const CVar* self, const std::string& value) {
		try {
			const uint32_t size = std::stoul(value);
			if (size == 0) {
				return false;
			}

			if (s_client_instance != nullptr) {
				auto [ w, h ] = s_client_instance->get_window_size();
				if (self->name() == "vid_window_x") {
					s_client_instance->set_window_size(size, h);
				}
				if (self->name() == "vid_window_y") {
					s_client_instance->set_window_size(w, size);
				}
			}
			return true;
		} catch (...) {
			return false;
		}
	}
	static bool UpdateWindowState(const CVar* self, const std::string& value) {
		try {
			if (self->name() == "vid_fullscreen") {
				const std::string lowercase = StringUtil::ToLower(value);
				const bool fullscreen = lowercase == "true" ? true : (lowercase == "false" ? false : std::stoi(value) != 0);

				if (s_client_instance != nullptr) {
					s_client_instance->set_window_fullscreen(fullscreen);
				}
			} else if (self->name() == "vid_monitor") {
				const int monitor_idx = std::stoi(value);
				if (s_client_instance != nullptr) {
					s_client_instance->set_window_monitor(monitor_idx);
				}
			}
			return true;
		} catch (...) {
			return false;
		}
	}
	static bool UpdateSwapInterval(const CVar* self, const std::string& value) {
		(void)self;
		try {
			const std::string lowercase = StringUtil::ToLower(value);
			const int i = lowercase == "true" ? 1 : (lowercase == "false" ? 0 : std::stoi(value));
			glfwSwapInterval(i);
			return true;
		} catch (...) {
			return false;
		}
	}
	static GLFWmonitor* GetMonitorFromIndex(const int idx) {
		GLFWmonitor* monitor = nullptr;
		if (idx < 0) {
			monitor = glfwGetPrimaryMonitor();
		} else {
			int monitor_count;
			GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
			if (idx < monitor_count) {
				monitor = monitors[idx];
			}
		}
		return monitor;
	}
}
