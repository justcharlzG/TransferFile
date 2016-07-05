#include "sendwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    sendwidget w;
    w.show();

    return a.exec();
}
