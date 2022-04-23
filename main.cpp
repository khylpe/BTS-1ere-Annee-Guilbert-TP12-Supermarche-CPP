#include "fenetre.h"
#include "session.h"
#include <QtCore/QCoreApplication>

#include <QApplication>

// https://stackoverflow.com/questions/10896056/qt-signals-and-slots-in-different-classes

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Fenetre w;
    session f;
    QObject::connect(&f, SIGNAL(sessionTypeValue(QString)), &w, SLOT(showMainWindow()));
    QObject::connect(&f, SIGNAL(sessionTypeValue(QString)), &w, SLOT(firstStatusUi(QString)));
    f.show();
    return a.exec();
}
