#pragma once

namespace toadll
{
	class c_ActiveRenderInfo
	{
	private:
		jclass ariclass = nullptr;

	public:
		explicit c_ActiveRenderInfo(const jclass& ariklass) : ariclass(ariklass) {}

	public:
		void get_modelview(GLint* viewportbuf) const;
		void get_projection(GLfloat* projectionbuf) const;
		void get_viewport(GLfloat* viewportBuf) const;

	public:
		_NODISCARD vec3 get_render_pos() const;
	};
}
