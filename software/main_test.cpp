#include <iostream>
#include "profile.h"

int main() {

    Profile testProfile("testProfile");

    testProfile.setMacro(1, "program", "/path/to/executable");

    testProfile.saveProfile("profile.txt");
    std::cout << "Profile saved to 'profile.txt'.\n";

    Profile loadedProfile = Profile::loadProfile("profile.txt");
    std::cout << "Profile loaded from 'profile.txt'.\n";

    std::cout << "Running Macro 1:\n";
    loadedProfile.runMacro(1);
    return 0;

}
