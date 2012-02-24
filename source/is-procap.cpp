#include "processcapture.h"

int main()
{
    ProcessCapture procap("ping","www.heise.de");

    procap();

    system("pause");
}
