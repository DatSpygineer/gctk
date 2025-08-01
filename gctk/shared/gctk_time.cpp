#include "gctk_time.hpp"

namespace gctk {
	double Time::m_dTimePrevious = 0.0;
	double Time::m_dDeltaTime = 0.0;

	double Time::CurrentTime() {
		return glfwGetTime();
	}

	void Time::Initialize() {
		m_dTimePrevious = CurrentTime();
	}
	void Time::UpdateDeltaTime() {
		m_dDeltaTime = CurrentTime() - m_dTimePrevious;
	}

}