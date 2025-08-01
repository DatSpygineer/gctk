#pragma once
#include <GLFW/glfw3.h>

namespace gctk {
	class Time {
		static double m_dTimePrevious;
		static double m_dDeltaTime;
	public:
		static double CurrentTime();
		constexpr static double DeltaTime() { return m_dDeltaTime; }

		static void Initialize();
		static void UpdateDeltaTime();
	};
}
