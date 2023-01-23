#pragma once
namespace toadll
{
	class c_Minecraft
	{
	private:
		jclass mcclass = nullptr;
	public:
		bool init();
	public:
		[[nodiscard]] jobject get_mc() const;
		[[nodiscard]] jobject get_localplayer() const;
		[[nodiscard]] jobject get_world() const;

	public:
		void clean_up();
	};

	inline std::unique_ptr<c_Minecraft> p_Minecraft = nullptr;
}
