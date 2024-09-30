#include "mainwindow.h"

#include <QApplication>

#include <QDebug>

#include <Windows.h>

#include "crashDump.h"

int main(int argc, char *argv[])
{
    cm::CrashDump crashDump;

    QApplication a(argc, argv);


#ifdef QT_DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif


    Q_INIT_RESOURCE(resources);
    MainWindow w;
    w.show();
    return a.exec();
}
