#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();

    int screenWidth = screenGeometry.width();

//    //Open photocam
//    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
//    for (const QCameraDevice &cameraDevice : cameras) {
//        if (cameraDevice.description() == "UVC Camera (046d:0825)")
//            camera = new QCamera(cameraDevice);
//    }

//    captureSession.setCamera(camera);
//    //captureSession.setVideoOutput(ui->graphicsView);

//    viewfinder = new QVideoWidget;
//    captureSession.setVideoOutput(viewfinder);
//    viewfinder->show();

//    camera->start();

#ifndef HDCAM
    ui->hdcam_label->setVisible(false);
    ui->hdcam_addr_label->setVisible(false);
    ui->hdcam_address->setVisible(false);
    ui->hdcam_connect->setVisible(false);
    ui->hdcam_connected_diod->setVisible(false);

    ui->hdcam_view->setVisible(false);

    ui->hdcam_settings->setVisible(false);
    ui->hdcam_frameRate_label->setVisible(false);
    ui->hdcam_framerate->setVisible(false);
    ui->hdcam_resolutionList->setVisible(false);
    ui->hdcam_resolution_label->setVisible(false);
#endif

#ifndef DEPTHCAM
    ui->depthcam_label->setVisible(false);
    ui->depthCam_addr_label->setVisible(false);
    ui->depthcam_address->setVisible(false);
    ui->depthcam_connect->setVisible(false);
    ui->depthcam_connected_diod->setVisible(false);

    ui->depthcam_view->setVisible(false);

    ui->depthcam_settings->setVisible(false);
    ui->depthcam_frameRate_label->setVisible(false);
    ui->depthcam_framerate->setVisible(false);
    ui->depthcam_resolutionList->setVisible(false);
    ui->depthcam_resolution_label->setVisible(false);
#endif

#ifndef THERMALCAM
    ui->thermalcam_label->setVisible(false);
    ui->thermalcam_addr_label->setVisible(false);
    ui->thermalcam_address->setVisible(false);
    ui->thermalcam_connect->setVisible(false);
    ui->thermalcam_connected_diod->setVisible(false);

    ui->thermalcam_view->setVisible(false);

    ui->thermalcam_settings->setVisible(false);
    ui->thermalcam_frameRate_label->setVisible(false);
    ui->thermalcam_framerate->setVisible(false);
    ui->thermalcam_resolutionList->setVisible(false);
    ui->thermalcam_resolution_label->setVisible(false);
#endif

    ui->communicationSetting_groupBox->setMinimumWidth(screenWidth / 2);
    ui->communicationSetting_groupBox->setMaximumWidth(screenWidth / 2);

    ui->hdcam_view->setMinimumWidth(screenWidth * 0.3);
    ui->hdcam_view->setMaximumWidth(screenWidth * 0.3);

    ui->depthcam_view->setMinimumWidth(screenWidth * 0.3);
    ui->depthcam_view->setMaximumWidth(screenWidth * 0.3);

    ui->thermalcam_view->setMinimumWidth(screenWidth * 0.3);
    ui->thermalcam_view->setMaximumWidth(screenWidth * 0.3);


//    QSize maxResolution(0,0);
//    for(auto resolution : camera->cameraDevice().photoResolutions()) {
//        if((resolution.width() > maxResolution.width()) && (resolution.width() > maxResolution.width()))
//            maxResolution.setWidth(resolution.width());
//            maxResolution.setHeight(resolution.height());
//    }

    //Load settings
    settings = new QSettings("settings.ini", QSettings::IniFormat);

    ui->hdcam_address->setText(settings->value("HD_CAM_ADDRESS", "http://127.0.0.1:8080").toString());
    ui->hdcam_resolutionList->setCurrentIndex(settings->value("HD_CAM_RESOLUTION", 0).toInt());
    ui->hdcam_framerate->setCurrentIndex(settings->value("HD_CAM_FRAMERATE", 0).toInt());
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_setHDcamAddr_clicked()
{
    QStringList addressParts = ui->hdcam_address->text().split(':');

    QString ip = addressParts[0] + ':' + addressParts[1];
    int port = addressParts[2].toInt();

    addLog("INFO", "Trying connection to HDCAM...");

    //Setting of watchdog timer
    connect(&watchdogTimer, &QTimer::timeout, this, &MainWindow::hdcam_connectionWatchDogHandler);
    watchdogTimer.setSingleShot(true);
    watchdogTimer.start(1000);

    //Start HDCAM http requests

    hdCamExchanger = new JsonExchanger(ip, port);
    connect(hdCamExchanger, &JsonExchanger::dataReady, this, &MainWindow::on_hdcam_responce_received);
    connect(&hdcamTimer, &QTimer::timeout, this, &MainWindow::hdcam_requestFaces);

    //hdCamExchanger->request("get", "status");
    hdCamExchanger->request("get", "modes");
    //hdcamTimer.singleShot(1000, this, &MainWindow::hdcam_requestFaces);

      //Start HDCAM frames requests
      _socket.connectToHost(QHostAddress("127.0.0.1"), 65433);
      connect(&_socket, SIGNAL(readyRead()), this, SLOT(hdcam_readyRead()));
      //_socket.write(QByteArray(3, 'A'));
}

