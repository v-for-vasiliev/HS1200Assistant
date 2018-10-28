#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMessageBox>
#include <QSettings>
#include <QThread>

#include "usbworker.h"
#include "acceptscreen.h"
#include <Windows.h>

#define     SETTINGS_FILE       "settings.ini"

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
    void closeEvent(QCloseEvent *event);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage(QString text);
    void messageClicked();
    void saveButtonClicked();
    void detectCoordButtonClicked();
    void xLineEditTextChanged(QString s);
    void yLineEditTextChanged(QString s);

    void processWorker();
    void stopWorker();
    void workerFinished(int code);
    void workerButtonClicked(int code);
    void threadFinished();
    
private:
    Ui::MainWindow *ui;
    USBWorker* worker;
    QThread* thread;
    bool working;

    void createActions();
    void swapActions();
    void createTrayIcon();
    void setIcons();
    void createButtonCombo();
    void setupLineEdits();
    void leftClick();
    void setMousePosition();
    void loadSettings();
    void saveSettings(bool defaults = false);

    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *processWorkerAction;
    QAction *stopWorkerAction;
};

#endif // MAINWINDOW_H
