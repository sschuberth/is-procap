#include "processcapture.h"

static ProcessCapture* s_procap=NULL;

extern "C" {

/*
 * To undecorate names with __stdcall, you must specify them by using aliases in
 * the EXPORTS section of the .def file.
 */

bool __stdcall CaptureProcess(char const* exe,char const* arg)
{
    if (s_procap) {
        return false;
    }

    s_procap=new ProcessCapture();
    s_procap->run(exe,arg);

    return true;
}

bool __stdcall GetProcessOutput(char const** text)
{
    if (!s_procap) {
        return false;
    }

    if (!s_procap->is_running()) {
        delete s_procap;
        s_procap=NULL;
        return false;
    }

    *text=s_procap->read();

    return true;
}

}