void MainWindow::on_hdcam_responce_received(QJsonObject jsonData) {

    QByteArray txt = jsonData["data"].toString().toUtf8();

    //Set diod color - green
    ui->hdcam_connected_diod->setStyleSheet("background-color: rgb(63, 255, 15);border-radius: 25px;");

    if(hdcamIsConnected == false) {
        addLog("INFO", "Connection OK!");
        hdcamIsConnected = true;
        watchdogTimer.stop();
    }

    if(jsonData["entity"].toString() == "status") {
        currentWidth = jsonData["video_mode"].toArray()[0].toInt();
        currentHeight = jsonData["video_mode"].toArray()[1].toInt();
        hdCamExchanger->request("get", "modes");

        addLog("INFO", "Current videomode is " + QString::number(currentWidth) + " " + QString::number(currentHeight));
    }

    if(jsonData["entity"].toString() == "modes") {
        for(auto mode : jsonData["modes"].toArray()) {

            ui->hdcam_resolutionList->addItem(QString::number(mode.toArray()[0].toInt())+"x"+QString::number(mode.toArray()[1].toInt()));
        }

        connect(ui->hdcam_resolutionList, &QComboBox::currentTextChanged, this, &MainWindow::on_hdcam_resolutionList_currentTextChanged);

        addLog("INFO", "Receiving supported modes - OK");
        //hdcamTimer.singleShot(40, this, &MainWindow::hdcam_requestFaces);

        _socket.connectToHost(QHostAddress("127.0.0.1"), 65433);
        connect(&_socket, SIGNAL(readyRead()), this, SLOT(hdcam_readyRead()));
        _socket.write(QByteArray(3, 'A'));
    }

    if(jsonData["entity"].toString() == "face_count") {
        ui->face_count_value->setText(jsonData["face_count"].toString());
    }
}

void MainWindow::on_hdCamError(QString error) {
    //Set diod color - red
    ui->hdcam_connected_diod->setStyleSheet("background-color: rgb(255, 63, 15);border-radius: 25px;");
    ui->hdcam_view->setStyleSheet("background-color: rgb(0, 0, 0);");
    addLog("ERROR", error);

    hdcamIsConnected = false;
    hdcamTimer.stop();

    disconnectHdCamHandlers();
}


void MainWindow::on_hdcam_address_editingFinished(){
    settings->setValue("HD_CAM_ADDRESS", ui->hdcam_address->text());
}


void MainWindow::on_hdcam_framerate_currentIndexChanged(int index){
    settings->setValue("HD_CAM_FRAMERATE", index);
}


void MainWindow::on_hdcam_resolutionList_currentIndexChanged(int index){
    settings->setValue("HD_CAM_RESOLUTION", index);
}

void MainWindow::hdcam_connectionWatchDogHandler(){

    disconnectHdCamHandlers();
    addLog("ERROR", "HDCam connection error!");
}

void MainWindow::hdcam_requestFaces(){
    hdCamExchanger->request("get", "face_count");
    hdcamTimer.singleShot(40, this, &MainWindow::hdcam_requestFaces);
}

