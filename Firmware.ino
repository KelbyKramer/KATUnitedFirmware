#if 1 
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#include <stdint.h>
#include <pin_magic.h>
#include <registers.h>

#define MINPRESSURE 10
#define MAXPRESSURE 1000

const int trigPin = 24;
const int echoPin = 22;

#include <Servo.h> // include servo library to use its related functions
#define Servo_PWM 41 // A descriptive name for D6 pin of Arduino to provide PWM signal
Servo MG995_Servo;  // Define an instance of of Servo with the name of "MG995_Servo"

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9  // can be a digital pin

const int TS_LEFT = 907, TS_RT = 136, TS_TOP = 942, TS_BOT = 139;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button oneDay, threeDay, fiveDay, startBtn, stopBtn, countdown;


int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        //pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        //pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
        pixel_x = map(p.x, TS_RT, TS_LEFT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_BOT, TS_TOP, 0, tft.height());
    }
    return pressed;
}
 unsigned long previousMillis = 0;
 long val = 300;
 const long interval = 1000;
 
void runMotor(){
  Serial.println("The motor is running");// You can display on the serial the signal value
  MG995_Servo.attach(Servo_PWM);//Always use attach function after detach to re-connect your servo with the board
  MG995_Servo.write(180);
  delay(4360);
  MG995_Servo.detach();//Stop
  delay(2000);
  //set value back to 10
  val = 300;
}

void stopMotor(){
  Serial.println("Stopping the motor");
  MG995_Servo.detach();
}


#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

void setup(void)
{
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    Serial.begin(9600);
    MG995_Servo.attach(Servo_PWM);  // Connect D6 of Arduino with PWM signal pin of servo motor

    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    Serial.println("Calibrate for your Touch Panel");
    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(ID);
    tft.setRotation(3);            //PORTRAIT TODO
    tft.fillScreen(BLACK);
    countdown.initButton(&tft,  160, 80, 240, 80, WHITE, WHITE, BLACK, "", 2);
    startBtn.initButton(&tft,  100, 20, 80, 40, WHITE, GREEN, BLACK, "START", 2);
    stopBtn.initButton(&tft,  220, 20, 80, 40, WHITE, RED, BLACK, "STOP", 2);
    oneDay.initButton(&tft,  40, 200, 80, 40, WHITE, CYAN, BLACK, "1 DAY", 2);
    threeDay.initButton(&tft, 160, 200, 80, 40, WHITE, CYAN, BLACK, "3 DAYS", 2);
    fiveDay.initButton(&tft, 280, 200, 80, 40, WHITE, CYAN, BLACK, "5 DAYS", 2);
    countdown.drawButton(false);
    startBtn.drawButton(false);
    stopBtn.drawButton(false);
    oneDay.drawButton(false);
    threeDay.drawButton(false);
    fiveDay.drawButton(false);
    //tft.fillRect(40, 80, 240, 80, WHITE);
}

long duration;
int distance;

bool distanceCheck(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  
  if(distance > 17){
    return true;
  }
  else{
    return false;
  }
}

String convertToTime(long count){
  long days = count / 86400;
  count -= days*86400;
  long hours = count / 3600;
  count -= hours*3600;

  long minutes = count / 60;
  count -= minutes*60;

  long seconds = count;

  String ret = String(days) + ":" + String(hours) + ":" + String(minutes) + ":" + String(seconds);
  return ret;
  
}

void loop(void)
{
  bool down = Touch_getXY();
  //if the screen was touched
  if(down){
    if(startBtn.contains(pixel_x, pixel_y)){
      Serial.println("Start button");
      bool distance = distanceCheck();

      if(distance){
        
        Serial.println("There motor is running");
        tft.fillRect(60, 40, 220, 60, WHITE);
        tft.setCursor(80,80);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("The motor is running");
        val = 0;
        runMotor();
      }
      else{
        Serial.println("There is something in the way");
        tft.fillRect(60, 40, 220, 60, WHITE);
        tft.setCursor(80,80);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("There is something in the way");
        val = 0;
      }
      
    }
    else if(stopBtn.contains(pixel_x, pixel_y)){
      Serial.println("Stop button");
      stopMotor();
    }
    else if(oneDay.contains(pixel_x, pixel_y)){
      Serial.println("One Day");
      val = 86400;
    }
    else if(threeDay.contains(pixel_x, pixel_y)){
      Serial.println("Three Day");
      val = 259200;
    }
    else if(fiveDay.contains(pixel_x, pixel_y)){
      Serial.println("Five Day");
      val = 432000;
    }
    else{
      Serial.println("Outside the range") ;
    }
  }
 
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    // if the LED is off turn it on and vice-versa:
    val--;
    String counter = convertToTime(val);
    tft.fillRect(60, 40, 220, 60, WHITE);
    tft.setCursor(160,80);
    tft.setTextColor(BLACK);
    tft.setTextSize(2);
    tft.print(counter);
    Serial.println(counter);
    //function to convert to dd:hh:mm:ss
    

  }

  if(val < 0){
    bool distanceCheckFlag = distanceCheck();
    if(distanceCheckFlag == true){
      Serial.println("The motor is running");
      tft.fillRect(60, 40, 220, 60, WHITE);
      tft.setCursor(80,80);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.print("The motor is running");
      runMotor();
    }
    else{
      Serial.println("There is something in the way");
      tft.fillRect(60, 40, 220, 60, WHITE);
      tft.setCursor(80,80);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.print("There is something in the way");
      val = 0;
    } 
    Serial.println("Disposal is being actuated");
  }  
}
#endif
