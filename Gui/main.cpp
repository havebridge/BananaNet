#include "Application.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    //AllocConsole();
    //freopen("CONOUT$", "w", stdout);

    QApplication application(argc, argv);

    Application window;

    window.show();

    return application.exec();
}