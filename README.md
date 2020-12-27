# DLL Injector
A DLL injector that loads the specified DLL into Urban Terror. 

To load static and dynamic libraries, Windows executables can use the LoadLibraryA API function. This function takes a single argument, which is a full path to the library to load.
```
HMODULE LoadLibraryA(
    LPCSTR lpLibFileName
);
```
If we call LoadLibraryA in our injector's code, the DLL will be loaded into our injector's memory. Instead, we want our injector to force the game to call LoadLibraryA. To do this, we will use the CreateRemoteThread API to create a new thread in the game. This thread will then execute LoadLibraryA inside the game's running process.

However, since the thread is running inside the game's memory, LoadLibraryA will not be able to find the path of our DLL specified in our injector. To get around this, we have to write our DLL's path into the game's memory. To ensure that we do not corrupt any other memory, we will also need to allocate additional memory inside the game using VirtualAllocEx. 

The full explanation for how this code works is available at https://gamehacking.academy/lesson/25
