#include "mainwindow.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QSharedMemory>

int main(int argc, char *argv[])
{
    QSharedMemory sharedMemory;
    sharedMemory.setKey("HS_1200_ASSISTANT");

    if(sharedMemory.attach()) {
        return -1;
    }

    if (!sharedMemory.create(1)) {
        return -1;
    }

    QApplication a(argc, argv);
    a.addLibraryPath(QCoreApplication::applicationDirPath () + "/plugins");
    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow w;
    w.show();
    
    return a.exec();
}
