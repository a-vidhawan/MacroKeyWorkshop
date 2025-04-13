#ifndef FILEIO_H
#define FILEIO_H

#include <QObject>
#include <QFile>
#include <QTextStream>

class FileIO : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)

public:
    explicit FileIO(QObject *parent = nullptr);

    QString filePath() const;
    void setFilePath(const QString &path);

    Q_INVOKABLE QString read();
    Q_INVOKABLE bool write(const QString &data);

signals:
    void filePathChanged();

private:
    QString m_filePath;
};

#endif // FILEIO_H
