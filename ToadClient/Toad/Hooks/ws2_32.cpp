#include "pch.h"
#include "Toad/toad.h"
#include "ws2_32.h"

namespace toadll
{
	//typedef int (WINAPI* tWSA_Recv)(_In_ SOCKET, _In_ _Out_ const char*, _In_ int len, _In_ _Out_ int flags);

	//tWSA_Recv oWSA_Recv = nullptr;

	int c_WSASend::Hook(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
	{
		while (StopSends)
		{
			SLOW_SLEEP(1);
		}
		return oWSA_Send(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
	}

	bool c_WSASend::init()
	{
		return create_hook("WS2_32.dll", "WSASend", &c_WSASend::Hook, reinterpret_cast<LPVOID*>(&oWSA_Send));
	}
}
