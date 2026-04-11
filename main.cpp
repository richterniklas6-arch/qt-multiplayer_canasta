#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MAIN_WINDOW w;
    w.show();
    return a.exec();
}
