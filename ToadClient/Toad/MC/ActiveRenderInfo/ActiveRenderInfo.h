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
		void getModelView(std::array<float, 16>& arr) const;
		void getProjection(std::array<float, 16>& arr) const;
	public:
		/// Returns the camera position,
		///	ONLY WORKS ON 1.8.9
		_NODISCARD Vec3 get_render_pos() const;
	};
}
