#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worker.h"
#include "searchdialog.h"

#include <QThread>
#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowStyle();

    startWorker();
}

MainWindow::~MainWindow()
{
    stopWorker();
    delete ui;
}

void MainWindow::setWindowStyle()
{
    setWindowOpacity(m_windowOpacity);
    setStyleSheet("background: white; border: 0px;");
    //setStyleSheet("border: 0px;");
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);

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
        loadBook();
    }
    qDebug() << ac->text().toStdString().c_str();
}

int MainWindow::loadBook()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open"), QString(), tr("Txt Files (*.txt);;All Files (*.*)"));
    if (fileName.isEmpty())
        return -1;

    QFile inFile(fileName);
    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return -1;
    }

    ui->plainTextEdit->setPlainText(inFile.readAll());

    return 0;
}

int MainWindow::searchBook()
{
    SearchDialog dialog;
    connect(&dialog, &SearchDialog::search, this, [=](QString text){
        ui->plainTextEdit->find(text);
    });
    dialog.exec();
}

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

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if( event->key() == Qt::Key_F11 ) {
        if (m_windowOpacity > 0) {
            m_windowOpacity -= 0.1;
        }
        setWindowOpacity(m_windowOpacity);
    } else if( event->key() == Qt::Key_F12 ) {
        if (m_windowOpacity < 1) {
            m_windowOpacity += 0.1;
        }
        setWindowOpacity(m_windowOpacity);
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
