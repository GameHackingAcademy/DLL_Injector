/* 
A DLL injector that injects the DLL specified in dll_path into the Urban Terror process. This injector requires Urban Terror to be running. 

A different process can be specified by changing the executable name in the strcmp call.

To load static and dynamic libraries, Windows executables can use the LoadLibraryA API function. This function takes a single argument, 
which is a full path to the library to load:

HMODULE LoadLibraryA(
    LPCSTR lpLibFileName
);

If we call LoadLibraryA in our injector's code, the DLL will be loaded into our injector's memory. Instead, we want our injector to force the game 
to call LoadLibraryA. To do this, we will use the CreateRemoteThread API to create a new thread in the game. This thread will then execute LoadLibraryA 
inside the game's running process.

However, since the thread is running inside the game's memory, LoadLibraryA will not be able to find the path of our DLL specified in our injector.
To get around this, we have to write our DLL's path into the game's memory. To ensure that we do not corrupt any other memory, we will also need to 
allocate additional memory inside the game using VirtualAllocEx. 

The full explanation for how this code works is available at https://gamehacking.academy/lesson/25
*/

#include <windows.h>
#include <tlhelp32.h>

const char *dll_path = "C:\\Users\\IEUser\\source\\repos\\wallhack\\Debug\\wallhack.dll";

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
