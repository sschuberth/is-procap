#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

typedef bool (__stdcall *BeginProcessCapturePtr)(char const* executable,char const* arguments);
static BeginProcessCapturePtr BeginProcessCapture=NULL;

typedef bool (__stdcall *GetProcessOutputPtr)(char const** text);
static GetProcessOutputPtr GetProcessOutput=NULL;

typedef void (__stdcall *EndProcessCapturePtr)();
static EndProcessCapturePtr EndProcessCapture=NULL;

int main()
{
    HMODULE handle=LoadLibrary("is-procap.dll");

    BeginProcessCapture=(BeginProcessCapturePtr)GetProcAddress(handle,"BeginProcessCapture");
    assert(BeginProcessCapture);

    GetProcessOutput=(GetProcessOutputPtr)GetProcAddress(handle,"GetProcessOutput");
    assert(GetProcessOutput);

    EndProcessCapture=(EndProcessCapturePtr)GetProcAddress(handle,"EndProcessCapture");
    assert(EndProcessCapture);

    BeginProcessCapture("ping","www.heise.de");

    OutputDebugString("*** BEGIN ***\n");

    char const* text;
    while (GetProcessOutput(&text)) {
        if (text) {
            OutputDebugString(text);
        }
    }

    OutputDebugString("*** END ***\n");

    EndProcessCapture();
}
