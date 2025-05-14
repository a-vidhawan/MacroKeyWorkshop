#include <QImage>
#include <QSerialPort>
#include <QDebug>

void sendRGBImageToESP32(const QString &portName, const QString &imagePath, uint8_t profile, uint8_t imageSlot) {
    QImage image(imagePath);
    if (image.isNull()) {
        qDebug() << "Failed to load image:" << imagePath;
        return;
    }

    image = image.convertToFormat(QImage::Format_RGB888);
    if (image.width() != 64 || image.height() != 64) {
        image = image.scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    QByteArray imageBytes;
    const uchar* pixelData = image.constBits();
    imageBytes.append(reinterpret_cast<const char*>(pixelData), 64 * 64 * 3);

    QSerialPort serial;
    serial.setPortName(portName);
    serial.setBaudRate(QSerialPort::Baud115200);
    if (!serial.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open serial port:" << portName;
        return;
    }

    QByteArray packet;
    packet.append(char(47));               // START
    packet.append(char(profile));          // Profile
    packet.append(char(imageSlot));        // Image index
    packet.append(imageBytes);             // RGB bytes
    packet.append(char(47));               // END

    serial.write(packet);
    if (!serial.waitForBytesWritten(5000)) {
        qDebug() << "Timeout writing to ESP32";
    } else {
        qDebug() << "Sent RGB image to ESP32: Profile" << profile << "Image" << imageSlot;
    }

    serial.close();
}