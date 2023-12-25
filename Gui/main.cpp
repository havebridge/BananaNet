#include "Application.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    Application window;

    window.show();
    
    return application.exec();
}