#include "include/GameInitScreen.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");
    a.setWindowIcon(QIcon(":/icons/icon.png"));
    QTranslator translator;
    QString translationFile = QCoreApplication::applicationDirPath() + "/" + QApplication::applicationName() + "_" + QLocale::system().name();
    if (!translator.load(translationFile))
    {
        qWarning() << "Translation Error";
        return;
    }
    a.installTranslator(&translator);
    GameInitScreen w;
    w.show();
    return a.exec();
}
