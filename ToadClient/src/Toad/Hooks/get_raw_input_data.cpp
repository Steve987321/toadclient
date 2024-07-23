#include "pch.h"
#include "Toad/toadll.h"
#include "get_raw_input_data.h"

namespace toadll
{

	bool HGetRawInputData::Init()
	{
		return create_hook("User32.dll", "GetRawInputData", &HGetRawInputData::GetRawInputDataDetour, reinterpret_cast<LPVOID*>(&oGetRawInputData));
	}

	UINT HGetRawInputData::GetRawInputDataDetour(HRAWINPUT hRawInput, UINT uiCommand, LPVOID pData, PUINT pcbSize, UINT cbSizeHeader)
	{
		UINT result = oGetRawInputData(hRawInput, uiCommand, pData, pcbSize, cbSizeHeader);

		if (result == -1 || uiCommand != RID_INPUT)
			return result;

		if (move_mouse && (x != 0 || y != 0))
		{
			RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(pData);

			if (raw && raw->header.dwType == RIM_TYPEMOUSE)
			{
				raw->data.mouse.lLastX = x;
				raw->data.mouse.lLastY = y;
				move_mouse = false;
			}
		}

		return result;
	}
}