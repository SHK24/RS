#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTimer>

#include "jsonexchanger.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_setHDcamAddr_clicked();

    //Get Frames Slots
    void on_hdFrameReceived(QJsonObject jsonData);

    //Errors slots
    void on_hdCamError(QString error);

    void on_hdcam_address_editingFinished();

    void on_hdcam_framerate_currentIndexChanged(int index);

    void on_hdcam_resolutionList_currentIndexChanged(int index);

    void hdcam_connectionWatchDogHandler();

private:
    Ui::MainWindow *ui;
    JsonExchanger * hdCamExchanger;    
    QSettings * settings;

    QTimer hdcamTimer, depthcamTimer;
    QTimer watchdogTimer;

    bool hdcamIsConnected = false;

    //Requests functions
    void hdcam_requestFrame();

    //Disconnect functions
    void disconnectHdCamHandlers();

    //Add log
    void addLog(QString level, QString message);
};
#endif // MAINWINDOW_H
