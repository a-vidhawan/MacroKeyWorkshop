#include "mainwindow.h"
#include "fileio.h"
#include "QApplication"
#include <QQmlEngine>
#include "QIcon"
#include <QAction>
#include <QVBoxLayout>
#include <QMenu>
#include <QQuickItem>
#include <QScrollArea>
#include <QDebug>
#include <thread>
#include "profile.h"


#ifdef _WIN32

HHOOK MainWindow::keyboardHook = nullptr;
#endif

#ifdef __APPLE__
#include "macvolume.h"
int MainWindow::macVolume = getSystemVolume();
#endif

QList<Profile*> profiles;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), trayIcon(new QSystemTrayIcon(this)), trayMenu(new QMenu(this)), m_serialHandler(new SerialHandler(this)){

    setWindowTitle("MacroPad - Configuration");

    // Create QQuickWidget to display QML
    qmlWidget = new QQuickWidget(this);
    qmlWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

    FileIO *fileIO = new FileIO(this);
    Macro *macro = new Macro(this);

    qmlRegisterType<FileIO>("FileIO", 1, 0, "FileIO");
    qmlRegisterType<Macro>("Macro", 1, 0, "Macro");

    initializeProfiles();

    // Register with QML
    qmlWidget->engine()->rootContext()->setContextProperty("fileIO", fileIO);
    qmlWidget->engine()->rootContext()->setContextProperty("Macro", macro);
    qmlWidget->engine()->rootContext()->setContextProperty("profileInstance", profileInstance);
    qmlWidget->engine()->rootContext()->setContextProperty("mainWindow", this);
    qmlWidget->setSource(QUrl("qrc:/Main.qml"));

    QObject *root = qmlWidget->rootObject();
    // if (root) {
    //     QObject *profileObj = root->findChild<QObject*>("profileManager");
    //     if (profileObj) {
    //         connect(profileObj, SIGNAL(keyConfigured(int,QString,QString)),
    //                 this, SLOT(onKeyConfigured(int,QString,QString)));
    //     }
    // }


    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(qmlWidget);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    createTrayIcon();
    
    connect(m_serialHandler, &SerialHandler::dataReceived,
        this, &MainWindow::onDataReceived);
    QObject::connect(&appTracker, &AppTracker::appChanged, this, &MainWindow::switchCurrentProfile);
}

MainWindow::~MainWindow() {
    /* if (keyboardHook) {
        UnhookWindowsHookEx(keyboardHook);
        keyboardHook = nullptr;
    } */
    qDeleteAll(profiles);
    profiles.clear();
}

// required profileCount function for QML_PROPERTY
qsizetype MainWindow::profileCount(QQmlListProperty<Profile> *list) {
    auto profiles = static_cast<QList<Profile*>*>(list->data);
    return profiles->size();
}

// required profileAt function for QML_PROPERTY
Profile *MainWindow::profileAt(QQmlListProperty<Profile> *list, qsizetype index) {
    auto profiles = static_cast<QList<Profile*>*>(list->data);
    return profiles->at(index);
}

// getter for QML to access profiles
QQmlListProperty<Profile> MainWindow::getProfiles() {
    return QQmlListProperty<Profile>(
        this,
        &profiles, // use MainWindow instance as the data object
        &MainWindow::profileCount,
        &MainWindow::profileAt
        );
}

void MainWindow::setProfileInstance(Profile* profile) {
    if (profileInstance != profile) {
        profileInstance = profile;
        emit profileInstanceChanged();
    }
}

void MainWindow::initializeProfiles() {
    QString names[6] = {"General", "Profile 1", "Profile 2", "Profile 3", "Profile 4", "Profile 5"};
    QString apps[6] = {"", "Google Chrome", "Qt Creator", "MacroPad", "Discord", "Spotify"};

    for (int i = 0; i < 6; ++i) {
        Profile* profile = Profile::loadProfile(names[i]);
        profiles.append(profile);
    }

    profileInstance = profiles[0];
    currentProfile = profiles[0];
}

void MainWindow::switchCurrentProfile(const QString& appName) {
    qDebug() << "Current app:" << appName;
    for (Profile* profile : profiles) {
        if (profile->getApp() == appName) {
            currentProfile = profile;
            qDebug() << "Current profile set to:" << currentProfile->getName();
            return;
        }
    }
}

