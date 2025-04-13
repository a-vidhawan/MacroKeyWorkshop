#include <QApplication>
#include "mainwindow.h"
#include "apptracker.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();

    AppTracker appTracker;

    // Connect the signal emitted by AppTracker when the active app changes
    QObject::connect(&appTracker, &AppTracker::appChanged, [](const QString &appName) {
        // This lambda function will be called whenever the app changes
        qDebug() << "Active app changed to:" << appName;
    });

    return a.exec();
}

