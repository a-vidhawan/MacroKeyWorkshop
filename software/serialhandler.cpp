#include "serialhandler.h"
#include <QSerialPortInfo>
#include <QDebug>

SerialHandler::SerialHandler(QObject *parent)
    : QObject(parent),
    buffer(nullptr)
{
    qDebug() << "Available ports:";
    std::string port = "";
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        qDebug() << "  Name:" << info.portName()
        << "  Description:" << info.description()
        << "  System Location:" << info.systemLocation();

        if(info.description().mid(0,3) == "UNO")
        {
            port = "COM3";//info.portName();
            // Example configuration; adjust port name & baud rate as needed
            COMPORT = new QSerialPort();
            COMPORT->setPortName(info.portName());   // or "/dev/ttyACM0" etc.
            COMPORT->setBaudRate(QSerialPort::BaudRate::Baud19200);
            COMPORT->setParity(QSerialPort::Parity::NoParity);
            COMPORT->setDataBits(QSerialPort::DataBits::Data8);
            COMPORT->setStopBits(QSerialPort::StopBits::OneStop);
            COMPORT->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
            COMPORT->open(QIODevice::ReadWrite);

            // Try to open the port
            if (!COMPORT->isOpen()) {
                qDebug() << "Error opening serial port:" << COMPORT->error();
            } else {
                qDebug() << "Serial port opened successfully on" << COMPORT->portName();
            }
        }
    }

    connect(COMPORT, &QSerialPort::readyRead, this, &SerialHandler::readSerialData);

}

SerialHandler::~SerialHandler()
{
    // Clean up
    if (COMPORT->isOpen()) {
        COMPORT->close();
    }
}

void SerialHandler::readSerialData()
{
    QByteArray data = COMPORT->readAll();
    qDebug() << "Received data (raw bytes):" << data;

    // Convert raw bytes to QString and trim off \r, \n, or other whitespace
    QString trimmedText = QString::fromUtf8(data).trimmed();


    // If you want it back in a QByteArray named 'butt'
    QByteArray butt = trimmedText.toUtf8();
    qDebug() << "Number only:" << butt;

    int number = trimmedText.toInt();
    qDebug() << "Integer:" << number;

    // Or emit the trimmed string
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
