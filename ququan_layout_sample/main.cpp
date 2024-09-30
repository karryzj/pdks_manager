#include "mainwindow.h"

#include <QApplication>

#include <QDebug>
#include <Windows.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef QT_DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif

    MainWindow w;
    w.show();
    return a.exec();
}