void MainWindow::createTrayIcon() {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::warning(this, "Warning", "System tray is not available!");
        return;
    }

    // Set a valid icon (adjust path accordingly)
#ifdef Q_OS_MAC
    QString iconPath = QCoreApplication::applicationDirPath() + "/../Resources/MPIcon.png";
    QIcon icon(iconPath);
    // qDebug() << "Loading tray icon from:" << iconPath;
    // qDebug() << "File exists:" << QFile::exists(iconPath);
    // qDebug() << "Icon loaded successfully:" << !icon.isNull();
    trayIcon->setIcon(icon);
#endif

    trayIcon->setToolTip("Configuration Software Running");

    // Create tray menu actions
    QAction *restoreAction = new QAction("Show Window", this);
    QAction *exitAction = new QAction("Exit", this);

    connect(restoreAction, &QAction::triggered, this, &MainWindow::showWindow);
    connect(exitAction, &QAction::triggered, this, &MainWindow::exitApplication);

    // Add actions to menu
    trayMenu->addAction(restoreAction);
    trayMenu->addAction(exitAction);

    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (trayIcon->isVisible()) {
        hide();  // Hide the window
        event->ignore();  // Ignore the close event
        toggleDockIcon(false);
    }
}

void MainWindow::showWindow() {
    toggleDockIcon(true);
    showNormal();  // Restore window
    activateWindow();
}

void MainWindow::exitApplication() {
    trayIcon->hide();  // Hide tray icon before quitting
    QApplication::quit();
}

// Toggle Dock Icon on macOS
void MainWindow::toggleDockIcon(bool show) {
#ifdef Q_OS_MAC
    ProcessSerialNumber psn = { 0, kCurrentProcess };
    if (show) {
        TransformProcessType(&psn, kProcessTransformToForegroundApplication);
    } else {
        TransformProcessType(&psn, kProcessTransformToUIElementApplication);
    }
#endif

}

static void volumeUp()
{
#ifdef _WIN32
    qDebug() << "volumeUp called";
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_VOLUME_UP;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_VOLUME_UP;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));
#endif

#ifdef __APPLE__
    MainWindow::macVolume = (MainWindow::macVolume >= 100) ? MainWindow::macVolume : MainWindow::macVolume + 6;
    setSystemVolume(MainWindow::macVolume);
#endif
}

static void volumeDown()
{
#ifdef _WIN32
    qDebug() << "volumeDown called";
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_VOLUME_DOWN;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_VOLUME_DOWN;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));
#endif

#ifdef __APPLE__
    MainWindow::macVolume = (MainWindow::macVolume <= 0) ? MainWindow::macVolume : MainWindow::macVolume - 6;
    setSystemVolume(MainWindow::macVolume);
#endif
}

static void mute()
{
#ifdef _WIN32
    qDebug() << "mute called";
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_VOLUME_MUTE;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_VOLUME_MUTE;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));
#endif

#ifdef __APPLE__
    toggleMuteSystem();
#endif
}


// Scroll functions

static void scrollUp()
{
#ifdef _WIN32
    // qDebug() << "scrollUp called on Windows";
    // QScrollArea* scrollArea = ui->scrollArea;
    // QScrollBar* vScrollBar = scrollArea->verticalScrollBar();
    // if (vScrollBar) {
    //     vScrollBar->setValue(vScrollBar->value() - 50);
    // }
#endif

#ifdef __APPLE__
    // qDebug() << "scrollUp called on macOS";
    // QScrollArea* scrollArea = ui->scrollArea;
    // QScrollBar* vScrollBar = scrollArea->verticalScrollBar();
    // if (vScrollBar) {
    //     vScrollBar->setValue(vScrollBar->value() - 50);
    // }
#endif
}

static void scrollDown()
{
#ifdef _WIN32
    // qDebug() << "scrollDown called on Windows";
    // QScrollArea* scrollArea = ui->scrollArea;
    // QScrollBar* vScrollBar = scrollArea->verticalScrollBar();
    // if (vScrollBar) {
    //     vScrollBar->setValue(vScrollBar->value() + 50);
    // }
#endif

#ifdef __APPLE__
    // qDebug() << "scrollDown called on macOS";
    // QScrollArea* scrollArea = ui->scrollArea;
    // QScrollBar* vScrollBar = scrollArea->verticalScrollBar();
    // if (vScrollBar) {
    //     vScrollBar->setValue(vScrollBar->value() + 50);
    // }
#endif
}



