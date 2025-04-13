#include "fileio.h"
#include <QDebug>
#include <QUrl>
#include <QFileInfo>
#include <QDir>


FileIO::FileIO(QObject *parent) : QObject(parent) {}

QString FileIO::filePath() const {
    return m_filePath;
}

void FileIO::setFilePath(const QString &path) {
    if (m_filePath != path) {
        m_filePath = path;
        emit filePathChanged();
    }
}

QString FileIO::read() {
    if (m_filePath.isEmpty()) return "";

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "";

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    return content;
}


bool FileIO::write(const QString &data) {
    if (m_filePath.isEmpty()) {
        qWarning() << "ERROR: File path is empty!";
        return false;
    }

    QString localPath = QUrl(m_filePath).toLocalFile();

    QFileInfo fileInfo(localPath);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "ERROR: Failed to create directory for file:" << localPath;
            return false;
        }
    }

    QFile file(localPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "ERROR: Cannot open file for writing:" << localPath;
        return false;
    }

    QTextStream out(&file);
    out << data;
    file.close();

    qDebug() << "File written successfully:" << localPath;
    return true;
}
