#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Worker;
class QThread;

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
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void onPlainTextEditContext();
    void onPlainTextEditActions();

private:
    void setWindowStyle();
    int loadBook();
    int searchBook();
    void startWorker();
    void stopWorker();

    Ui::MainWindow *ui;
    Worker *m_worker = NULL;
    QThread *m_workThread = NULL;
    QPoint m_oldPosition;
    double m_windowOpacity = 0.5;
};

#endif // MAINWINDOW_H
