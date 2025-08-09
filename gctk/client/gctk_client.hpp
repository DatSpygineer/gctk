#pragma once

#include <string>
#include <unordered_map>

#include "gctk_math.hpp"
#include "gctk_filesys.hpp"

#include <GLFW/glfw3.h>

namespace gctk {
	class Client {
		GLFWwindow* m_pWindow;
		bool m_bGlfwInitialized;
		GLFWimage* m_pIconImage;
		GLFWimage* m_pCursorImage;
		Color m_cBackgroundColor;
		std::string m_sName;
	public:
		Client(int argc, char** argv, const std::string& name,
			const std::vector<std::string>& asset_packs,
			const std::optional<Path>& mod_path = std::nullopt
		);
		virtual ~Client();

		void init();
		void update();
		void render();
		[[nodiscard]] bool should_exit() const;

		void set_window_title(const std::string& title) const;
		[[nodiscard]] std::string get_window_title() const;
		void set_window_location(int x, int y) const;
		[[nodiscard]] std::tuple<int, int> get_window_location() const;
		void set_window_size(int width, int height) const;
		[[nodiscard]] std::tuple<int, int> get_window_size() const;
		void set_window_fullscreen(bool fullscreen) const;
		void set_window_fullscreen(bool fullscreen, int monitor_idx) const;
		void set_window_fullscreen(bool fullscreen, GLFWmonitor* monitor_ptr) const;
		[[nodiscard]] bool get_window_fullscreen() const;
		void set_window_monitor(int idx) const;
		void set_window_monitor(GLFWmonitor* monitor) const;
		[[nodiscard]] GLFWmonitor* get_window_monitor() const;

		void set_cursor(const std::string& cursor_name);
		void reset_cursor();

		[[nodiscard]] constexpr GLFWwindow* get_window() const { return m_pWindow; }

		void set_background_color(const Color& color);
		[[nodiscard]] Color get_background_color() const;

		static Client* Instance();
	};
}