// std::string pathNotion = "C:\\Users\\aarav\\AppData\\Local\\Programs\\Notion\\Notion.exe";
// std::wstring wpathNotion(pathNotion.begin(), pathNotion.end());


void MainWindow::onDataReceived(int number)
{
    // qDebug() << "onDataReceived: " << number;
    // if (number > 10 && number < 20)
    // {
    //     profile = number - 10;
    //     qDebug() << "Profile switched to " << profile;
    // }

    //Volume Control
    if (number > 70 && number < 80)
    {
        if (number == 72)
            volumeUp();
        else if (number == 71)
            volumeDown();
        else if (number == 73)
            mute();
        return;
    }

    //Key Detection
#ifdef _WIN32
    // if(number == 1)//Button 1 Pressed - Press Key 1 for Profile 0
    // {
    //     std::thread([] {
    //         extern std::wstring wpathNotion;
    //         ShellExecuteW(NULL, L"open", wpathNotion.c_str(), NULL, NULL, SW_SHOWNORMAL);
    //     }).detach();
    //     qDebug() << "Notion launched";
    // }
    // if(number == 2) //Button 2 Pressed
    // {
    //     std::thread([] {
    //         extern std::wstring wpathNotion;
    //         ShellExecuteW(NULL, L"open", wpathNotion.c_str(), NULL, NULL, SW_SHOWNORMAL);
    //     }).detach();
    //     qDebug() << "Arduino launched";
    // }

    if (number > 0 && number < 10) executeHotkey(number);

#endif

#ifdef __APPLE__
    if (number > 0 && number < 10) executeHotkey(number);
#endif
}

// ===== WINDOWS IMPLEMENTATION =====

#ifdef _WIN32

//std::string path = "C:\\Users\\aarav\\OneDrive\\Desktop\\Arduino IDE.lnk";
// std::string path = "Notepad";
//std::wstring wpath(path.begin(), path.end());  // Convert std::string to std::wstring


std::unordered_map<UINT, std::function<void()>> MainWindow::hotkeyActions;
std::unique_ptr<Profile> currentProfile = std::make_unique<Profile>("DefaultProfile");
//HHOOK MainWindow::keyboardHook = NULL;


LRESULT CALLBACK MainWindow::hotkeyCallback(int nCode, WPARAM wParam, LPARAM lParam){
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN) {
            int vkCode = kbdStruct->vkCode;

            // Check if the key has a registered action
            auto it = MainWindow::hotkeyActions.find(vkCode);
            if (it != MainWindow::hotkeyActions.end()) {
                it->second(); // Execute the stored action (macro)
                return 1;  // Prevents default key behavior (optional)
            }
        }
    }

    // Pass the event to the next hook in the chain
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

