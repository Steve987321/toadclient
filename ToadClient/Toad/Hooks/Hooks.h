#pragma once

#include "Toad/Types.h"
#include "Toad/MC/Utils/utils.h"

namespace toadll
{
	class c_Hooks
	{
	private:
		bool enabled = false;
		void* optr = nullptr;

	public:
		bool init();
		void enable();
		void disable();
		void dispose() const;
	};

    inline int SCREEN_HEIGHT = -1, SCREEN_WIDTH = -1;

    inline float testingfloat = 1;

	inline std::unique_ptr<c_Hooks> p_Hooks = nullptr;
    inline std::vector<std::pair<vec2, const char*>> renderNames;
    inline std::vector<std::shared_ptr<std::string>> logs;

    inline void setup_ortho()
    {
        //std::cout << "ortho\n";
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glPushMatrix();
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, viewport[2], viewport[3], 0, -1, -1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDisable(GL_DEPTH_TEST);
    }

    inline void restore_GL()
    {
        glPopMatrix();
        glPopAttrib();
    }

}
