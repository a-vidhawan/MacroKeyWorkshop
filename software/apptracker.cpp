#include "AppTracker.h"
#include <windows.h>

static AppTracker *instance = nullptr;  // Static instance for callback
static HWINEVENTHOOK winEventHook = nullptr;  // Windows event hook handle

void CALLBACK handleWinEvent(HWINEVENTHOOK, DWORD, HWND hwnd, LONG, LONG, DWORD, DWORD) {
    if (instance && hwnd) {
        char windowTitle[256];
        GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));

        QString appName = QString::fromLocal8Bit(windowTitle).trimmed();
        if (!appName.isEmpty()) {
            emit instance->appChanged(appName);
        }
    }
}

AppTracker::AppTracker(QObject *parent) : QObject(parent) {
    instance = this;
    startTracking();
}

AppTracker::~AppTracker() {
    stopTracking();
}

void AppTracker::startTracking() {
    winEventHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
        NULL,
        handleWinEvent,
        0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
        );
}

void AppTracker::stopTracking() {
    if (winEventHook) {
        UnhookWinEvent(winEventHook);
        winEventHook = nullptr;
    }
}
