#define PROFILES 6
#define IMAGES_PER_PROFILE 9
#define IMG_SIZE 64 * 64 * 3  // 12288 bytes (RGB)

enum State { NORMAL, RECEIVING };
State currentState = NORMAL;

uint8_t imageMatrix[PROFILES][IMAGES_PER_PROFILE][IMG_SIZE];

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 RGB Image Receiver Ready.");
}

void loop() {
  static uint16_t index = 0;
  static uint8_t profile = 0;
  static uint8_t imageSlot = 0;
  static bool waitingForHeader = true;

  if (Serial.available()) {
    if (currentState == NORMAL) {
      if (Serial.read() == 47) {
        currentState = RECEIVING;
        waitingForHeader = true;
        Serial.println("Entering RECEIVE mode.");
      }
    } else if (currentState == RECEIVING) {
      if (Serial.peek() == 47 && Serial.available() > 0) {
        Serial.read();  // END marker
        currentState = NORMAL;
        Serial.println("Exiting RECEIVE mode.");
        return;
      }

      if (waitingForHeader && Serial.available() >= 2) {
        profile = Serial.read();
        imageSlot = Serial.read();

        if (profile >= PROFILES || imageSlot >= IMAGES_PER_PROFILE) {
          Serial.println("Invalid profile/image index!");
          currentState = NORMAL;
          return;
        }

        index = 0;
        waitingForHeader = false;
        Serial.printf("Receiving RGB image: Profile %d, Slot %d\n", profile, imageSlot);
      }

      while (Serial.available() && index < IMG_SIZE) {
        imageMatrix[profile][imageSlot][index++] = "0x" + Serial.read() + Serial.read();
      }

      if (index == IMG_SIZE) {
        Serial.println("RGB image received and stored.");
        waitingForHeader = true;
      }
    }
  }
}