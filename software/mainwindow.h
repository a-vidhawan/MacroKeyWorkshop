#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QMessageBox>
#include <QQuickWidget>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "serialhandler.h"
#include "apptracker.h"

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include "shellapi.h"
#elif __APPLE__
#include <Carbon/Carbon.h>
#elif __linux__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#endif

#include "profile.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Profile> profiles READ getProfiles NOTIFY profilesChanged)
    Q_PROPERTY(Profile* profileInstance READ getProfileInstance WRITE setProfileInstance NOTIFY profileInstanceChanged)

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Q_INVOKABLE QQmlListProperty<Profile> getProfiles();

    static qsizetype profileCount(QQmlListProperty<Profile> *list);
    static Profile* profileAt(QQmlListProperty<Profile> *list, qsizetype index);
    Profile* getProfileInstance() { return profileInstance; };
    void setProfileInstance(Profile* profile);

#ifdef _WIN32
    void doTasks(std::vector<INPUT>& inputs);
    static std::unordered_map<UINT, std::function<void()>> hotkeyActions;

#endif

Q_INVOKABLE void registerGlobalHotkey(Profile* profile, int keyNum, const QString& type, const QString& content);

signals:
    void profilesChanged();
    void profileInstanceChanged();

protected:
    void closeEvent(QCloseEvent *event) override;  // Override close event to minimize to tray

private slots:
    void onDataReceived(int number);

    void showWindow();  // Restore window from system tray
    void exitApplication();  // Quit application
    void toggleDockIcon(bool show);

private:
    void createTrayIcon();
    Profile* profileInstance;
    AppTracker appTracker;
    QList<Profile*> profiles;
    Profile* currentProfile;
    SerialHandler *m_serialHandler;

    void initializeProfiles();
    void switchCurrentProfile(const QString& appName);

QQuickWidget *qmlWidget;
QSystemTrayIcon *trayIcon;
QMenu *trayMenu;

#ifdef _WIN32
    static LRESULT CALLBACK hotkeyCallback(int nCode, WPARAM wParam, LPARAM lParam);
    static HHOOK keyboardHook;
#elif __APPLE__
    OSStatus hotkeyCallback(EventHandlerCallRef nextHandler, EventRef event, void *userData);
    void executeHotkey(int hotKeyNum);
public:
    static int macVolume;
#elif __linux__
    static void listenForHotkeys();
    Display *display;
#endif

};
#endif // MAINWINDOW_H
