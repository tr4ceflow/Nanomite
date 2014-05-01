#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>

#include "cls/clsMemManager.h"




#define NOMINMAX



int main(int argc, char *argv[])
{
    clsMemManager clsMManage = clsMemManager();



    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
