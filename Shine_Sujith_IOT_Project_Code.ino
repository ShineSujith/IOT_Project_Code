//code for lamp
#include <DFRobot_DHT11.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ThingSpeak.h>
#include "time.h"
#include "rgb_lcd.h"
#include "homepage.h"
#include "secrets.h"

rgb_lcd lcd;
DFRobot_DHT11 DHT;
WiFiClient  client;

#define DHT11_PIN 4
#define TRIG_PIN 5
#define ECHO_PIN 18
#define BUTTON 27
#define BUZZER 17
#define LED 19
#define LIGHT_SENSOR_PIN 34

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

WebServer server(80);

//used to calculate temperature value that will be sent to the web server
String getTemp() {
  DHT.read(DHT11_PIN);
  String temp = String(DHT.temperature);
  return temp;
}

//used to calculate humidity value that will be sent to the web server
String getHumidity() {
  DHT.read(DHT11_PIN);
  String humid = String(DHT.humidity);
  return humid;
}

//values used to config time
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

//strings used to store time values
char timeHour[3];
char timeMinute[3];
char timeWeekDay[10];
char timeMonth[10];
char timeDayMonth[3];

int alarm1 = 25;
char alarm2[3];
int buzzerOff = 0;
int daytime;
int nightMode = 0;
int sliderValue;
int currentMode = 1;
int alarmOff = 0;
String myStatus;


void handleRoot() {
  //creates a string that will be sent to the web server
  String message = homePagePart1 + getTemp() + homePagePart2 + getHumidity() + homePagePart3;
  String message1 = homePagePart4 + getTemp() + homePagePart2 + getHumidity() + homePagePart3;

  if(daytime >= 9 && daytime <= 21) {
    server.send(200, "text/html", message); //message is sent during daytime hours
  }
  else {
    server.send(200, "text/html", message1); //message1 is sent during night-time hours
  }
}

//used to send an error message if you cant connect to the server
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleKeyPress() {
  String recievedData = "You pressed: " + server.arg("button");
  currentMode = recievedData.toInt();
  server.send(200);
}

void handleSlider() {
  String recievedSliderInput = server.arg("slider");
  sliderValue = recievedSliderInput.toInt();
  server.send(200);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //initializes serial monitor at 115200 baud
  
  lcd.begin(16, 2); //initalizes 16x2 LCD
  lcd.setRGB(colorR, colorG, colorB); 

  ThingSpeak.begin(client); // Initialize ThingSpeak
  WiFi.begin(ssid, pass); //connects to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");//prints dots while waiting to connect to the wifi
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/sentKeyPressToWebServer", handleKeyPress);
  server.on("/sentSliderInputToWebServer", handleSlider);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  //pins are set as inputs or outputs
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED, OUTPUT);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);//configures time

  //used to set alarm from the serial monitor
  Serial.println("Set alarm");
  Serial.println(" ");
  while(alarm1 > 24) {
    if(Serial.available() > 0) {
      for(int i= 0 ; i < 2; i++) {
        alarm2[i] = Serial.read();
      }
      alarm1 = atoi(alarm2);
    }
    delay(1000);
  }
  Serial.print("alarm set to: ");
  Serial.println(alarm1);
}

void loop() {
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks
  delay(1000);

  if(nightMode == 1) {// decreases the delay for the ultrasonic sensor
    ultra_sonic();
  }
  printLocalTime(); //function to get and print the time

  int analogValue = 0;
  switch(currentMode) {
    case 0:
      ultra_sonic(); //function to turn on the LED for 10 seconds using an ultrasonic sensor
      nightMode = 1;
    break;
    case 1:
      analogValue = analogRead(LIGHT_SENSOR_PIN)/16; //returns a value from 0 - 255
      //int light_level = (analogValue + 255) - (2 * analogValue); //used to flip that value i.e 255 becomes 0
      //Serial.println(analogValue);
      analogWrite(LED, analogValue); //changes brightness of the LED
      nightMode = 0;
    break;
    case 2:
      analogWrite(LED, sliderValue);
    break;
    default:
      Serial.println("Pick a mode on the website");
    break;
  }
  //reads temperature and humidity values from the DHT11 and displays them on the LCD
  DHT.read(DHT11_PIN);
  lcd.clear();
  lcd.print("temp:");
  lcd.print(DHT.temperature);
  lcd.print(" Degrees");
  lcd.setCursor(0, 1);
  lcd.print("humi:");
  lcd.print(DHT.humidity);
  lcd.print(" Percent");

  
  ThingSpeak.setField(1, DHT.temperature);
  ThingSpeak.setField(2, DHT.humidity);

  if(DHT.temperature > 25) {
    myStatus = String("Temperature is too high.");
  }
  else if(DHT.temperature < 15) {
    myStatus = String("Temperature is too low.");
  }
  else {
    myStatus = String("Temperature is fine.");
  }

  ThingSpeak.setStatus(myStatus);
  int j = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if(j == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(j));
  }
  //delay(20000);
}

