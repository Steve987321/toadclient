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

	inline std::unique_ptr<c_Hooks> p_Hooks = nullptr;

	inline vec3 render_pos_vec3(0,0,0);
	inline vec3 render_rotation(0, 0, 0);

    inline std::vector<vec3> entities;

    inline void setup_ortho()
    {
        std::cout << "ortho\n";
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

    inline void draw_outline(float x, float y, float width, float height, float lineWidth, const GLubyte col[3])
    {
        glLineWidth(lineWidth);
        glBegin(GL_LINE_STRIP);
        glColor3ub(col[0], col[1], col[2]);

        glVertex2f(x- 0.5f, y - 0.5f);
        glVertex2f(x + width + 0.5f, y - 0.5f);
        glVertex2f(x + width + 0.5f, y + height + 0.5f);
        glVertex2f(x - 0.5f, y + height + 0.5f);
        glVertex2f(x - 0.5f, y - 0.5f);
        glEnd();
    }

    inline vec2 WorldToScreen(const vec3& worldPosition, const vec3& cameraPosition, float camyaw, float campitch, float fieldOfView, int screenWidth, int screenHeight) {
        auto relativePos = worldPosition - cameraPosition;
        auto [yaw, pitch] = get_angles(cameraPosition, worldPosition);

        auto relativeYaw = camyaw - yaw;
        auto relativePitch = campitch - pitch;

        if (relativePitch > 180 || relativeYaw > 180) return {-1,-1};

        return {screenWidth / 2.f + relativeYaw, screenHeight / 2.f + relativePitch};
    }
	
	//inline void update_renderinfo();

}
