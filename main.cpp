#include "MainWindow.h"
#include"LoginDialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    LoginDialog w;
    w.show();
    return a.exec();
}
