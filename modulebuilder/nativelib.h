#include <windows.h>

extern "C" __declspec(dllexport) void* SubWindow(int x, int y, int w, int h, HWND parent);
extern "C" __declspec(dllexport) int Add(int x, int y);
extern "C" __declspec(dllexport) char* DllLog();
extern "C" __declspec(dllexport) int TestModule(char*, char*);


