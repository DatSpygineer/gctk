#include "gctk_client.hpp"
#include "gctk_cvar.hpp"
#include "gctk_debug.hpp"
#include "gctk_paths.hpp"
#include "gctk_str.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace gctk {
	static bool UpdateWindowSize(const CVar* self, const std::string& value);
	static bool UpdateWindowState(const CVar* self, const std::string& value);
	static bool UpdateSwapInterval(const CVar* self, const std::string& value);
	static GLFWmonitor* GetMonitorFromIndex(int idx);

	CVar vid_window_x("vid_window_x", "1024", CVAR_FLAG_USER_DATA, &UpdateWindowSize);
	CVar vid_window_y("vid_window_y", "768", CVAR_FLAG_USER_DATA, &UpdateWindowSize);
	CVar vid_fullscreen("vid_fullscreen", "false", CVAR_FLAG_USER_DATA, &UpdateWindowState);
	CVar vid_monitor("vid_monitor", "-1", CVAR_FLAG_USER_DATA, &UpdateWindowState);
	CVar vid_vsync("vid_vsync", "false", CVAR_FLAG_USER_DATA, &UpdateSwapInterval);

	static Client* s_client_instance = nullptr;

	Client::Client(const int argc, char** argv, const std::string& name) : m_bGlfwInitialized(false), m_pIconImage(nullptr) {
		if (s_client_instance != nullptr) {
			FatalError("Client already running!");
		}

		Paths::init(argc, argv);

		if (glfwInit() == GLFW_FALSE) {
			const char* errmsg;
			const int errcode = glfwGetError(&errmsg);
			FatalError(std::format("Failed to initialize GLFW:\n{} (error code {})", errmsg, errcode));
		}

		m_bGlfwInitialized = true;

		Console::LoadConfig("userdata.cfg");
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

		m_pWindow = glfwCreateWindow(vid_window_x.get_integer(), vid_window_y.get_integer(), name.c_str(), monitor, nullptr);
		if (m_pWindow == nullptr) {
			const char* errmsg;
			const int errcode = glfwGetError(&errmsg);
			FatalError(std::format("Failed to create window: \n{} (error code {})", errmsg, errcode));
		}
		glfwMakeContextCurrent(m_pWindow);

		if (std::filesystem::exists(Paths::res_path() / "game.png")) {
			int icon_x, icon_y;
			const auto icon_data = stbi_load((Paths::res_path() / "game.png").string().c_str(), &icon_x, &icon_y, nullptr, 4);

			if (icon_data != nullptr) {
				m_pIconImage = new GLFWimage;
				m_pIconImage->width = icon_x;
				m_pIconImage->height = icon_y;
				m_pIconImage->pixels = icon_data;
				glfwSetWindowIcon(m_pWindow, 1, m_pIconImage);
			} else {
				LogErr(std::format("Failed to load window icon \"{}\"", (Paths::res_path() / "game.png").string()));
			}
		}

		m_dTimePrev = glfwGetTime();
		s_client_instance = this;
	}
	Client::~Client() {
		Console::StoreUserData();

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
	}

	void Client::update() {
		glfwPollEvents();
		const double time = glfwGetTime();
		on_pre_update(m_dTimePrev - time);

		on_update(m_dTimePrev - time);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		on_render(m_dTimePrev - time);

		on_post_update(m_dTimePrev - time);
		glfwSwapBuffers(m_pWindow);
	}

	void Client::set_window_title(const std::string& title) const {
		glfwSetWindowTitle(m_pWindow, title.c_str());
	}
	std::string Client::get_window_title() const {
		return glfwGetWindowTitle(m_pWindow);
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
