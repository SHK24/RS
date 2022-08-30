#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTimer>
#include <QScreen>

#include "moduleexchanger.h"


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

    //Errors slots
    void on_hdCamError(QString error);

    void on_hdcam_address_editingFinished();

    void on_hdcam_framerate_currentIndexChanged(int index);

    void on_hdcam_resolutionList_currentIndexChanged(int index);

    void hdcam_connectionWatchDogHandler();

    void on_hdcam_resolutionList_currentTextChanged(const QString &arg1);

    void on_hdcam_connect_clicked(bool checked);

    void on_depthcam_connect_clicked(bool checked);

    //HDCAM Get frames and messages slots
    void hdcam_frameReady(QByteArray frame);
    void on_hdcam_response_ready(QJsonObject jsonData);

    //DEPTHCAM Get frames and messages slots
    void depthcam_frameReady(QByteArray frame);
    void on_depthcam_response_ready(QJsonObject jsonData);

    void on_depthcam_address_editingFinished();

private:
    Ui::MainWindow *ui;

    ModuleExchanger * hdcamExchanger;
    ModuleExchanger * depthcamExchanger;

    QSettings * settings;

    //HDCAM
    QByteArray imgBuffer;
    uchar imgTmp[640*480];

    bool hdcamIsConnected = false;

    QTimer faceCountRequestTimer;
    QTimer sceletonCountRequestTimer;

    bool depthCamConnected;

    //Photocam
    //QCamera * camera;
    //QMediaCaptureSession captureSession;
    //QVideoWidget * viewfinder;


    //Requests functions
    void hdcam_faceCountRequest();
    void depthcam_sceletonCountRequest();

    //Disconnect functions
    void disconnectHdCamHandlers();

    //Add log
    void addLog(QString level, QString message);

    void resizeEvent(QResizeEvent* event);
};
#endif // MAINWINDOW_H
