#include "profile.h"
#include <iostream>
#include <fstream>
#include <memory>
#include "config.h"
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDir>


Profile::Profile(QObject* parent) : QObject(parent) {}

Profile::Profile(const QString& userName, QObject* parent) : QObject(parent), name(userName) {}

QString Profile::getName() const {
    return name;
}


void Profile::setName(const QString& newName) {
    if (name != newName) {
        name = newName;
        emit nameChanged();
    }
}

QString Profile::getApp() const {
    return application;
}

void Profile::setApp(const QString& newApp) {
    if (application != newApp) {
        application = newApp;
        emit appChanged();
    }
}


void Profile::setMacro(int keyNum, const QString& type, const QString& content) {
    macros[keyNum] = QSharedPointer<Macro>::create(type, content);
}


void Profile::deleteMacro(int keyNum) {
    macros.remove(keyNum);
}

QSharedPointer<Macro> Profile::getMacro(int keyNum) {
    return macros.value(keyNum, QSharedPointer<Macro>());
}

void Profile::saveProfile() {
    QString configDir = Config::getConfigDir();
    QString filePath = configDir + "/" + name + ".txt";

    QFile outFile(filePath);
    if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&outFile);
        out << "Name: " << name << "\n";
        out << "Application: " << application << "\n";

        for (auto it = macros.begin(); it != macros.end(); ++it) {
            out << it.key() << ":\n";
            out << "type: " << it.value()->getType() << "\n";
            out << "content: " << it.value()->getContent() << "\n";
        }

        outFile.close();
    } else {
        qCritical() << "Unable to open file for writing:" << filePath;
    }
}

Profile* Profile::loadProfile(const QString& nameLookUp) {
    QString filePath = Config::getConfigDir() + "/" + nameLookUp + ".txt";
    QFile inFile(filePath);

    if (inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&inFile);
        QString line;
        QString profileName;
        QString application;
        QString macroType;
        QString macroContent;
        int keyNum = -1;

        line = in.readLine();
        if (line.startsWith("Name: ")) {
            profileName = line.mid(6);
        } else {
            qWarning() << "Missing profile name!";
            return new Profile("");
        }

        Profile* userProfile = new Profile(profileName);

        if (line.startsWith("Application: ")){
            application = line.mid(13);
        }

        while (!in.atEnd()) {
            line = in.readLine();

            if (line[0].isDigit()) {
                keyNum = line[0].digitValue();
            }
            else if (line.startsWith("type: ")) {
                macroType = line.mid(6);
            }
            else if (line.startsWith("content: ")) {
                macroContent = line.mid(9);
                if (keyNum != -1) {
                    userProfile->setMacro(keyNum, macroType, macroContent);
                    keyNum = -1;
                }
            }
        }

        inFile.close();
        return userProfile;

    } else {
        qWarning() << "Unable to open file for reading:" << filePath;
        return new Profile("");
    }
}

void Profile::printMacros() {
    for (auto it = macros.constBegin(); it != macros.constEnd(); ++it) {
        qDebug() << "Key:" << it.key() << "Macro:" << it.value()->toString();
    }
}
