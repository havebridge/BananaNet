#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    QApplication application(argc, argv);

    App::MainWindow window;
    window.show();

    return application.exec();
}
