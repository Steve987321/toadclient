#include "pch.h"
#include "Toad/toad.h"
#include "ws2_32.h"

namespace toadll
{
	//typedef int (WINAPI* tWSA_Recv)(_In_ SOCKET, _In_ _Out_ const char*, _In_ int len, _In_ _Out_ int flags);
	typedef int (WINAPI* tWSA_Send)(SOCKET, LPWSABUF, DWORD, LPDWORD, DWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);

	//tWSA_Recv oWSA_Recv = nullptr;
	tWSA_Send oWSA_Send = nullptr;

	int WINAPI Hook_WSA_Send(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
	{
		/*log_Debug(lpBuffers->buf);*/
		return oWSA_Send(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
	}

	bool c_WSASend::init()
	{
		return create_hook("WS2_32.dll", "WSASend", Hook_WSA_Send, reinterpret_cast<LPVOID*>(&oWSA_Send));
	}
}
