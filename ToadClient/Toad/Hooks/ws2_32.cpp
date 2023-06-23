#include "pch.h"
#include "Toad/toad.h"
#include "ws2_32.h"

namespace toadll
{
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

	int c_WSARecv::Hook(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
	{
		while (StopRecvs)
		{
			SLOW_SLEEP(1);
		}
		return oWSA_Recv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
	}

	bool c_WSARecv::init()
	{
		return create_hook("WS2_32.dll", "WSARecv", &c_WSARecv::Hook, reinterpret_cast<LPVOID*>(&oWSA_Recv));
	}
}
