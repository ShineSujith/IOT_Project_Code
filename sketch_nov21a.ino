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
int buzzer_off = 0;
int daytime;
int nightmode = 0;
unsigned long mytime;
String myStatus;
//int morning, night;

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
  if(nightmode == 1) {// decreases the delay for the ultrasonic sensor
    ultra_sonic();
  }
  printLocalTime(); //function to get and print the time
  if(daytime > 9 && daytime < 21) {
    int analogValue = analogRead(LIGHT_SENSOR_PIN)/16; //returns a value from 0 - 255
    int light_level = (analogValue + 255) - (2 * analogValue); //used to flip that value i.e 255 becomes 0
    //Serial.println(light_level);
  	analogWrite(LED, light_level); //changes brightness of the LED
    nightmode = 0;
  }
  else {
    ultra_sonic(); //function to turn on the LED for 10 seconds using an ultrasonic sensor
    nightmode = 1;
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
  delay(20000);
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
  if(nightmode == 1) {
    ultra_sonic();
  }
  
  strftime(timeHour, 3, "%H", &timeinfo); //store byte values from &timeinfo into timehour
  strftime(timeMinute, 3, "%M", &timeinfo);
  strftime(timeWeekDay, 10, "%A", &timeinfo);
  strftime(timeMonth, 10, "%B", &timeinfo);
  strftime(timeDayMonth, 3, "%d", &timeinfo);
  //Serial.println("");
  //Serial.println(touchRead(BUTTON));
  daytime =  atoi(timeHour); //converts the string timeHour to an integer and stores it in daytime
  //turns on and off the alarm/buzzer
  if(daytime == alarm1) {
    if(touchRead(BUTTON) < 30) {
      buzzer_off = 1;
    }
    else if(buzzer_off == 1) {
      digitalWrite(BUZZER, LOW);
    }
    else {
      digitalWrite(BUZZER, HIGH);
    }
  }
  else {
    buzzer_off = 0;
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
