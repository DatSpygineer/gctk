#include <iostream>
#include <sstream>
#include <string>

#include <QApplication>

#include <gctk/gctk.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

extern void InitUI();
extern void UpdateUI();
extern void DisposeUI();

int main(int argc, char** argv) {
	QApplication app(argc, argv);

	if (!GctkInitGame(argc, argv)) {
		return GctkLastErrorCode();
	}

	GctkSetUpdateCallback([](double delta) {
		(void)delta;
		UpdateUI();
	});
	GctkSetPreRenderCallback([](double delta) {
		(void)delta;
		ImGui::Render();
	});
	GctkSetPostRenderCallback([](double delta) {
		(void)delta;
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	});

	InitUI();
	while (GctkUpdate()) { }

	DisposeUI();
	GctkDispose();

	return 0;
}