//week 8 keyboard version
void MainWindow::registerGlobalHotkey(Profile* profile, int keyNum, const QString& type, const QString& content){
#ifdef DEBUG
    UINT vkCode = 0;

    // Map keyNum to virtual key code (adjust mapping as needed)
    switch (keyNum) {
    case 1: vkCode = 0x31; break; //should be changed to macro keys after profile is loaded
    case 2: vkCode = 0x32; break;
    case 3: vkCode = 0x33; break;
    case 4: vkCode = 0x34; break;
    case 5: vkCode = 0x35; break;
    case 6: vkCode = 0x36; break;
    case 7: vkCode = 0x37; break;
    case 8: vkCode = 0x38; break;
    case 9: vkCode = 0x39; break;
    default:
        std::cerr << "Invalid key number specified.\n";
        return;
    }

    // Register the hotkey based on the type
    if (type == "executable") {
        std::wstring wcontent = content.toStdWString();
        hotkeyActions[vkCode] = [wcontent]() {
            ShellExecuteW(NULL, L"open", wcontent.c_str(), NULL, NULL, SW_SHOWNORMAL);
        };
    } else if (type == "keystroke") {
        hotkeyActions[vkCode] = [content]() {
            std::thread([content]() {

                // Define key mapping
                QMap<QString, int> keyMap = {
                    {"Cmd", VK_LWIN}, {"Shift", VK_SHIFT}, {"Ctrl", VK_CONTROL}, {"Alt", VK_MENU},
                    {"Space", VK_SPACE}, {"Enter", VK_RETURN}, {"Backspace", VK_BACK}, {"Tab", VK_TAB},
                    {"Esc", VK_ESCAPE}
                };

                for (char c = '0'; c <= '9'; ++c) {
                    keyMap[QString(c)] = c;
                }

                for (char c = 'A'; c <= 'Z'; ++c) {
                    keyMap[QString(c)] = c;
                }

                // Parse the key sequence
                QStringList keySequence = content.split("+");
                std::vector<int> keyCodes;

                for (const QString& key : std::as_const(keySequence)) {
                    if (keyMap.contains(key)) {
                        keyCodes.push_back(keyMap[key]);
                    }
                }

                // Press all keys
                for (int key : keyCodes) {
                    keybd_event(key, 0, 0, 0);
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Small delay

                // Release all keys (in reverse order)
                for (auto it = keyCodes.rbegin(); it != keyCodes.rend(); ++it) {
                    keybd_event(*it, 0, KEYEVENTF_KEYUP, 0);
                }
            }).detach();
        };
    } else {
        std::cerr << "Unsupported action type.\n";
    }

    // Ensure the keyboard hook is set
    if (!keyboardHook) {
        keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, hotkeyCallback, GetModuleHandle(NULL), 0);
    }
#endif
    profile->setMacro(keyNum, type, content);
}


std::string pathNotion = "C:\\Users\\aarav\\AppData\\Local\\Programs\\Microsoft \\VS Code\\Code.exe";
std::wstring wpathNotion(pathNotion.begin(), pathNotion.end());

std::wstring wpathExec;
void MainWindow::executeHotkey(int hotKeyNum)
{
    QSharedPointer<Macro> macro = profileInstance->getMacro(hotKeyNum);
    if (!macro.isNull()) {
        qDebug() << hotKeyNum << "key pressed! Type:" << macro->getType()
        << "Content:" << macro->getContent();

        const QString& type    = macro->getType();
        const QString& content = macro->getContent();

        // std::string s = content.toStdString();
        // std::replace(s.begin(), s.end(), '/', '\\');

        wpathExec = content.toStdWString();
        std::replace(wpathExec.begin(), wpathExec.end(), L'/', L'\\');
        wpathExec = wpathExec.substr(1);

        qDebug() << "corrected path" << wpathExec;

        if (type == "keystroke") {
            std::thread([content]() {
                QMap<QString, int> keyMap = {
                    {"Cmd", VK_LWIN}, {"Shift", VK_SHIFT}, {"Ctrl", VK_CONTROL}, {"Alt", VK_MENU},
                    {"Space", VK_SPACE}, {"Enter", VK_RETURN}, {"Backspace", VK_BACK}, {"Tab", VK_TAB},
                    {"Esc", VK_ESCAPE}
                };
                for (char c = '0'; c <= '9'; ++c) keyMap[QString(c)] = c;
                for (char c = 'A'; c <= 'Z'; ++c) keyMap[QString(c)] = c;

                QStringList keySequence = content.split("+");
                std::vector<int> keyCodes;
                for (const QString& key : std::as_const(keySequence)) {
                    if (keyMap.contains(key)) keyCodes.push_back(keyMap[key]);
                }

                for (int key : keyCodes) keybd_event(key, 0, 0, 0);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                for (auto it = keyCodes.rbegin(); it != keyCodes.rend(); ++it)
                    keybd_event(*it, 0, KEYEVENTF_KEYUP, 0);
            }).detach();
        }
        else if (type == "executable") { // need the executable paths in the correct format
            std::thread([] {
                extern std::wstring wpathExec;
                    ShellExecuteW(NULL, L"open", wpathExec.c_str(), NULL, NULL, SW_SHOWNORMAL);
                }).detach();
                qDebug() << content << " launched";
        }
    }
}
#endif

// ===== MACOS IMPLEMENTATION =====
#ifdef __APPLE__
#include <objc/NSObject.h>
#include <objc/objc.h>
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <QDebug>
#include <QProcess>
#include <QFileInfo>
#include <QDir>

