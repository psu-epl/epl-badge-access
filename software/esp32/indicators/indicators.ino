/*
  ledcWrite_RGB.ino
  Runs through the full 255 color spectrum for an rgb led
  Demonstrate ledcWrite functionality for driving leds with PWM on ESP32

  This example code is in the public domain.

  Some basic modifications were made by vseven, mostly commenting.
*/
#define in4 34   //GPIO 34
int authorized = 0;
int id = 1;

// Set up the rgb led names
uint8_t ledR = 13;  //Red
uint8_t ledG = 12;  //Green
uint8_t ledB = 33;  //Blue
const int buzzer = 4; //buzzer to arduino pin 4
uint8_t ledArray[3] = {1, 2, 3}; // three led channels
const boolean invert = true; // set true if common anode, false if common cathode
uint8_t color = 0;          // a value from 0 to 255 representing the hue
uint32_t R, G, B;           // the Red Green and Blue color components
uint8_t brightness = 255;  // 255 is maximum brightness, but can be changed.  Might need 256 for common anode to fully turn off.

// the setup routine runs once when you press reset:
void setup()
{
  Serial.begin(115200);
  delay(10);

  pinMode(in4, INPUT);
  ledcAttachPin(ledR, 1); // assign RGB led pins to channels
  ledcAttachPin(ledG, 2);
  ledcAttachPin(ledB, 3);
  
  // Initialize channels
  // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
  // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
  ledcSetup(1, 12000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(2, 12000, 8);
  ledcSetup(3, 12000, 8);
  authorized = in4;

  delay(10);
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 4 as an output
}

// void loop runs over and over again
void loop() 
{  
  // If your RGB LED turns off instead of on here you should check if the LED is common anode or cathode.
  // If it doesn't fully turn off and is common anode try using 256.
  //RGB <(LED No., Brightness)>
  //LED No.: 1 = Red, 2 = Green, 3 = Blue.

  //Uncomment to test RGB
  /*
    Serial.println("2 seconds delay between cycles.");
    Serial.println("Red on.");
    ledcWrite(1, 255);    //Red on
    delay(2000);
    Serial.println("Red off.");
    ledcWrite(1, 0);      //Red off
    delay(2000);

    Serial.println("Green on.");
    ledcWrite(2, 255);    //Green on
    delay(2000);
    Serial.println("Green off.");
    ledcWrite(2, 0);      //Green off
    delay(2000);

    Serial.println("Blue on.");
    ledcWrite(3, 255);    //Blue on
    delay(2000);
    Serial.println("Red off.");
    ledcWrite(3, 0);      // Blue off
    delay(500);
  */
  
  //Default state
  default_state();

  //Test authorization
  while(authorized == 1){
  authorize_student(id);
  }
 
}

  void default_state() {
  Serial.println("State: Idle.");
  ledcWrite(1, 255); //Red
  delay(2000);
}

  void authorize_student(int id){
  
  if (id == authorized) {
    Serial.println("Student authorized");
    ledcWrite(2, 255); //Green

    //Buzzer beep once
    digitalWrite(buzzer, HIGH);
    Serial.println("buzz briefly");
    delay(100);
    digitalWrite(buzzer, LOW);

    //Buzzer beep 3 times
    for (int i = 0; i < 3; i++) {
      digitalWrite(buzzer, HIGH);
      delay(100);
      digitalWrite(buzzer, LOW);
      delay(900);
    }
    Serial.println("TEST: buzzed 3 times successfully");
    delay(5000);
  }
}
