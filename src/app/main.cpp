#include "mainwindow.h"
#include "QAppLogging.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QAppLogging::installHandler();
    QAppLogging::instance()->setFilterRulesByLevel(QAppLogging::TraceLevel);
    QAppLogging::instance()->setOutputDest(QAppLogging::eDestSystem);

    MainWindow w;
    w.show();

    return a.exec();
}
