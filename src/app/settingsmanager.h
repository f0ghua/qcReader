#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>

class SettingsManagerPrivate;

class SettingsManager : public QObject
{
    Q_OBJECT
public:
    static SettingsManager *instance();

    explicit SettingsManager();
    ~SettingsManager();

    static void setSettings(QSettings *settings);
    static QSettings *settings();

private:

};

#endif // SETTINGSMANAGER_H
