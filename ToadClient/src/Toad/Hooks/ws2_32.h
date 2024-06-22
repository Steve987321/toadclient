#pragma once

namespace toadll {

class HWSASend SET_HOOK_CLASS(HWSASend)
{
private:
	typedef int (WINAPI* tWSA_Send)(SOCKET, LPWSABUF, DWORD, LPDWORD, DWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
	static inline tWSA_Send oWSA_Send = nullptr;

	static int Hook(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

public:
	inline static bool StopSends = false;

public:
	bool Init() override;
};

class HWSARecv SET_HOOK_CLASS(HWSARecv)
{
private:
	typedef int (WINAPI* tWSA_Recv)(SOCKET, LPWSABUF, DWORD, LPDWORD, LPDWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
	static inline tWSA_Recv oWSA_Recv = nullptr;

	static int Hook(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

public:
	inline static bool StopRecvs = false;

public:
	bool Init() override;
};

}