#pragma once

namespace toadll
{
	class ActiveRenderInfo
	{
	private:
		jclass ariclass = nullptr;
		JNIEnv* env = nullptr;

	public:
		explicit ActiveRenderInfo(jclass ariklass, JNIEnv* env) : ariclass(ariklass), env(env) {}

	public:
		[[deprecated]]
		std::vector<float> get_modelview() const;
		void set_modelview(std::array<float, 16>& arr) const;

		[[deprecated]]
		std::vector<float> get_projection() const;
		void set_projection(std::array<float, 16>& arr) const;
		//void get_viewport(GLint viewportBuf[4]) const;

	public:
		_NODISCARD Vec3 get_render_pos() const;
	};
}
