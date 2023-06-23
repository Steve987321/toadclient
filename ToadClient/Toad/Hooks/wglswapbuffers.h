#pragma once

namespace toadll
{

class c_Swapbuffershook SET_HOOK_CLASS(c_Swapbuffershook)
{
private:
	typedef BOOL(WINAPI* twglSwapBuffers) (HDC hDc);
	static inline twglSwapBuffers owglSwapBuffers = nullptr;

	static BOOL Hook(HDC hDc);

public:
	bool Init() override;
};

}