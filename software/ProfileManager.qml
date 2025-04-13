import QtQuick
import QtQuick.Controls
import QtCore 6.2
import FileIO 1.0
import Macro 1.0

Item {
    id: profileManager

    property var profiles: []
    property var profileNames: ["General", "Profile 1", "Profile 2", "Profile 3", "Profile 4", "Profile 5", "Profile 6"]

    FileIO {
        id: fileIO
        filePath: StandardPaths.writableLocation(StandardPaths.AppDataLocation) + "/profiles.json"
    }

    function loadProfiles() {
        var fileData = fileIO.read();
        if (fileData.length > 0) {
            var lines = fileData.split("\n");
            profiles = [];

            var currentProfile = null;

            for (var i = 0; i < lines.length; i++) {
                var line = lines[i].trim();
                if (line.startsWith("name:")) {
                    if (currentProfile) {
                        profiles.push(currentProfile);
                    }
                    currentProfile = { name: line.substring(6).trim(), keys: [] };
                } else if (line.match(/^\d+:/)) {
                    var keyIndex = parseInt(line.split(":")[0].trim());
                    var type = lines[i + 1].split(":")[1].trim();
                    var content = lines[i + 2].split(":")[1].trim();
                    currentProfile.keys[keyIndex - 1] = { type: type, content: content };
                    i += 2;
                }
            }

            if (currentProfile) {
                profiles.push(currentProfile);
            }
        } else {
            profiles = [{ name: "General", keys: new Array(9).fill(null).map(() => ({ type: "", content: "" })) }];
        }
    }

    function saveProfiles() {
        if (profiles.length === 0) {
            console.log("No profiles to save.");
            return;
        }

        var currentProfile = profiles[profileSelector.currentIndex];

        if (!currentProfile) {
            console.log("ERROR: No current profile selected.");
            return;
        }

        console.log("Saving profile:", currentProfile.name);

        profileInstance.setName(currentProfile.name);

        for (var i = 0; i < currentProfile.keys.length; i++) {
            var key = currentProfile.keys[i];
            if (key) {
                profileInstance.setMacro(i + 1, key.type, key.content);
            }
        }

        profileInstance.saveProfile();
        console.log("Profile saved successfully.");
    }


    function loadProfile(index) {
            console.log("Loading profile:", profileNames[index]);
            if (profiles[index] === undefined) {
                profiles[index] = { name: profileNames[index], keys: new Array(9).fill(null).map(() => ({ keystroke: "", executable: "" })) };
            }
            saveProfiles();
        }

    function setKeyConfig(keyIndex, type, value) {
        var profile = profiles[profileSelector.currentIndex];

        if (!profile.keys) {
            profile.keys = new Array(9).fill(null).map(() => ({ type: "", content: "" }));
        }

        if (!profile.keys[keyIndex - 1]) {
            profile.keys[keyIndex - 1] = { type: "", content: "" };
        }

        console.log("Updating key:", keyIndex, "Type:", type, "Value:", value);

        if (type === "keystroke" && value !== "") {
            profile.keys[keyIndex - 1].type = "keystroke";
            profile.keys[keyIndex - 1].content = value;
        } else if (type === "executable" && value !== "") {
            profile.keys[keyIndex - 1].type = "executable";
            profile.keys[keyIndex - 1].content = value;
        }

        saveProfiles();
    }

    Component.onCompleted: {
        loadProfiles();
    }
}

