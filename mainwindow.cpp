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
    int screenHeight = screenGeometry.height();

    depthCamConnected = false;

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

//    ui->map_main_view->setMinimumWidth(screenWidth * 0.3);
//    ui->map_main_view->setMaximumWidth(screenWidth * 0.3);

    ui->hdcam_main_view->setMinimumWidth(screenWidth * 0.3);
    ui->hdcam_main_view->setMaximumWidth(screenWidth * 0.3);
    ui->hdcam_main_view->setMinimumHeight(screenHeight * 0.3);
    ui->hdcam_main_view->setMaximumHeight(screenHeight * 0.3);

    ui->depthcam_main_view->setMinimumWidth(screenWidth * 0.3);
    ui->depthcam_main_view->setMaximumWidth(screenWidth * 0.3);
    ui->depthcam_main_view->setMinimumHeight(screenHeight * 0.3);
    ui->depthcam_main_view->setMaximumHeight(screenHeight * 0.3);

    ui->thermalcam_main_view->setMinimumWidth(screenWidth * 0.3);
    ui->thermalcam_main_view->setMaximumWidth(screenWidth * 0.3);
    ui->thermalcam_main_view->setMinimumHeight(screenHeight * 0.3);
    ui->thermalcam_main_view->setMaximumHeight(screenHeight * 0.3);

    //Load settings
    settings = new QSettings("settings.ini", QSettings::IniFormat);

    ui->hdcam_address->setText(settings->value("HD_CAM_ADDRESS", "http://127.0.0.1:8080").toString());
    ui->hdcam_resolutionList->setCurrentIndex(settings->value("HD_CAM_RESOLUTION", 0).toInt());
    ui->hdcam_framerate->setCurrentIndex(settings->value("HD_CAM_FRAMERATE", 0).toInt());

    ui->depthcam_address->setText(settings->value("DEPTH_CAM_ADDRESS", "http://127.0.0.1:8081").toString());
}

MainWindow::~MainWindow()
{
    delete ui;
}

//UI Handlers

void MainWindow::on_hdcam_response_ready(QJsonObject jsonData) {

    QByteArray txt = jsonData["data"].toString().toUtf8();

    //Set diod color - green

    if(jsonData["entity"].toString() == "status") {

        addLog("INFO", "HDCAM Connection - OK!");

        ui->hdcam_connected_diod->setStyleSheet("background-color: rgb(63, 255, 15);border-radius: 25px;");

        int width = jsonData["video_mode"].toArray()[0].toInt();
        int height = jsonData["video_mode"].toArray()[1].toInt();
        hdcamExchanger->request("get", "modes");

        addLog("INFO", "Current videomode is " + QString::number(width) + " " + QString::number(height));
    }

    if(jsonData["entity"].toString() == "modes") {

        disconnect(ui->hdcam_resolutionList, &QComboBox::currentTextChanged, this, &MainWindow::on_hdcam_resolutionList_currentTextChanged);

        for(auto mode : jsonData["modes"].toArray()) {

            ui->hdcam_resolutionList->addItem(QString::number(mode.toArray()[0].toInt())+"x"+QString::number(mode.toArray()[1].toInt()));
        }

        connect(ui->hdcam_resolutionList, &QComboBox::currentTextChanged, this, &MainWindow::on_hdcam_resolutionList_currentTextChanged);

        addLog("INFO", "Receiving supported modes - OK");

        hdcamExchanger->tcpConnect();
        faceCountRequestTimer.singleShot(40, this, &MainWindow::hdcam_faceCountRequest);
    }

    if(jsonData["entity"].toString() == "face_count") {
        ui->face_count_value->setText(jsonData["face_count"].toString());
    }
}

