#include <GL/glew.h>

#include "gctk/rendering/render_common.h"

void GctkSetBlendmode(GctkBlendMode blend_mode) {
	if (blend_mode == GCTK_BLEND_NONE) {
		glDisable(GL_BLEND);
	} else {
		glEnable(GL_BLEND);
		switch (blend_mode) {
			case GCTK_BLEND_ADD:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				break;
			case GCTK_BLEND_MULTIPLY:
				glBlendFunc(GL_DST_COLOR, GL_ZERO);
				break;
			default:
			case GCTK_BLEND_ALPHA_MIX:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;
		}
	}
}