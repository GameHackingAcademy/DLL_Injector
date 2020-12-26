#include <windows.h>
#include <tlhelp32.h>

const char* dll_path = "C:\\Users\\IEUser\\source\\repos\\wallhack\\Debug\\wallhack.dll";

int main(int argc, char** argv) {
	HANDLE snapshot = 0;
	PROCESSENTRY32 pe32 = { 0 };

	DWORD exitCode = 0;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	Process32First(snapshot, &pe32);

	do {
		if (strcmp((const char*)pe32.szExeFile, (const char*)L"Quake3-UrT.exe") == 0) {
			HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, true, pe32.th32ProcessID);

			void *lpBaseAddress = VirtualAllocEx(process, NULL, strlen(dll_path) + 1, MEM_COMMIT, PAGE_READWRITE);
			WriteProcessMemory(process, lpBaseAddress, dll_path, strlen(dll_path) + 1, NULL);
			HMODULE kernel32base = GetModuleHandle(L"kernel32.dll");
			HANDLE thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(kernel32base, "LoadLibraryA"), lpBaseAddress, 0, NULL);
			WaitForSingleObject(thread, INFINITE);
			GetExitCodeThread(thread, &exitCode);

			VirtualFreeEx(process, lpBaseAddress, 0, MEM_RELEASE);
			CloseHandle(thread);
			CloseHandle(process);
			break;
		}
	} while (Process32Next(snapshot, &pe32));

	return 0;
}
