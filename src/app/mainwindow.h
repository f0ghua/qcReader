#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Worker;
class QThread;
class QxtGlobalShortcut;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    // void mousePressEvent(QMouseEvent *event);
    // void mouseMoveEvent(QMouseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void onPlainTextEditContext();
    void onPlainTextEditActions();

private:
    void setWindowStyle();
    int openFile();
    int loadBook();
    void searchBook();
    void readSettings();
    void saveSettings();
    void startWorker();
    void stopWorker();

    Ui::MainWindow *ui;
    Worker *m_worker = NULL;
    QThread *m_workThread = NULL;
    QPoint m_oldPosition;
    double m_windowOpacity = 0.5;
    int m_cursorPos = 0;
    QString m_fileName;
    QMenu *m_rlMenu = NULL;
    QxtGlobalShortcut *m_globalShortcut;
};

#endif // MAINWINDOW_H
