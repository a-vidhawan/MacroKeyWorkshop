#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <QDebug>

bool setSystemVolume(int volume) {
  if (volume < 0) volume = 0;
  if (volume > 100) volume = 100;
  std::string command = "osascript -e 'set volume output volume " + std::to_string(volume) + "'";
  qDebug() << "Set system volume to" << volume;
  return (system(command.c_str()) == 0);
}

int getSystemVolume() {
  FILE* pipe = popen("osascript -e 'output volume of (get volume settings)'", "r");
  if (!pipe) return -1;

  char buffer[128];
  if (fgets(buffer, sizeof(buffer), pipe) == nullptr) {
      pclose(pipe);
      return -1;
  }
  pclose(pipe);

  try {
      return std::stof(buffer);
  } catch (...) {
      return -1;
  }
}

void toggleMuteSystem() {
  FILE* pipe = popen("osascript -e 'output muted of (get volume settings)'", "r");
  if (!pipe) return;

  char buffer[16];
  fgets(buffer, sizeof(buffer), pipe);
  pclose(pipe);

  bool isMuted = std::string(buffer).find("true") != std::string::npos;

  if (isMuted)
      std::system("osascript -e \"set volume without output muted\"");
  else
      std::system("osascript -e \"set volume with output muted\"");
}
