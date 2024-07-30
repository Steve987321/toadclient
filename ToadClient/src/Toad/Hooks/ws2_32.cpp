#include "pch.h"
#include "Toad/toadll.h"
#include "ws2_32.h"

namespace toadll
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
		{/*
			ok.emplace_back(args{ s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine }, Timer{});
			std::cout << ok.size() << std::endl;

			auto it = ok.begin();
			while (it != ok.end())
			{
				auto& [arg, timer] = *it;

				if (timer.Elapsed() > 100)
				{
					int res = oWSA_Send(arg.s, arg.lpBuffers, arg.dwBufferCount, arg.lpNumberOfBytesSent, arg.dwFlags, arg.lpOverlapped, arg.lpCompletionRoutine);
					ok.erase(it);
					return res;
				}

				it++;
			}*/
			//return oWSA_Send(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
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
		return oWSA_Recv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
	}

	bool HWSARecv::Init()
	{
		return create_hook("WS2_32.dll", "WSARecv", &HWSARecv::Hook, reinterpret_cast<LPVOID*>(&oWSA_Recv));
	}
}
