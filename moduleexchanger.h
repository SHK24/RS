#ifndef MODULEEXCHANGER_H
#define MODULEEXCHANGER_H

#include <QTimer>
#include <QJsonObject>
#include <QTimer>

#include "jsonexchanger.h"

#define MARKER 0x55AA55AA

class ModuleExchanger : public QObject
{
    Q_OBJECT

private:

    QString host;
    int httpPort, tcpPort;

    QTimer frameTimer;
    QStringList states;

    QTimer connectionWatchdog;
    QTimer httpWatchdog;
    QTimer tcpWatchdog;

    QTimer requestTimer;
    QTcpSocket socket;
    QByteArray tcpBuffer;
    JsonExchanger * jsonExchanger;

    bool markerFound;

    unsigned int payloadSize;

    void disconnectHandlers();

public:
    //Constructors and destructor
    ModuleExchanger();
    ModuleExchanger(QString host, int httpPort, int tcpPort);
    ~ModuleExchanger();

    //Functions
    void request(QString type, QString entity, QString value="");

    //TCP
    void tcpConnect();
    void tcpConnect(QString host, int port);
    void tcpDisconnect();

    //HTTP
    void httpConnect();
    void httpConnect(QString host, int port);
    void httpDisconnect();

signals:
    void frameReady(QByteArray frame);
    void responseReady(QJsonObject jsonData);
    void error(QString error);

    void tcpConnected();
    void tcpDisconnected();


private slots:
    void httpResponseReady(QJsonObject jsonData);
    void tcpReadyRead();

    void connection_watchDogHandler();
    void tcp_watchDogHandler();
    void http_watchDogHandler();
};

#endif // MODULEEXCHANGER_H
