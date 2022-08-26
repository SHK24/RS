#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    //Start rhdcam frames requests
    hdCamExchanger = new JsonExchanger(ip, port);
    connect(hdCamExchanger, &JsonExchanger::dataReady, this, &MainWindow::on_hdFrameReceived);
    //connect(&hdcamTimer, &QTimer::timeout, this, &MainWindow::hdcam_requestFrame);

    hdCamExchanger->request("get", "video_frame");
    hdcamTimer.singleShot(40, this, &MainWindow::hdcam_requestFrame);
}

void MainWindow::on_hdFrameReceived(QJsonObject jsonData) {

    QByteArray txt = jsonData["data"].toString().toUtf8();
    QPixmap image;
    image.loadFromData(QByteArray::fromBase64(txt));

    if(hdcamIsConnected == false) {
        addLog("INFO", "Connection OK!");
        hdcamIsConnected = true;
        watchdogTimer.stop();
    }

    //Set diod color - green
    ui->hdCamConnected_diod->setStyleSheet("background-color: rgb(63, 255, 15);border-radius: 25px;");

    //Set image to quick view widget
    ui->hdcam_view->setPixmap(image);
    ui->hdcam_view->setScaledContents( true );
    ui->hdcam_view->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    hdcamTimer.singleShot(40, this, &MainWindow::hdcam_requestFrame);
}

void MainWindow::on_hdCamError(QString error) {
    //Set diod color - red
    ui->hdCamConnected_diod->setStyleSheet("background-color: rgb(255, 63, 15);border-radius: 25px;");
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

void MainWindow::hdcam_requestFrame(){
    hdCamExchanger->request("get", "video_frame");
}

void MainWindow::disconnectHdCamHandlers() {
    disconnect(hdCamExchanger, &JsonExchanger::dataReady, this, &MainWindow::on_hdFrameReceived);
    disconnect(&hdcamTimer, &QTimer::timeout, this, &MainWindow::hdcam_requestFrame);

    delete(hdCamExchanger);
}

void MainWindow::addLog(QString level, QString message){
    QString ts = QTime::currentTime().toString();
    ui->log->append("[" + ts + "] " + level + " : " + message + "\n");
}

