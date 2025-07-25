#pragma once

#include "gctk_api.hpp"

#include <GLFW/glfw3.h>
#include <GL/glew.h>

namespace gctk {
	class GCTK_CLIENT_API Client {
		GLFWwindow* m_pWindow;
		bool m_bGlfwInitialized;
		GLFWimage* m_pIconImage;
		double m_dTimePrev;
	public:
		Client(int argc, char** argv, const std::string& name);
		virtual ~Client();

		virtual void on_init() { }
		virtual void on_pre_update(double dt) { }
		virtual void on_update(double dt) { }
		virtual void on_post_update(double dt) { }
		virtual void on_render(double dt) { }
		virtual void on_exit() { }

		void update();

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
	};
}