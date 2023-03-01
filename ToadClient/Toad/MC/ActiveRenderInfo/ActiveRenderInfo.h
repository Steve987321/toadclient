#pragma once

namespace toadll
{
	class c_ActiveRenderInfo
	{
	private:
		jclass ariclass = nullptr;

	public:
		explicit c_ActiveRenderInfo(jclass ariklass) { ariclass = ariklass; }

	public:
		void get_modelview(GLfloat modelviewbuf[15]) const;
		void get_projection(GLfloat projectionbuf[15]) const;
		void get_viewport(GLint viewportBuf[3]) const;

	public:
		_NODISCARD vec3 get_render_pos() const;
	};
}
