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
		[[nodiscard]] jclass get_mcclass() const;
		[[nodiscard]] jobject get_mc() const;
		[[nodiscard]] std::shared_ptr<c_Entity> get_localplayer() const;
		[[nodiscard]] jobject get_localplayerobj() const;
		[[nodiscard]] jobject get_world() const;
		[[nodiscard]] std::vector<std::shared_ptr<toadll::c_Entity>> get_playerList() const;

	public:
		void clean_up();
	};

	inline std::unique_ptr<c_Minecraft> p_Minecraft = nullptr;
}
