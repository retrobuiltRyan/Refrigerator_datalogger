/* micro SD card Datalogger for:
DHT22 Temp and Humidity
Photo Resistor
TMP36 Temp Sensor (physical)
Copy and pasted by Ryan, poorly. 
*/
const int analogTemp1 = A0;   // Analog input pin for TMP36 Temp sensor
const int analogTemp2 = A1;   // Analog input pin for TMP36 Temp sensor
int CPUtempDiv = 3;
int RAMtempDiv = 3;
const int analogInPin2 = A2;   // Analog input pin that the photoresistor is attached to
int sensorValueA = 0;          // value read from the photoresistor

const int analogInPin3 = A3;   // Analog input pin that the photoresistor is attached to
int sensorValueB = 0;          // value read from the photoresistor
int led4 = 4;  
int led5 = 5; 
int led6 = 6;        

byte DIV_0_OF_5[8] = {
  B00000, 
  B00000,
  B00000,
  B11011,
  B00000,
  B00000,
  B00000,
  B00000
}; // 0 / 5

byte DIV_1_OF_5[8] = {
  B10000, 
  B10000,
  B10000,
  B11011,
  B10000,
  B10000,
  B10000,
  B00000
}; // 1 / 5

byte DIV_2_OF_5[8] = {
  B11000, 
  B11000,
  B11000,
  B11011,
  B11000,
  B11000,
  B11000,
  B00000
}; // 2 / 5 

byte DIV_3_OF_5[8] = {
  B11100, 
  B11100,
  B11100,
  B11111,
  B11100,
  B11100,
  B11100,
  B00000
}; // 3 / 5

byte DIV_4_OF_5[8] = {
  B11110, 
  B11110,
  B11110,
  B11111,
  B11110,
  B11110,
  B11110,
  B00000
}; // 4 / 5

byte DIV_5_OF_5[8] = {
  B11111, 
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000
}; // 5 / 5

byte thermometer[8] = {
  B00100, 
  B01010,
  B01010,
  B01010,
  B01010,
  B10001,
  B10001,
  B01110
}; 

// include the library code
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
const int LCD_NB_ROWS = 2 ;
const int LCD_NB_COLUMNS = 5 ;

//-blink without delay stuff
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 4000;           // interval at which to blink (milliseconds)
int screenState = LOW;

LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address for a 16 chars and 2 line display

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2  // Digital pin connected to the DHT sensor
#include <SPI.h>
//#include <SD.h>
uint32_t delayMS;


// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
DHT_Unified dht(DHTPIN, DHTTYPE);
const int chipSelect = 10;
float Ftemp;

void setup() {
    
    Serial.begin(9600);
 
    pinMode(led4,OUTPUT);  pinMode(led5,OUTPUT);  pinMode(led6,OUTPUT);
    digitalWrite(led6,HIGH); delay(250);   //red
    digitalWrite(led5,HIGH); delay(250);  //yellow
    digitalWrite(led4,HIGH); delay(250);  //green
    digitalWrite(led4,LOW);delay(250);  digitalWrite(led5,LOW);delay(250); digitalWrite(led6,LOW);
    //digitalWrite(leds[random(0,sizeof(leds)/sizeof(int))],LOW);

lcd.init(); //initialize the lcd
lcd.backlight(); //open the backlight
  lcd.begin(LCD_NB_COLUMNS ,LCD_NB_ROWS);
  setup_progressbar();
  
  lcd.clear();
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  digitalWrite(led4,HIGH);  //turn green led one

Serial.println("Data format is:");
Serial.println("Temp_F, %Humitidy, CPU, RAM, time(sec) elasped");
}


void loop() {
  setup_progressbar();
  unsigned long currentMillis = millis();
  
  //---------------------TMP36 sensor---------------------------------
 delay(50); int Temp1 = analogRead(analogTemp1); 
 delay(50); int Temp2 = analogRead(analogTemp2); //added some delay, reading a sensor right after the previous one gives weird readings.

 //-------TMP36 conversions--------------------------
 float voltage1 = Temp1 * 5.0;
 voltage1 /=  1024.0;
 float Temp1C = (voltage1 - 0.5) * 100 ;
 float Temp1F = (Temp1C * 9.0 / 5.0) + 32.0;
 
//-------TMP36 conversions--------------------------
 float voltage2 = Temp2 * 5.0;
 voltage2 /=  1024.0;
 float Temp2C = (voltage2 - 0.5) * 100 ;
 float Temp2F = (Temp2C * 9.0 / 5.0) + 32.0;  //

 
int timeSec = millis()/1000;

  
  //-------------DHT11 Temp & Humidity----------------------------
  digitalWrite(led5,LOW);
  delay(delayMS);
  digitalWrite(led5,HIGH);
  sensors_event_t event; // Get temperature event and print its value.
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    //Serial.print(F("Temperature: "));
    //Serial.print((1.8*event.temperature+32));
    Ftemp = ((event.temperature)*1.8)+31;
  }
  dht.humidity().getEvent(&event); // Get humidity event and print its value.
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    //Serial.print(F("Humidity: "));
    //Serial.println(event.relative_humidity);
    //Serial.println(F("%"));
  }
