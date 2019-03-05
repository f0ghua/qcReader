#include "mainwindow.h"
#include "appconstants.h"
#include "settingsmanager.h"
#include "QAppLogging.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings *settings = new QSettings(QCoreApplication::instance()->applicationDirPath()+
                                        QLatin1Char('/')+
                                        Core::Constants::CONFIG_FILENAME,
                                        QSettings::IniFormat);
    settings->setIniCodec("UTF-8");

    SettingsManager settingsManager;
    SettingsManager::setSettings(settings);

    QAppLogging::installHandler();
    QAppLogging::instance()->setFilterRulesByLevel(QAppLogging::TraceLevel);
    QAppLogging::instance()->setOutputDest(QAppLogging::eDestSystem);

    MainWindow w;
    w.show();

    return a.exec();
}