void MainWindow::depthcam_frameReady(QByteArray frame) {
    QPixmap mpixmap;

    qDebug() << "Received DEPTHCAM frame";

    unsigned int width = *(unsigned short*)frame.data();
    unsigned int height = *(unsigned short*)(frame.data() + 2);

    const uchar * imgPtr = (uchar*)(frame.data() + 4);

    QImage image(imgPtr, width, height, QImage::Format::Format_BGR888);

    ui->depthcam_view->setPixmap(QPixmap::fromImage(image));
    ui->depthcam_main_view->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::on_depthcam_response_ready(QJsonObject jsonData) {

    ui->depthcam_connected_diod->setStyleSheet("background-color: rgb(63, 255, 15);border-radius: 25px;");

    if(!depthCamConnected) {
        addLog("INFO", "Depth Cam Connection - OK!");
        depthCamConnected = true;
    }

    if(jsonData["entity"].toString() == "sceleton_count") {
        ui->sceleton_count_value->setText(jsonData["sceleton_count"].toString());
    }
}

void MainWindow::on_hdCamError(QString error) {
    //Set diod color - red
    ui->hdcam_connected_diod->setStyleSheet("background-color: rgb(255, 63, 15);border-radius: 25px;");
    ui->hdcam_view->setStyleSheet("background-color: rgb(0, 0, 0);");
    addLog("ERROR", error);
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

void MainWindow::hdcam_faceCountRequest(){
    hdcamExchanger->request("get", "face_count");
    faceCountRequestTimer.singleShot(40, this, &MainWindow::hdcam_faceCountRequest);
}

void MainWindow::depthcam_sceletonCountRequest() {
    depthcamExchanger->request("get", "sceleton_count");
    sceletonCountRequestTimer.singleShot(40, this, &MainWindow::depthcam_sceletonCountRequest);
}

void MainWindow::disconnectHdCamHandlers() {
    disconnect(hdcamExchanger, &ModuleExchanger::frameReady, this, &MainWindow::hdcam_frameReady);
    disconnect(hdcamExchanger, &ModuleExchanger::responseReady, this, &MainWindow::on_hdcam_response_ready);
    disconnect(hdcamExchanger, &ModuleExchanger::error, this, &MainWindow::on_hdCamError);
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


void MainWindow::hdcam_frameReady(QByteArray frame) {

    QPixmap mpixmap;

    qDebug() << "Received HDCAM frame";

    unsigned int width = *(unsigned short*)frame.data();
    unsigned int height = *(unsigned short*)(frame.data() + 2);

    const uchar * imgPtr = (uchar*)(frame.data() + 4);

    QImage image(imgPtr, width, height, QImage::Format::Format_BGR888);

    ui->hdcam_view->setScaledContents( true );
    ui->hdcam_view->setPixmap(QPixmap::fromImage(image).scaled(ui->hdcam_view->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    ui->hdcam_main_view->setScaledContents( true );
    ui->hdcam_main_view->setPixmap(QPixmap::fromImage(image).scaled(ui->hdcam_main_view->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
}


void MainWindow::on_hdcam_resolutionList_currentTextChanged(const QString &arg1) {
    hdcamExchanger->request("set", "video_mode", arg1);
}


void MainWindow::on_hdcam_connect_clicked(bool checked)
{
    if(checked) {
        QStringList addressParts = ui->hdcam_address->text().split(':');

        QString ip = addressParts[0];
        int port = addressParts[1].toInt();

        addLog("INFO", "Trying to connect to HDCAM...");

        hdcamExchanger = new ModuleExchanger(ip, port, 1234);

        connect(hdcamExchanger, &ModuleExchanger::frameReady, this, &MainWindow::hdcam_frameReady);
        connect(hdcamExchanger, &ModuleExchanger::responseReady, this, &MainWindow::on_hdcam_response_ready);
        connect(hdcamExchanger, &ModuleExchanger::error, this, &MainWindow::on_hdCamError);
        hdcamExchanger->request("get", "status");
    }
    else {
        disconnectHdCamHandlers();
        delete(hdcamExchanger);
    }
}


void MainWindow::on_depthcam_connect_clicked(bool checked)
{
    if(checked) {
        QStringList addressParts = ui->depthcam_address->text().split(':');

        QString ip = addressParts[0];
        int port = addressParts[1].toInt();

        addLog("INFO", "Trying to connect to DepthCam...");

        depthcamExchanger = new ModuleExchanger(ip, port, 1235);

        connect(depthcamExchanger, &ModuleExchanger::frameReady, this, &MainWindow::depthcam_frameReady);
        connect(depthcamExchanger, &ModuleExchanger::responseReady, this, &MainWindow::on_depthcam_response_ready);

        depthcamExchanger->tcpConnect();
        faceCountRequestTimer.singleShot(40, this, &MainWindow::depthcam_sceletonCountRequest);
        //connect(depthcamExchanger, &ModuleExchanger::error, this, &MainWindow::on_hdCamError);
        //depthcamExchanger->request("get", "status");


    }
    else {
        disconnectHdCamHandlers();
        delete(hdcamExchanger);
    }
}


void MainWindow::on_depthcam_address_editingFinished() {
    settings->setValue("DEPTH_CAM_ADDRESS", ui->depthcam_address->text());
}

