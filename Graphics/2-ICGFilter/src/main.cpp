#include "ICGFilter.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ICGFilter w;
    w.show();
    return a.exec();
}
