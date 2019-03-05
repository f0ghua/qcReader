#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worker.h"
#include "searchdialog.h"
#include "settingsmanager.h"
#include "QAppLogging.h"

#include <QThread>
#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowStyle();
    qApp->installEventFilter(this);

    readSettings();
    startWorker();
}

MainWindow::~MainWindow()
{
    saveSettings();
    stopWorker();
    delete ui;
}

void MainWindow::setWindowStyle()
{
    setWindowOpacity(m_windowOpacity);
    setStyleSheet("#MainWindow #plainTextEdit {background: white; border: 0px;}"
                  "#centralWidget {background: white; border: 0px;}"
                  "#mainToolBar {background: white; border: 0px;}"
                  "#menuBar {background: white; border: 0px;}"
                  "#statusBar {background: white; border: 0px;}"
                  );
    //setStyleSheet("background: white; border: 0px;");
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
#if defined(Q_OS_WIN)
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
#endif
    setAttribute(Qt::WA_TranslucentBackground);

    ui->plainTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->plainTextEdit->setPlaceholderText(tr("CustomContextMenu"));
    connect(ui->plainTextEdit, &QPlainTextEdit::customContextMenuRequested,
            this, &MainWindow::onPlainTextEditContext);
}

void MainWindow::onPlainTextEditContext()
{
    QMenu tmpMenu;
    QMenu *menu = &tmpMenu; //new QMenu(this);
    QAction *a1 = menu->addAction("Open");
    QAction *a2 = menu->addAction("Search");
    QAction *a3 = menu->addAction("Exit");
    connect(a1, &QAction::triggered, this, &MainWindow::onPlainTextEditActions);
    connect(a2, &QAction::triggered, this, &MainWindow::onPlainTextEditActions);
    connect(a3, &QAction::triggered, this, &MainWindow::onPlainTextEditActions);

    menu->exec(QCursor::pos());
}

void MainWindow::onPlainTextEditActions()
{
    QAction *ac = qobject_cast<QAction *>(sender());
    if(!ac) return;
    if (ac->text() == "Exit") {
        qApp->exit();
    } else if (ac->text() == "Search") {
        searchBook();
    } else if (ac->text() == "Open") {
        openFile();
    }
    qDebug() << ac->text().toStdString().c_str();
}

int MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open"), QString(), tr("Txt Files (*.txt);;All Files (*.*)"));
    if (fileName.isEmpty())
        return -1;

    m_fileName = fileName;
    return loadBook();
}

int MainWindow::loadBook()
{
    if (m_fileName.isEmpty())
        return -1;

    QFile inFile(m_fileName);
    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return -1;
    }

    ui->plainTextEdit->setPlainText(inFile.readAll());

    // reload the cursor
    QTextCursor cursor = ui->plainTextEdit->textCursor();
    cursor.setPosition(m_cursorPos, QTextCursor::MoveAnchor);
    ui->plainTextEdit->setTextCursor(cursor);

    return 0;
}

void MainWindow::searchBook()
{
    SearchDialog dialog(this);
    connect(&dialog, &SearchDialog::search, this, [=](QString text){
        ui->plainTextEdit->find(text);
    });
    dialog.exec();
}

void MainWindow::readSettings()
{
    QSettings *settings = SettingsManager::instance()->settings();
    m_cursorPos = settings->value("cursor", 0).toInt();
    m_fileName = settings->value("file", QString()).toString();

    loadBook();
}

void MainWindow::saveSettings()
{
    int cursorPos = ui->plainTextEdit->textCursor().position();
    QSettings *settings = SettingsManager::instance()->settings();
    settings->setValue("cursor", cursorPos);
    settings->setValue("file", m_fileName);
}

#if 0
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    //if (event->buttons().testFlag(Qt::MiddleButton))
    {
        m_oldPosition = event->globalPos();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //if (event->buttons().testFlag(Qt::MiddleButton))
    {
        const QPoint delta = event->globalPos() - m_oldPosition;
        move(x()+delta.x(), y()+delta.y());
        m_oldPosition = event->globalPos();
    }
}
#endif

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{

    if ((obj == ui->plainTextEdit) &&
            (event->type() == QEvent::MouseButtonPress)) {
        QMouseEvent *ev = static_cast<QMouseEvent *>(event);
        if (ev->buttons() & Qt::MiddleButton) {
            m_oldPosition = ev->globalPos();
        }
    }

    if ((obj == ui->plainTextEdit || obj == ui->plainTextEdit->viewport()) &&
            (event->type() == QEvent::MouseMove)) {
        QMouseEvent *ev = static_cast<QMouseEvent *>(event);
        if (ev->buttons() & Qt::MiddleButton) {
            const QPoint delta = ev->globalPos() - m_oldPosition;
            move(x()+delta.x(), y()+delta.y());
            m_oldPosition = ev->globalPos();
        }
    }

    return QObject::eventFilter(obj, event);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case (Qt::Key_F11):
        if (m_windowOpacity > 0) {
            m_windowOpacity -= 0.1;
        }
        setWindowOpacity(m_windowOpacity);
        break;
    case (Qt::Key_F12):
        if (m_windowOpacity < 1) {
            m_windowOpacity += 0.1;
        }
        setWindowOpacity(m_windowOpacity);
        break;
    case (Qt::Key_F):
        if (event->modifiers() & Qt::ControlModifier) {
            searchBook();
        }
        break;
    default:
        break;
    }
}

void MainWindow::startWorker()
{
    m_workThread = new QThread();
    m_worker = new Worker();
    m_worker->moveToThread(m_workThread);
    QObject::connect(m_workThread, &QThread::started, m_worker, &Worker::run);
    QObject::connect(m_workThread, &QThread::finished, m_worker, &Worker::deleteLater);
    QObject::connect(m_workThread, &QThread::finished, m_workThread, &QThread::deleteLater);
    //QObject::connect(this, &MainWindow::workStop, m_worker, &Worker::onWorkStop);

    m_workThread->start(QThread::HighPriority);
    qDebug() << "Worker thread started.";
}

void MainWindow::stopWorker()
{
    //emit workStop();

    if(m_workThread && (!m_workThread->isFinished())) {
        m_workThread->quit();
        if(!m_workThread->wait()) {
            qDebug() << "can't stop thread";
        }
        qDebug() << "Worker thread finished.";
    }
}
