#include "processcapture.h"

int main()
{
    ProcessCapture procap;

    procap.run("ping","www.heise.de");

    system("pause");
}
