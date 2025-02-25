#include "ICGPaint.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ICGPaint window;
    window.show();
    return a.exec();
}
