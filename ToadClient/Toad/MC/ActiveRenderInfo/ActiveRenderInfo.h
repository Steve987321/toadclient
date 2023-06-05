#pragma once

namespace toadll
{
	class c_ActiveRenderInfo
	{
	private:
		jclass ariclass = nullptr;
		JNIEnv* env = nullptr;

	public:
		explicit c_ActiveRenderInfo(jclass ariklass, JNIEnv* env) : ariclass(ariklass), env(env) {}

	public:
		void get_modelview(GLfloat modelviewbuf[16]) const;
		void get_projection(GLfloat projectionbuf[16]) const;
		//void get_viewport(GLint viewportBuf[4]) const;

	public:
		_NODISCARD vec3 get_render_pos() const;
	};
}
