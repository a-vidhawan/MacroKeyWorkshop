// INO for the MacroKey Workhop - SP25
// Support PCB 1 - With 2 Keys and 1 Volume Knob
// GitHub Repo - https://github.com/a-vidhawan/MacroKeyWorkshop

#define BUTTON1 7
#define BUTTON2 6

// Define rotary encoder pins

// The encoder pins can be seen in the schmeatic image on the README file.
// identify which pins pin 'A' and 'B' are connected to, via the schematic image.
#define ENC_A ___
#define ENC_B ___

#define MUTEBUTTON 0

unsigned long _lastIncReadTime = micros(); 
unsigned long _lastDecReadTime = micros(); 
int _pauseLength = 25000;
int _fastIncrement = 10;

volatile int counter = 0;

void setup() 
{

  Serial.begin(9600);

  pinMode(MUTEBUTTON, INPUT_PULLUP);
  
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);

  // Set encoder pins and attach interrupts
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENC_A), read_encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), read_encoder, CHANGE);

}

// look through 'ondataReceived' in 'mainwindow.cpp' on the github repo to fill the blanks.
void loop() 
{
  
  if(digitalRead(BUTTON1) == LOW) 
  {
    Serial.println("___");
    delay(250);
  }
  if(digitalRead(BUTTON2) == LOW) 
  {
    Serial.println("___");
    delay(250);
  }
  if(digitalRead(MUTEBUTTON) == LOW) 
  {
    Serial.println("___");
    delay(250);
  }

  static int lastCounter = 0;
  static bool buttonPressed = digitalRead(MUTEBUTTON);
  int buttonPressStart = millis();

  // If count has changed print the new value to serial
  if(counter < lastCounter){
    Serial.println(___); //volume DOWM
    lastCounter = counter;
  }
  else if (counter > lastCounter) {
    Serial.println(___); //volume UP
    lastCounter = counter;
  }
}

void read_encoder() {
  // Encoder interrupt routine for both pins. Updates counter
  // if they are valid and have rotated a full indent
 
  static uint8_t old_AB = 3;  // Lookup table index
  static int8_t encval = 0;   // Encoder value  
  static const int8_t enc_states[]  = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0}; // Lookup table

  old_AB <<=2;  // Remember previous state

  if (digitalRead(ENC_A)) old_AB |= 0x02; // Add current state of pin A
  if (digitalRead(ENC_B)) old_AB |= 0x01; // Add current state of pin B
  
  encval += enc_states[( old_AB & 0x0f )];

  // Update counter if encoder has rotated a full indent, that is at least 4 steps
  if( encval > 3 ) {        // Four steps forward
    int changevalue = 1;
    if((micros() - _lastIncReadTime) < _pauseLength) {
      changevalue = _fastIncrement * changevalue; 
    }
    _lastIncReadTime = micros();
    counter = counter + changevalue;              // Update counter
    encval = 0;
  }
  else if( encval < -3 ) {        // Four steps backward
    int changevalue = -1;
    if((micros() - _lastDecReadTime) < _pauseLength) {
      changevalue = _fastIncrement * changevalue; 
    }
    _lastDecReadTime = micros();
    counter = counter + changevalue;              // Update counter
    encval = 0;
  }

}
