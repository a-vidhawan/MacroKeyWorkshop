#ifndef MACVOLUME_H
#define MACVOLUME_H

bool setSystemVolume(int volume); // volume between 0.0 and 1.0
int getSystemVolume();
void toggleMuteSystem();

#endif // MACVOLUME_H
