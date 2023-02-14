#include <FL/Fl.h>

#include "PsmApp.h"
#include "PsmUI.h"

PsmApp *theApp = 0;
PsmUI *theUI = 0;

int main(int argc, char **argv)
{
    PsmApp app;
    theApp = &app;

    PsmUI ui;
    theUI = &ui;

    ui.go();
}
