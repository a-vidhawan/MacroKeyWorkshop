#ifndef CONFIG_H
#define CONFIG_H

#include <filesystem>

#ifdef _WIN32
//
#elif __APPLE__
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#elif __linux__
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

#include <QString>
#include <QObject>

class Config : public QObject {
    Q_OBJECT

public:
    Config() = delete;  // Prevent instantiation
    ~Config() {};

    static Q_INVOKABLE QString getConfigDir();  // Static method accessible from QML
};

#endif // CONFIG_H
