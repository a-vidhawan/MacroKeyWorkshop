#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QMainWindow>
#include <QObject>
#include <QSerialPort>
#include <QMessageBox>
#include <QString>
#include <QDebug>

class SerialHandler : public QObject
{
    Q_OBJECT
public:
    explicit SerialHandler(QObject *parent = nullptr);
    ~SerialHandler();

    void sendProfile(int profileCode);

signals:
    //void buttonPressed(int button);
    void dataReceived(const int number);

private slots:
    void readSerialData();

private:
    QSerialPort* COMPORT;
    QByteArray buffer;
};

#endif // SERIALHANDLER_H