//-------------Formatting for SD card writing----------------------------
//// make a single line string for assembling the data to log:
  String dataString = ""; // this is a string now, its not a numerical value....
  dataString += String((Ftemp));                         dataString += ",";
  dataString += String(round(event.relative_humidity));  dataString += ",";
  dataString += String(round(Temp1F));                   dataString += ",";
  dataString += String(round(Temp2F));                   dataString += ",";
  dataString += String(timeSec);
    

//---------------------------blinky function------------------
  digitalWrite(led6,HIGH); delay(50);
  Serial.println(dataString);  // print to the serial port too:
  digitalWrite(led6,LOW);
//-----------------------print to LCD-----------------------------
  
  

  if (currentMillis - previousMillis >= interval) {
    // save the last time you switched the LCD
    previousMillis = currentMillis;

    //--switch between two pages LCD-------------------------------------:
    if (screenState == LOW) {
      screenState = HIGH;
      lcd.clear();
      lcd.setCursor(0,0); lcd.print("CPU "); lcd.print(round(Temp1F)); lcd.write(0b11011111); lcd.print("F ");
      draw_progressbar1(Temp1F); //Displays the value CPU
      
      lcd.setCursor(0,1); lcd.print("RAM "); lcd.print(round(Temp2F)); lcd.write(0b11011111); lcd.print("F ");
      draw_progressbar2(Temp2F); //Displays the value RAM
      
    } else {
      screenState = LOW;
      lcd.clear();
      lcd.setCursor(0,0); lcd.print("CPU "); lcd.print(round(Temp1F)); lcd.write(0b11011111);
      lcd.setCursor(9,0); lcd.print("RAM "); lcd.print(round(Temp2F)); lcd.write(0b11011111);
      
      lcd.setCursor(0,1); lcd.print("Air:"); lcd.print(round(Ftemp)); lcd.write(0b11011111); lcd.print("F "); 
      lcd.setCursor(9,1); lcd.print("RH "); lcd.print(round(event.relative_humidity)); lcd.print("%");
      //alarm functions
      if (Temp1F>= 100){ lcd.setCursor(9,0); lcd.print("CPU!"); lcd.write(6);} 
      if (Temp2F>= 100){ lcd.setCursor(9,0); lcd.print("RAM!"); lcd.write(6);}
    }}


  //-------some LCD animations------
  
  
}

void setup_progressbar() {
  lcd.createChar(0, DIV_0_OF_5);
  lcd.createChar(1, DIV_1_OF_5);
  lcd.createChar(2, DIV_2_OF_5);
  lcd.createChar(3, DIV_3_OF_5);
  lcd.createChar(4, DIV_4_OF_5);
  lcd.createChar(5, DIV_5_OF_5);
  lcd.createChar(6, thermometer);
}

  void draw_progressbar1(byte Temp1F) {
  lcd.setCursor(9,0); lcd.write(0b01011011); lcd.setCursor(15,0);lcd.write(0b0001011101); //prints "[" and "]"  
  lcd.setCursor(10,0);
  byte nb_columns = map(Temp1F ,60 ,150 ,0 ,LCD_NB_COLUMNS * 5); // Map the range(0~150) to the range(0~LCD_NB_COLUMNS*5) 
  for(byte i = 0 ; i < LCD_NB_COLUMNS ; ++ i) { //Draw each character per line
  if(nb_columns == 0) { // empty box
  lcd.write((byte) 0);
  } else if(nb_columns >= 5) { // Full case
  lcd.write(5);
  nb_columns -= 5 ;

  } else { // Last box not empty
  lcd.write(nb_columns);
  nb_columns = 0 ;
  }}}

   void draw_progressbar2(byte Temp2F) {
  lcd.setCursor(9,1); lcd.write(0b01011011); lcd.setCursor(15,1);lcd.write(0b0001011101); //prints "<" and ">"  
  lcd.setCursor(10,1);
  byte nb_columns = map(Temp2F ,60 ,150 ,0 ,LCD_NB_COLUMNS * 5); // Map the range(0~150) to the range(0~LCD_NB_COLUMNS*5) 
  for(byte i = 0 ; i < LCD_NB_COLUMNS ; ++ i) { //Draw each character per line
  if(nb_columns == 0) { // empty box
  lcd.write((byte) 0);
  } else if(nb_columns >= 5) { // Full case
  lcd.write(5);
  nb_columns -= 5 ;

  } else { // Last box not empty
  lcd.write(nb_columns);
  nb_columns = 0 ;
  }}}
 
