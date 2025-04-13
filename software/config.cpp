#include "config.h"
#include <iostream>
#include <QString>


#ifdef _WIN32
#include <shlobj.h>
#endif

QString Config::getConfigDir() {
    std::filesystem::path configPath;

#ifdef _WIN32 // C:\Users\username\AppData\Local\YourAppName\

    #include <minwindef.h>
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
        configPath = std::filesystem::path(path) / "MacroPad";
    }
#elif __APPLE__ // /Users/username/Library/Application Support/YourAppName/
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw->pw_dir;
    }

    configPath = std::filesystem::path(home) / "Library/Application Support/MacroPad";
#elif __linux__ // /home/username/.config/YourAppName/
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw->pw_dir;
    }

    configPath = std::filesystem::path(home) / ".config/MacroPad";

#endif

    // create the directory if one does not already exist
    if (!std::filesystem::exists(configPath)) {
        if(std::filesystem::create_directories(configPath)) {
            std::cout << "Created config directory: " << configPath << std::endl;
        } else {
            std::cerr << "Failed to create config directory!" << std::endl;
        }
    }

    return QString::fromStdString(configPath.string());

}
