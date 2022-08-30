#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTimer>
#include <QScreen>

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
    void on_hdcam_responce_received(QJsonObject jsonData);

    //Errors slots
    void on_hdCamError(QString error);

    void on_hdcam_address_editingFinished();

    void on_hdcam_framerate_currentIndexChanged(int index);

    void on_hdcam_resolutionList_currentIndexChanged(int index);

    void hdcam_connectionWatchDogHandler();

    void on_hdcam_connect_clicked();
    void hdcam_readyRead();

    void on_hdcam_resolutionList_currentTextChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    JsonExchanger * hdCamExchanger;    
    QSettings * settings;

    //HDCAM
    QTimer hdcamTimer, depthcamTimer;
    QTimer watchdogTimer;
    QTcpSocket  _socket;
    QByteArray imgBuffer;
    uchar imgTmp[640*480];

    bool hdcamIsConnected = false;
    bool markerFound = false;
    unsigned int expectedSize;
    int currentWidth, currentHeight;

    //Photocam
    //QCamera * camera;
    //QMediaCaptureSession captureSession;
    //QVideoWidget * viewfinder;


    //Requests functions
    void hdcam_requestFaces();

    //Disconnect functions
    void disconnectHdCamHandlers();

    //Add log
    void addLog(QString level, QString message);

    void resizeEvent(QResizeEvent* event);
};
#endif // MAINWINDOW_H
