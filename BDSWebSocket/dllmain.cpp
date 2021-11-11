#include "pch.h"
#include "WebSocketServer.h"

BOOL APIENTRY DllMain(
	HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH: 
	case DLL_THREAD_DETACH: 
		break;
	case DLL_PROCESS_DETACH: {
		stopWebSocketServer();
		break;
	}
	}
	return TRUE;
}

void entry();
void initConsole() {
	system("chcp 65001");
	//enable colorful output
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode;
	GetConsoleMode(hOutput, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; //NOLINT
	SetConsoleMode(hOutput, dwMode);
}

extern "C" {
	_declspec(dllexport) void onPostInit() {
		std::ios::sync_with_stdio(false);
		entry();
		initConsole();
	}
}