void MainWindow::disconnectHdCamHandlers() {
    disconnect(hdCamExchanger, &JsonExchanger::dataReady, this, &MainWindow::on_hdcam_responce_received);
    disconnect(&hdcamTimer, &QTimer::timeout, this, &MainWindow::hdcam_requestFaces);

    delete(hdCamExchanger);
}

void MainWindow::addLog(QString level, QString message){
    QString ts = QTime::currentTime().toString();
    ui->log->append("[" + ts + "] " + level + " : " + message + "\n");
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();

    int screenWidth = screenGeometry.width();

    ui->communicationSetting_groupBox->setMinimumWidth(screenWidth / 2);
    ui->communicationSetting_groupBox->setMaximumWidth(screenWidth / 2);

    ui->hdcam_view->setMinimumWidth(screenWidth * 0.3);
    ui->hdcam_view->setMaximumWidth(screenWidth * 0.3);

    ui->depthcam_view->setMinimumWidth(screenWidth * 0.3);
    ui->depthcam_view->setMaximumWidth(screenWidth * 0.3);

    ui->thermalcam_view->setMinimumWidth(screenWidth * 0.3);
    ui->thermalcam_view->setMaximumWidth(screenWidth * 0.3);

}


void MainWindow::on_hdcam_connect_clicked()
{
    QStringList addressParts = ui->hdcam_address->text().split(':');

    QString ip = addressParts[0] + ':' + addressParts[1];
    int port = addressParts[2].toInt();

    addLog("INFO", "Trying connection to HDCAM...");

    //Setting of watchdog timer
    //connect(&watchdogTimer, &QTimer::timeout, this, &MainWindow::hdcam_connectionWatchDogHandler);
    //watchdogTimer.setSingleShot(true);
    //watchdogTimer.start(1000);

    //Start rhdcam frames requests
    hdCamExchanger = new JsonExchanger(ip, port);
    connect(hdCamExchanger, &JsonExchanger::dataReady, this, &MainWindow::on_hdcam_responce_received);
    disconnect(ui->hdcam_resolutionList, &QComboBox::currentTextChanged, this, &MainWindow::on_hdcam_resolutionList_currentTextChanged);

    hdCamExchanger->request("get", "status");
}

void MainWindow::hdcam_readyRead() {
    QByteArray data = _socket.readAll();

    imgBuffer.append(data);

    QPixmap mpixmap;

    if(!markerFound) {

        unsigned int * marker = nullptr;
        unsigned int markerPos = 0;
        expectedSize = 0;

        marker = (unsigned int*)imgBuffer.data();

        while(*marker != 0x55AA55AA) {
            marker = (unsigned int*)imgBuffer.data() + markerPos;
            markerPos += 4;

            if(markerPos > (unsigned int)imgBuffer.count()) {
                //imgBuffer.remove(0, markerPos + 4);
                //_socket.write(QByteArray(3, 'A'));
                return;
            }
        }

        imgBuffer.remove(0, markerPos + 4);
        currentWidth = *(unsigned short*)imgBuffer.data();
        currentHeight = *(unsigned short*)(imgBuffer.data() + 2);
        expectedSize = currentWidth * currentHeight * 3;

        markerFound = true;
        imgBuffer.remove(0, 4);
    }

    if(imgBuffer.count() < expectedSize)
        return;

    qDebug() << "Received frame";

    markerFound = false;

    const uchar * imgPtr = (uchar*)imgBuffer.data();

    QImage image(imgPtr, currentWidth, currentHeight, QImage::Format::Format_BGR888);

    ui->hdcam_view->setPixmap(QPixmap::fromImage(image));
    ui->hdcam_main_view->setPixmap(QPixmap::fromImage(image));

    //ui->hdcam_view->setScaledContents( true );
    //ui->hdcam_view->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    //ui->hdcam_main_view->setScaledContents( true );
    //ui->hdcam_main_view->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    int size = imgBuffer.count();
    imgBuffer.remove(0, expectedSize);

    int size2 = imgBuffer.count();
    qDebug() << "Request frame";
    _socket.write(QByteArray(3, 'A'));
    _socket.flush();
}


void MainWindow::on_hdcam_resolutionList_currentTextChanged(const QString &arg1)
{
    hdCamExchanger->request("set", "video_mode", arg1);
    currentWidth = arg1.split('x')[0].toInt();
    currentHeight = arg1.split('x')[1].toInt();
}

