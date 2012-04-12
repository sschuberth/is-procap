#include "processcapture.h"

static ProcessCapture* s_procap=NULL;

extern "C" {

/*
 * To undecorate names with __stdcall, you must specify them by using aliases in
 * the EXPORTS section of the .def file.
 */

bool __stdcall BeginProcessCapture(char const* executable,char const* arguments)
{
    if (s_procap) {
        return false;
    }

    // For some reason, Pascal Script passes an empty PAnsiChar string ('') as
    // a NULL pointer, so work around that case.
    std::string exe;
    if (executable) {
        exe=executable;
    }

    std::string arg;
    if (arguments) {
        arg=arguments;
    }

    s_procap=new ProcessCapture();
    s_procap->run(exe,arg);

    return s_procap->is_running();
}

bool __stdcall GetProcessOutput(char const** text)
{
    if (!s_procap) {
        *text=NULL;
        return false;
    }

    // Check the running state before reading the output to be sure output is
    // read a last time after the thread has stopped running.
    bool running=s_procap->is_running();

    *text=s_procap->read();

    return running;
}

void __stdcall EndProcessCapture()
{
    delete s_procap;
    s_procap=NULL;
}

}
