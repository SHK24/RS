#ifndef JSONEXCHANGER_H
#define JSONEXCHANGER_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QtNetwork>
#include <QUrl>

class Frame {
public:
    const QString &getFrameData() const;
    void setFrameData(const QString &newFrameData);

    int getWidth() const;
    void setWidth(int newWidth);

    int getHeight() const;
    void setHeight(int newHeight);

private:
    int width, height;
    QString frameData;

    Frame();
    Frame(int height, int width, QString frameData);
};

class JsonExchanger : public QObject
{
    Q_OBJECT

    QString ip;
    int port;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;

public:

    JsonExchanger();
    JsonExchanger(QString ip, int port);
    int request(QString type, QString entity, QString value="");
    QString httpFinished();
    QString httpReadyRead();

    QJsonObject parse(QString jsonData);
    const QString &getIp() const;
    void setIp(const QString &newIp);
    int getPort() const;
    void setPort(int newPort);
signals:
    void dataReady(QJsonObject jsonData);
    void error(QString message);
};

#endif // JSONEXCHANGER_H
