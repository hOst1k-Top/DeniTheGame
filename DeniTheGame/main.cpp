#include "include/GameInitScreen.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");
    a.setWindowIcon(QIcon(":/icons/icon.png"));
    GameInitScreen w;
    w.show();
    return a.exec();
}
