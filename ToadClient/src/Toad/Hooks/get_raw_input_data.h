#pragma once
namespace toadll
{
	/// Hooks USER32.GetRawInputData 
	/// 
	/// WIP, need to trigger input event polling before this is of any use (jinput_raw_64_Java_net_java_games_input_RawInputEventQueue_nPoll)
	/// This adds a new way to move the mouse
	/// This method of moving the mouse works when raw mouse input is enabled in the settings.
	class HGetRawInputData SET_HOOK_CLASS(HGetRawInputData)
	{
	public:
		bool Init() override;

		// when true uses x and y fields to set the mouse position, gets set to false after moving is done.
		static inline bool move_mouse = false;
		static inline int x = 0;
		static inline int y = 0;

		typedef UINT(WINAPI* TGetRawInputData)(
			HRAWINPUT hRawInput,
			UINT      uiCommand,
			LPVOID    pData,
			PUINT     pcbSize,
			UINT      cbSizeHeader);

		static inline TGetRawInputData oGetRawInputData = nullptr;

		static UINT GetRawInputDataDetour(
			HRAWINPUT hRawInput,
			UINT      uiCommand,
			LPVOID    pData,
			PUINT     pcbSize,
			UINT      cbSizeHeader);
	};
}


