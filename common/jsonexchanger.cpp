#include "jsonexchanger.h"

//JsonExchanger

const QString &JsonExchanger::getIp() const
{
    return ip;
}

void JsonExchanger::setIp(const QString &newIp)
{
    ip = newIp;
}

int JsonExchanger::getPort() const
{
    return port;
}

void JsonExchanger::setPort(int newPort)
{
    port = newPort;
}

JsonExchanger::JsonExchanger() {

}

JsonExchanger::JsonExchanger(QString ip, int port) {
    this->ip = ip;
    this->port = port;
}

int JsonExchanger::request(QString type, QString entity, QString value){
    QUrl url(this->ip + ":" + QString::number(port));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject obj;
    obj["request"] = type;
    obj["entity"] = entity;
    obj["value"] = value;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    reply = qnam.post(request, data);

    QObject::connect(reply, &QNetworkReply::finished, [=](){
        if(reply->error() == QNetworkReply::NoError){
            QString contents = QString::fromUtf8(reply->readAll());
            QJsonObject jsonData = this->parse(contents);
            this->dataReady(jsonData);
        }
        else{
            QString err = reply->errorString();
            this->error(err);
        }
        //reply->deleteLater();
    });

    //connect(reply, &QNetworkReply::finished, this, &JsonExchanger::httpFinished);
    //connect(reply, &QIODevice::readyRead, this, &JsonExchanger::httpReadyRead);

    //return 0;
}

QString JsonExchanger::httpFinished() {
    int a = 10;
}

QString JsonExchanger::httpReadyRead() {
    QByteArray replyData = reply->readAll();
    int a = 10;
}

QJsonObject JsonExchanger::parse(QString jsonData) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8());
    return doc.object();
}

//Frame Class Members

const QString &Frame::getFrameData() const
{
    return frameData;
}

void Frame::setFrameData(const QString &newFrameData)
{
    frameData = newFrameData;
}

int Frame::getWidth() const
{
    return width;
}

void Frame::setWidth(int newWidth)
{
    width = newWidth;
}

int Frame::getHeight() const
{
    return height;
}

void Frame::setHeight(int newHeight)
{
    height = newHeight;
}

Frame::Frame(){}

Frame::Frame(int height, int width, QString frameData)
{
    this->height = height;
    this->width  = width;
    this->frameData = frameData;
}
