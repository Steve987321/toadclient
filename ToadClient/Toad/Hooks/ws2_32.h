#pragma once

namespace toadll {

class c_WSASend final : public c_Hook, public c_Singleton<c_WSASend>
{
private:
	typedef int (WINAPI* tWSA_Send)(SOCKET, LPWSABUF, DWORD, LPDWORD, DWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
	static inline tWSA_Send oWSA_Send = nullptr;

	static int Hook(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

public:
	inline static bool StopSends = false;

public:
	bool init();
};

class c_WSARecv final : public c_Hook, public c_Singleton<c_WSARecv>
{
public:
	bool init();
};

}