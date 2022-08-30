#include "moduleexchanger.h"

ModuleExchanger::ModuleExchanger() {

}

ModuleExchanger::ModuleExchanger(QString host, int httpPort, int tcpPort) {
    this->host = host;
    this->httpPort = httpPort;
    this->tcpPort = tcpPort;

    jsonExchanger = new JsonExchanger("http://" + host, httpPort);

    connect(jsonExchanger, &JsonExchanger::dataReady, this, &ModuleExchanger::httpResponseReady);

    connect(&httpWatchdog, &QTimer::timeout, this, &ModuleExchanger::http_watchDogHandler);
    connect(&tcpWatchdog,  &QTimer::timeout, this, &ModuleExchanger::tcp_watchDogHandler);
    connect(&connectionWatchdog, &QTimer::timeout, this, &ModuleExchanger::connection_watchDogHandler);
}
ModuleExchanger::~ModuleExchanger() {
    delete(jsonExchanger);
}

void ModuleExchanger::request(QString type, QString entity, QString value) {
    jsonExchanger->request(type, entity, value);

    httpWatchdog.start(1000);
}

void ModuleExchanger::tcpConnect() {
    socket.connectToHost(QHostAddress(this->host), this->tcpPort);
    connect(&socket, SIGNAL(readyRead()), this, SLOT(tcpReadyRead()));
    socket.write(QByteArray(4, '1'));

    tcpWatchdog.start(1000);
}

void ModuleExchanger::httpResponseReady(QJsonObject jsonData) {
    httpWatchdog.stop();

    responseReady(jsonData);
}

void ModuleExchanger::tcpReadyRead() {
    QByteArray data = socket.readAll();

    tcpBuffer.append(data);


    if(!markerFound) {

        unsigned int * marker = nullptr;
        unsigned int markerPos = 0;
        payloadSize = 0;

        marker = (unsigned int*)tcpBuffer.data();

        while(*marker != 0x55AA55AA) {
            marker = (unsigned int*)tcpBuffer.data() + markerPos;
            markerPos += 4;

            if(markerPos > (unsigned int)tcpBuffer.count()) {
                return;
            }
        }

        tcpBuffer.remove(0, markerPos + 4);

        markerFound = true;
    }

    if(tcpBuffer.count() < 4)
        return;

    if(payloadSize == 0) {
        payloadSize = *(unsigned int*)tcpBuffer.data();
        tcpBuffer.remove(0, 4);
    }

    if(tcpBuffer.count() < payloadSize)
        return;


    markerFound = false;

    frameReady(tcpBuffer);

    tcpBuffer.remove(0, payloadSize);

    tcpWatchdog.stop();
    tcpWatchdog.start(1000);

    socket.write(QByteArray(4, '1111'));
    socket.flush();
}

void ModuleExchanger::connection_watchDogHandler() {

}

void ModuleExchanger::tcp_watchDogHandler() {
    error("TCP connection error!");
    tcpWatchdog.stop();
}

void ModuleExchanger::http_watchDogHandler() {
    error("Http connection error!");
    httpWatchdog.stop();
}
