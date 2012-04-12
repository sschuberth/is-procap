#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

typedef bool (__stdcall *CaptureProcessPtr)(char const* exe,char const* arg);
static CaptureProcessPtr CaptureProcess=NULL;

typedef bool (__stdcall *GetProcessOutputPtr)(char const** text);
static GetProcessOutputPtr GetProcessOutput=NULL;

int main()
{
    HMODULE handle=LoadLibrary("is-procap.dll");

    CaptureProcess=(CaptureProcessPtr)GetProcAddress(handle,"CaptureProcess");
    assert(CaptureProcess);

    GetProcessOutput=(GetProcessOutputPtr)GetProcAddress(handle,"GetProcessOutput");
    assert(GetProcessOutput);

    CaptureProcess("ping","www.heise.de");

    OutputDebugString("*** BEGIN ***\n");

    char const* text;
    while (GetProcessOutput(&text)) {
        if (text) {
            OutputDebugString(text);
        }
    }

    OutputDebugString("*** END ***\n");
}
