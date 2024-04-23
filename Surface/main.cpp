#include "observer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Observer observer;
    observer.show();

    return a.exec();
}