void printLocalTime(){
  struct tm timeinfo; //%A = day of week, %B = month, %d = day of month, %Y = year, %H = hour, %M = minute, %S = second
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  lcd.clear();
  lcd.print(&timeinfo, "%A, %d,");
  lcd.setCursor(0, 1);
  lcd.print(&timeinfo,"%B %H:%M");
  delay(1000);
  if(nightMode == 1) {
    ultra_sonic();
  }
  
  strftime(timeHour, 3, "%H", &timeinfo); //store byte values from &timeinfo into timehour
  strftime(timeMinute, 3, "%M", &timeinfo);
  strftime(timeWeekDay, 10, "%A", &timeinfo);
  strftime(timeMonth, 10, "%B", &timeinfo);
  strftime(timeDayMonth, 3, "%d", &timeinfo);
  //Serial.println("");
  Serial.println(touchRead(BUTTON));
  daytime =  atoi(timeHour); //converts the string timeHour to an integer and stores it in daytime
  //turns on and off the alarm/buzzer
  if(daytime == alarm1) {
    if(alarmOff == 0) {
      if(touchRead(BUTTON) < 30) {
        buzzerOff = 1;
      }
      else if(buzzerOff == 1) {
        digitalWrite(BUZZER, LOW);
        buzzerOff = 0;
        alarmOff = 1;
      }
      else {
        digitalWrite(BUZZER, HIGH);
      }
    }
  }
  else {
    buzzerOff = 0;
    alarmOff = 0;
  }
}

void ultra_sonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH); //measures how long the echo pins stays high for
  float distance = duration * 0.034 / 2; //calculates the distance
  //Serial.println(distance);

  if (distance < 20) {
    //turns on the LED for 10 seconds
    digitalWrite(LED, HIGH);
    for(int x = 0; x < 10; x++) {
      delay(1000);
    }
    digitalWrite(LED, LOW);
  }
}

// // code for wristband
// #include <Adafruit_MPU6050.h>
// #include <Adafruit_Sensor.h>
// #include <WiFi.h>
// #include <WiFiClient.h>
// #include <ThingSpeak.h>
// #include "time.h"
// #include "Wire.h"
// #include "secrets.h"
// #include "DFRobot_Heartrate.h"

// #define HEART_RATE_PIN 4

// DFRobot_Heartrate heartrate(DIGITAL_MODE); // ANALOG_MODE or DIGITAL_MODE
// Adafruit_MPU6050 mpu;
// WiFiClient  client;

// char ssid[] = SECRET_SSID;   // your network SSID (name) 
// char pass[] = SECRET_PASS;   // your network password

// unsigned long myChannelNumber = SECRET_CH_ID;
// const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// //values used to config time
// const char* ntpServer = "pool.ntp.org";
// const long  gmtOffset_sec = 0;
// const int   daylightOffset_sec = 3600;

// int date, hrs1, min1, sec1;
// int awakeFlag = 1;
// hw_timer_t *MyTimer = NULL;

// char timeDayMonth[3];

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(115200);
//   ThingSpeak.begin(client); // Initialize ThingSpeak
//   WiFi.begin(ssid, pass);   //connects to wifi
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");//prints dots while waiting to connect to the wifi
//   }
//   Serial.println("");
//   Serial.print("Connected to ");
//   Serial.println(ssid);
//   Serial.print("IP address: ");
//   Serial.println(WiFi.localIP());
//   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);//configures time

//   // Try to initialize!
//   if (!mpu.begin()) {
//     Serial.println("Failed to find MPU6050 chip");
//     while (1) {
//       delay(10);
//     }
//   }
//   Serial.println("MPU6050 Found!");

//   //setupt motion detection
//   mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
//   mpu.setMotionDetectionThreshold(1);
//   mpu.setMotionDetectionDuration(20);
//   mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
//   mpu.setInterruptPinPolarity(true);
//   mpu.setMotionInterrupt(true);

//   Serial.println("");
//   delay(100);
//   MyTimer = timerBegin(0, 80, true);            
//   timerAttachInterrupt(MyTimer, &sleep, true);  //attaches interrupt
//   timerAlarmWrite(MyTimer, 1000000, true);      //set the delay to 1 sec
//   timerAlarmEnable(MyTimer);                    //enables interrupt
//   findDate();
//   temp = date;
// }

// void IRAM_ATTR sleep(){
//   if(awakeFlag == 0) {
//     sec++;
//     if(sec > 59) {
//       sec = 0;
//       min1++;
//     }
//     Serial.println(sec);
//   }
//   hrs = min1/60;
//   awakeFlag = 1;
// }

// void loop() {
//   ThingSpeak.setField(3, hrs1);
//   findDate();
//   if(temp != date) {
//     temp = date;
//     ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
//     hrs1 = 0;
//     min1 = 0;
//     sec1 = 0;
//   }

//   //code for DIGITAL_MODE
//   uint8_t rateValue;
//   heartrate.getValue(HEART_RATE_PIN); // samples values from pin
//   rateValue = heartrate.getRate(); // Get heart rate value 
//   if(rateValue)  {
//     if(mpu.getMotionInterruptStatus()) {
//       awakeFlag = 1;
//     }
//     else {
//       awakeFlag = 0;
//     }
//     Serial.println(rateValue);
//   }
//   delay(20);

//   //code for ANALOG_MODE
//   /*  int sensorValue = analogRead(HEART_RATE_PIN);
//   Serial.println(sensorValue);
//   delay(10);*/
// }

// void findDate(){
//   struct tm timeinfo; //%A = day of week, %B = month, %d = day of month, %Y = year, %H = hour, %M = minute, %S = second
//   if(!getLocalTime(&timeinfo)){
//     Serial.println("Failed to obtain time");
//     return;
//   }
//   strftime(timeDayMonth, 3, "%d", &timeinfo);
//   date = atoi(timeDayMonth);
// }