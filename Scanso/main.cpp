#include <QApplication>

#include "ScanWidget.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    ScanWidget   widget{};
    widget.show();
    return a.exec();
}
