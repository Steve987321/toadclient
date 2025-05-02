#include "pch.h"
#include "Toad/toad.h"
#include "ws2_32.h"

namespace toad
{
	struct args
	{
		args(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
			: s(s), lpBuffers(lpBuffers), dwBufferCount(dwBufferCount), lpNumberOfBytesSent(lpNumberOfBytesSent), dwFlags(dwFlags), lpOverlapped(lpOverlapped), lpCompletionRoutine(lpCompletionRoutine)
		{}

		SOCKET s;
		LPWSABUF lpBuffers;
		DWORD dwBufferCount;
		LPDWORD lpNumberOfBytesSent;
		DWORD dwFlags;
		LPWSAOVERLAPPED lpOverlapped;
		LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine;
	};
	static std::vector <std::pair<args, Timer>> ok;

	int HWSASend::Hook(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
	{
		while (StopSends)
		{			
			SLEEP(1);
		}
		return oWSA_Send(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
	}

	bool HWSASend::Init()
	{
		return create_hook("WS2_32.dll", "WSASend", &HWSASend::Hook, reinterpret_cast<LPVOID*>(&oWSA_Send));
	}

	int HWSARecv::Hook(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
	{
		while (StopRecvs)
		{
			SLEEP(1);
		}

		int result = oWSA_Recv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);

		return result;
		return oWSA_Recv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
	}

	bool HWSARecv::Init()
	{
		return create_hook("WS2_32.dll", "WSARecv", &HWSARecv::Hook, reinterpret_cast<LPVOID*>(&oWSA_Recv));
	}
}
