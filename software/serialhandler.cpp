// SerialHandler.cpp  — minimal patch to support ESP32 + UNO safely
#include "serialhandler.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

SerialHandler::SerialHandler(QObject *parent)
    : QObject(parent),
    COMPORT(nullptr),
    buffer(nullptr)
{
    qDebug() << "Available ports:";

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        qDebug() << "  Name:" << info.portName()
        << "  Description:" << info.description()
        << "  System Location:" << info.systemLocation();

        // ------- 1. recognise boards we care about -------
        const bool isUNO   = info.description().mid(0,3) == "UNO";
        const bool isCP210 = info.description().contains("CP210", Qt::CaseInsensitive);
        const bool isCH34  = info.description().contains("CH34",  Qt::CaseInsensitive);
        if (!(isUNO || isCP210 || isCH34))
            continue;                              // not for us

        // ------- 2. open the port -------
        COMPORT = new QSerialPort(info);           // ctor sets portName
        const qint32 baud =
            isUNO ? QSerialPort::Baud19200         // your small pad
                  : QSerialPort::Baud115200;       // ESP32 default
        COMPORT->setBaudRate(baud);
        COMPORT->setParity(QSerialPort::NoParity);
        COMPORT->setDataBits(QSerialPort::Data8);
        COMPORT->setStopBits(QSerialPort::OneStop);
        COMPORT->setFlowControl(QSerialPort::NoFlowControl);

        if (!COMPORT->open(QIODevice::ReadWrite)) {
            qDebug() << "Error opening serial port:"
                     << COMPORT->errorString();
            delete COMPORT;
            COMPORT = nullptr;
            continue;      // try the next candidate port, if any
        }

        qDebug() << "Serial port opened successfully on"
                 << COMPORT->portName() << "@" << baud;
        break;             // success → stop scanning
    }

    // ------- 3. hook up readyRead only if we actually have a port -------
    if (COMPORT) {
        connect(COMPORT, &QSerialPort::readyRead,
                this,     &SerialHandler::readSerialData);
    } else {
        qDebug() << "No compatible serial device found.";
    }
}

SerialHandler::~SerialHandler()
{
    if (COMPORT) {
        if (COMPORT->isOpen())
            COMPORT->close();
        delete COMPORT;
    }
}

// ---------------- your original methods remain unchanged ----------------
void SerialHandler::readSerialData()
{
    QByteArray data = COMPORT->readAll();
    qDebug() << "Received data (raw bytes):" << data;

    QString trimmedText = QString::fromUtf8(data).trimmed();
    QByteArray butt = trimmedText.toUtf8();
    qDebug() << "Number only:" << butt;

    int number = trimmedText.toInt();
    qDebug() << "Integer:" << number;

    emit dataReceived(number);
}

void SerialHandler::sendProfile(int profileCode)
{
    if (COMPORT && COMPORT->isOpen()) {
        QByteArray data = QByteArray::number(profileCode);
        qDebug() << "Sending profile code:" << profileCode;
        COMPORT->write(data);
    } else {
        qDebug() << "Serial port not open";
    }
}
