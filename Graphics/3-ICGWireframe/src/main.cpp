#include <QApplication>

#include "ICGWireframe.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ICGWireframe w;
    w.show();
    return a.exec();
}
