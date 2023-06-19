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
		std::vector<float> get_modelview() const;
		void set_modelview(std::vector<float>& vec) const;

		std::vector<float> get_projection() const;
		void set_projection(std::vector<float>& vec) const;
		//void get_viewport(GLint viewportBuf[4]) const;

	public:
		_NODISCARD vec3 get_render_pos() const;
	};
}