#ifdef DEBUG
static EventHandlerUPP eventHandlerUPP;

static const std::map<int, int> keyMap = {
    {1, kVK_ANSI_1},
    {2, kVK_ANSI_2},
    {3, kVK_ANSI_3},
    {4, kVK_ANSI_4},
    {5, kVK_ANSI_5},
    {6, kVK_ANSI_6},
    {7, kVK_ANSI_7},
    {8, kVK_ANSI_8},
    {9, kVK_ANSI_9}
};
#endif

bool isAppBundle(const QString &path) {
    QFileInfo appInfo(path);

    // 1. Check if path exists and is a directory
    if (!appInfo.exists() || !appInfo.isDir()) {
        return false;
    }

    // 2. Verify if it ends with ".app"
    if (!path.endsWith(".app", Qt::CaseInsensitive)) {
        return false;
    }

    // 3. Check if it contains an executable inside "Contents/MacOS/"
    QDir macOSDir(path + "/Contents/MacOS");
    QFileInfoList files = macOSDir.entryInfoList(QDir::Files | QDir::Executable);

    return !files.isEmpty();  // Returns true if there is at least one executable file
}

OSStatus MainWindow::hotkeyCallback(EventHandlerCallRef nextHandler, EventRef event, void *userData){
    EventHotKeyID hotKeyID;
    GetEventParameter(event, kEventParamDirectObject, typeEventHotKeyID, NULL, sizeof(hotKeyID), NULL, &hotKeyID);

    executeHotkey(hotKeyID.id);

    return noErr;
}

void MainWindow::executeHotkey(int hotKeyNum) {
    QSharedPointer<Macro> macro = profileInstance->getMacro(hotKeyNum);

    if (!macro.isNull()) {
        qDebug() << hotKeyNum << "key pressed! Type:" << macro->getType() << "Content:" << macro->getContent();

        const QString& type = macro->getType();
        const QString& content = macro->getContent();

        if (macro->getType() == "keystroke") {

        } else if (macro->getType() == "executable") {
            if (isAppBundle(content)) {
                QProcess::startDetached("open", {"-a", content});
            } else {
                QProcess::startDetached(content);
            }
        }
    }
}

void MainWindow::registerGlobalHotkey(Profile* profile, int keyNum, const QString& type, const QString& content) {
    qDebug() << "registerGlobalHotkey called with:" << keyNum << type << content;
#ifdef DEBUG
    EventTypeSpec eventType;
    eventType.eventClass = kEventClassKeyboard;
    eventType.eventKind = kEventHotKeyPressed;

    EventHotKeyRef hotkeyRef;
    EventHotKeyID hotkeyID;
    hotkeyID.id = keyNum;

    // Create the event handler
    eventHandlerUPP = NewEventHandlerUPP(hotkeyCallback);
    InstallApplicationEventHandler(eventHandlerUPP, 1, &eventType, nullptr, nullptr);

    OSStatus status = RegisterEventHotKey(keyMap.at(keyNum), 0, hotkeyID, GetApplicationEventTarget(), 0, &hotkeyRef);

    if (status != noErr) {
        qDebug() << "Failed to register hotkey. Error code:" << status;
    } else {
        qDebug() << "Hotkey registered successfully!";
    }
#endif
    profile->setMacro(keyNum, type, content);
}
#endif


// ===== LINUX IMPLEMENTATION =====
#ifdef __linux__
#include <cstdlib>

void MainWindow::listenForHotkeys() {
    XEvent event;
    while (true) {
        XNextEvent(display, &event);
        if (event.type == KeyPress) {
            // Use 'notify-send' to show a notification without stealing focus
            system("notify-send 'MacroPad' 'Hotkey F5 Pressed!'");
        }
    }
}

void MainWindow::registerGlobalHotkey() {
    display = XOpenDisplay(NULL);
    if (!display) return;

    Window root = DefaultRootWindow(display);
    KeyCode keycode = XKeysymToKeycode(display, XK_F5);

    XGrabKey(display, keycode, AnyModifier, root, True, GrabModeAsync, GrabModeAsync);
    listenForHotkeys();
}
#endif

