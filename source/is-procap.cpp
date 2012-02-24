#include "processcapture.h"

int main()
{
    ProcessCapture procap(boost::process::find_executable_in_path("ping"),"www.heise.de");

    procap();

    system("pause");
}
