#pragma once

namespace toadll {

class c_WSASend final : public c_Hook, public c_Singleton<c_WSASend>
{
public:
	bool init();
};

class c_WSARecv final : public c_Hook, public c_Singleton<c_WSARecv>
{
public:
	bool init();
};

}