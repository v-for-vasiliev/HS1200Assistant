#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //qApp->installEventFilter(this);

    working = false;

    createActions();
    createTrayIcon();
    createButtonCombo();
    setupLineEdits();
    loadSettings();

    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(this->ui->saveButton, SIGNAL(clicked()), this, SLOT(saveButtonClicked()));
    connect(this->ui->detectCoordButton, SIGNAL(clicked()), this, SLOT(detectCoordButtonClicked()));

    setIcons();
    trayIcon->show();

    setFixedSize(440, 130);

    setWindowTitle(tr("Creative HS-1200 Headset Assistant"));

    processWorker();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}

void MainWindow::showMessage(QString text)
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(1);
    trayIcon->showMessage("Creative HS-1200 Assistant", text, icon, 5000);
}

void MainWindow::setupLineEdits()
{
    QValidator *widthInputRange = new QIntValidator(0, QApplication::desktop()->size().width(), this);
    QValidator *heightInputRange = new QIntValidator(0, QApplication::desktop()->size().height(), this);
    this->ui->xLineEdit->setValidator(widthInputRange);
    this->ui->yLineEdit->setValidator(heightInputRange);
    connect(this->ui->xLineEdit, SIGNAL(textChanged(QString)), this, SLOT(xLineEditTextChanged(QString)));
    connect(this->ui->yLineEdit, SIGNAL(textChanged(QString)), this, SLOT(yLineEditTextChanged(QString)));
}

void MainWindow::createButtonCombo() {
    this->ui->buttonComboBox->addItem(tr("Volume up"));
    this->ui->buttonComboBox->addItem(tr("Volume down"));
    this->ui->buttonComboBox->addItem(tr("Accept call"));
    this->ui->buttonComboBox->setCurrentIndex(1);
}

void MainWindow::setIcons()
{
    QIcon icon(":/images/headset.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    trayIcon->setToolTip(QObject::tr("Creative HS1200 Assistant"));
}

void MainWindow::createActions()
{
    processWorkerAction = new QAction(tr("Start"), this);
    connect(processWorkerAction, SIGNAL(triggered()), this, SLOT(processWorker()));

    stopWorkerAction = new QAction(tr("Stop"), this);
    connect(stopWorkerAction, SIGNAL(triggered()), this, SLOT(stopWorker()));

    quitAction = new QAction(tr("Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit())); //TODO: implement quit()
}

void MainWindow::swapActions()
{
    processWorkerAction->setEnabled(!working);
    stopWorkerAction->setEnabled(working);
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(processWorkerAction);
    trayIconMenu->addAction(stopWorkerAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::processWorker()
{
    if (working)
    {
        showMessage(tr("Dota 2 click service is already running"));
        return;
    }
    showMessage(tr("Dota2 click service started"));
    working = true;

    worker = new USBWorker();
    thread = new QThread;
    worker->moveToThread(thread);

    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(buttonClicked(int)), this, SLOT(workerButtonClicked(int)));
    connect(worker, SIGNAL(finished(int)), this, SLOT(workerFinished(int)));
    connect(worker, SIGNAL(destroyed()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
    swapActions();
    return;
}

void MainWindow::stopWorker()
{
    worker->stop();
}

void MainWindow::setMousePosition()
{
    long x = this->ui->xLineEdit->text().toLong();
    long y = this->ui->yLineEdit->text().toLong();

    INPUT Input = { 0 };

    long fScreenWidth = GetSystemMetrics(SM_CXSCREEN) - 1;
    long fScreenHeight = GetSystemMetrics(SM_CYSCREEN) - 1;

    float fx = x * (65535.0f / fScreenWidth);
    float fy = y * (65535.0f / fScreenHeight);

    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE;
    Input.mi.dx = (LONG) fx;
    Input.mi.dy = (LONG) fy;
    SendInput(1,&Input,sizeof(INPUT));
}

void MainWindow::leftClick()
{
    INPUT Input = { 0 };

    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &Input, sizeof(INPUT));

    ZeroMemory(&Input, sizeof(INPUT));
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &Input, sizeof(INPUT));
}

void MainWindow::loadSettings()
{
    QString settingsFile(SETTINGS_FILE);
    if (!QFile::exists(settingsFile))
    {
        saveSettings(true);
    }

    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.sync();
    if (settings.status() == QSettings::NoError)
    {
        this->ui->xLineEdit->setText(settings.value("x", "0").toString());
        this->ui->yLineEdit->setText(settings.value("y", "0").toString());
        this->ui->buttonComboBox->setCurrentIndex(settings.value("button", 0).toInt());
    }
}

void MainWindow::saveSettings(bool defaults)
{
    QString settingsFile(SETTINGS_FILE);
    if (!QFile::exists(settingsFile))
    {
        QFile file;
        file.setFileName(SETTINGS_FILE);
        file.open(QIODevice::ReadWrite);
        file.close();
    }

    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.sync();
    if (settings.status() == QSettings::NoError)
    {
        if (defaults)
        {
            settings.setValue("x", 0);
            settings.setValue("y", 0);
            settings.setValue("button", 0);
            return;
        }
        QString xstr = (this->ui->xLineEdit->text().compare("", Qt::CaseInsensitive) == 0) ? "0" : this->ui->xLineEdit->text();
        settings.setValue("x", xstr);

        QString ystr = (this->ui->yLineEdit->text().compare("", Qt::CaseInsensitive) == 0) ? "0" : this->ui->yLineEdit->text();
        settings.setValue("y", ystr);

        settings.setValue("button", this->ui->buttonComboBox->currentIndex());
    }
}

void MainWindow::threadFinished()
{
    thread->deleteLater();
}

void MainWindow::workerFinished(int code)
{
    working = false;
    swapActions();
    if (code != 0)
    {
        showMessage(tr("Error starting service"));
    }
    else
    {
        showMessage(tr("Dota2 click service stopped"));
    }
    worker->deleteLater();
}

void MainWindow::workerButtonClicked(int code)
{
    if (code == this->ui->buttonComboBox->currentIndex())
    {
        setMousePosition();
        leftClick();
    }
}

void MainWindow::detectCoordButtonClicked()
{
    AcceptScreen *as = new AcceptScreen(this, this->ui->xLineEdit, this->ui->yLineEdit);
    as->show();
}

void MainWindow::xLineEditTextChanged(QString s)
{
    if (s.compare("") == 0)
    {
        this->ui->xLineEdit->setText("0");
    }
}

void MainWindow::yLineEditTextChanged(QString s)
{
    if (s.compare("") == 0)
    {
        this->ui->yLineEdit->setText("0");
    }
}

void MainWindow::saveButtonClicked()
{
    saveSettings();
    showMessage("Settings saved");
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        showNormal();
        break;
    case QSystemTrayIcon::MiddleClick:
        //showMessage(tr("Click service ") + (working ? tr("running") : tr("was stopped")));
        showMessage(tr("Dima xyu"));
        break;
    default:
        ;
    }
}

void MainWindow::messageClicked()
{
    //TODO: implement
}
