#pragma once

namespace toadll
{

class c_Swapbuffershook final : public c_Hook, public c_Singleton<c_Swapbuffershook>
{

public:
	bool init();

};

}